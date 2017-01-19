/*
 * Copyright 2017 Lorenz Haas <lorenz.haas@histomatics.de>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cppcrate/result.h>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <iostream>

namespace CppCrate {

/*!
 * \class CppCrate::Result
 *
 * \brief Encapsulates Crate's HTTP endpoint reply enriched by convenience functions for browsing
 *        thought the result set.
 *
 * The class %Result encapsulates the reply of a Crate HTTP endpoint request. In contrast to
 * RawResult it parses the reply and provides convenience function to access the result's
 * information directly.
 *
 * The following code for example shows how to iterate over all returned rows of a query:
 * \code
 * Result result = client.exec("SELECT name FROM players");
 * if (result) {
 *   for (int i = 0, total = result.recordSize(); i < total; ++i) {
 *     Record r = result.record(i);
 *     std::cout << r.value("name").asString() << "\n";
 *   }
 * } else {
 *   std::cout << result.errorString() << "\n";
 * }
 * \endcode
 */

/// \cond INTERNAL
class Result::Private {
 public:
  Private() : duration(0.0), rowCount(0) {}

  bool operator==(const Private& other) const {
    return rawResult == other.rawResult && errorString == other.errorString &&
           duration == other.duration && rowCount == other.rowCount && cols == other.cols &&
           colTypes == other.colTypes && rows == other.rows;
  }

  RawResult rawResult;
  std::string errorString;
  double duration;
  int rowCount;
  std::vector<std::string> cols;
  std::vector<CrateDataType> colTypes;
  std::vector<std::string> rows;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Result)

/*!
 * Constructs a result based on the raw result \a raw.
 */
Result::Result(const RawResult& raw) : p(new Private) {
  p->rawResult = raw;

  rapidjson::Document doc;
  doc.Parse(raw.reply());
  if (doc.HasParseError()) {
    p->errorString = "[json] Parse error at offset " + CPPCRATE_TO_STRING(doc.GetErrorOffset()) +
                     ": " + rapidjson::GetParseError_En(doc.GetParseError());
    return;
  }

  // Handle normal and bulk error
  // -----------------------------------------------------------------------------------------------

  if (doc.HasMember("error")) {
    if (doc["error"].IsObject()) {
      rapidjson::Value& error = doc["error"];
      if (error.HasMember("component")) {
        rapidjson::Value& component = error["component"];
        if (component.IsString()) {
          p->errorString.append(
              "[" + std::string(component.GetString(), component.GetStringLength()) + "] ");
        } else {
          p->errorString.append("[unknown] ");
        }
      } else {
        p->errorString.append("[crate] ");
      }
      if (error.HasMember("message")) {
        rapidjson::Value& message = error["message"];
        if (message.IsString()) {
          p->errorString.append(std::string(message.GetString(), message.GetStringLength()));
        } else {
          p->errorString.append("Unknown error.");
        }
      }
      if (error.HasMember("code")) {
        rapidjson::Value& code = error["code"];
        if (code.IsInt()) {
          p->errorString.append(" (" + CPPCRATE_TO_STRING(code.GetInt()) + ")");
        }
      }
      if (p->errorString.empty()) p->errorString = "Unknown error.";
    } else {
      p->errorString = "Unknown error.";
    }
    return;
  }

  // Handle bulk error (if any)
  if (doc.HasMember("results") && doc["results"].IsArray()) {
    std::vector<unsigned int> errorLines;
    const rapidjson::Value& results = doc["results"];
    for (unsigned int i = 0, total = results.Size(); i < total; ++i) {
      const rapidjson::Value& r = results[i];
      if (r.HasMember("rowcount") && r["rowcount"].IsInt() && r["rowcount"].GetInt() == -2) {
#ifdef ENABLE_CPP11_SUPPORT
        errorLines.emplace_back(i + 1);
#else
        errorLines.push_back(i + 1);
#endif
      }
    }
    if (!errorLines.empty()) {
      p->errorString = "[crate] Error in bulk arguments [" + CPPCRATE_TO_STRING(errorLines.front());
      for (std::size_t i = 1, total = errorLines.size(); i < total; ++i) {
        p->errorString.append(", " + CPPCRATE_TO_STRING(errorLines[i]));
      }
      p->errorString.append("].");
      return;
    }
  }

  // Extract information form JSON reply
  // -----------------------------------------------------------------------------------------------

  if (doc.HasMember("rowcount")) {
    const rapidjson::Value& rowcount = doc["rowcount"];
    if (rowcount.IsInt()) {
      p->rowCount = rowcount.GetInt();
    }
  }

  if (doc.HasMember("duration")) {
    const rapidjson::Value& duration = doc["duration"];
    if (duration.IsDouble()) {
      p->duration = duration.GetDouble();
    }
  }

  if (doc.HasMember("cols")) {
    const rapidjson::Value& cols = doc["cols"];
    if (cols.IsArray()) {
      for (rapidjson::Value::ConstValueIterator it = cols.Begin(), end = cols.End(); it != end;
           ++it) {
        if (it->IsString()) {
#ifdef ENABLE_CPP11_SUPPORT
          p->cols.emplace_back(it->GetString(), it->GetStringLength());
#else
          p->cols.push_back(std::string(it->GetString(), it->GetStringLength()));
#endif
        } else {
#ifdef ENABLE_CPP11_SUPPORT
          p->cols.emplace_back("");
#else
          p->cols.push_back("");
#endif
        }
      }
    }
  }

  if (doc.HasMember("col_types")) {
    const rapidjson::Value& colTypes = doc["col_types"];
    if (colTypes.IsArray()) {
      for (rapidjson::Value::ConstValueIterator it = colTypes.Begin(), total = colTypes.End();
           it != total; ++it) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        it->Accept(writer);
        const std::string definition = std::string(sb.GetString(), sb.GetSize());

        CrateDataType::Type type = CrateDataType::NotSupported;
        if (it->IsInt()) {
          type = CrateDataType::convert(it->GetInt());
        } else if (it->IsArray() || !it->Empty()) {
          const rapidjson::Value::ConstValueIterator innerType = it->Begin();
          if (innerType->IsInt()) {
            type = CrateDataType::convert(innerType->GetInt());
          }
        }
#ifdef ENABLE_CPP11_SUPPORT
        p->colTypes.emplace_back(type, definition);
#else
        p->colTypes.push_back(CrateDataType(type, definition));
#endif
      }
    }
  }

  if (doc.HasMember("rows")) {
    const rapidjson::Value& rows = doc["rows"];
    if (rows.IsArray()) {
      for (rapidjson::Value::ConstValueIterator it = rows.Begin(), total = rows.End(); it != total;
           ++it) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        it->Accept(writer);
#ifdef ENABLE_CPP11_SUPPORT
        p->rows.emplace_back(sb.GetString(), sb.GetSize());
#else
        p->rows.push_back(std::string(sb.GetString(), sb.GetSize()));
#endif
      }
    }
  }
}

/*!
 * Returns whether the result is valid.
 */
CppCrate::Result::operator bool() const { return !hasError(); }

/*!
 * Returns whether the result has an error.
 */
bool Result::hasError() const { return !p->errorString.empty(); }

/*!
 * Returns the error string.
 */
const std::string& Result::errorString() const { return p->errorString; }

/*!
 * Returns the raw result on which this result is based on. Use
 * \code
 * result.rawResult().reply();
 * \endcode
 * to get the HTTP endpoint's original reply.
 */
const RawResult& Result::rawResult() const { return p->rawResult; }

/*!
 * Returns the query's duration.
 */
double Result::duration() const { return p->duration; }

/*!
 * Returns the query's row count.
 * \note To iterate over the records use recordSize().
 */
int Result::rowCount() const { return p->rowCount; }

/*!
 * Returns the query's column names.
 */
const std::vector<std::string>& Result::cols() const { return p->cols; }

/*!
 * Returns the query's column types.
 */
const std::vector<CrateDataType>& Result::colTypes() const { return p->colTypes; }

/*!
 * Returns the query's rows.
 */
const std::vector<std::string>& Result::rows() const { return p->rows; }

/*!
 * Returns the amount of records that can be fetched using record().
 */
int Result::recordSize() const { return static_cast<int>(p->rows.size()); }

/*!
 * Returns the record on the position \a pos. If \a pos is outside the record's boundaries an empty
 * record is returned.
 */
Record Result::record(int pos) const {
  if (pos < 0) {
    return Record();
  }

  const std::size_t index = static_cast<std::size_t>(pos);
  if (index >= p->rows.size()) return Record();
  return Record(p->rows[index], p->cols, p->colTypes);
}

}  // namespace CppCrate
