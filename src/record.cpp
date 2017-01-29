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

#include <cppcrate/record.h>
#include "global_p.h"

#include <cppcrate/value.h>

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace CppCrate {

/*!
 * \class CppCrate::Record
 *
 * \brief Encapsulates the values of a single row inside a result.
 *
 * The class %Record encapsulates the values of a single row inside a Result.
 *
 *
 * To iterate over all values contained in a result use
 * \code
 * Record r;
 * for (int i = 0, total = r.size(); i < total; ++i) {
 *   Value value = r.value(i);
 *   // use value here
 * }
 * \endcode
 *
 * or use C++11's range-based for-loop, or use begin() and end() with STL algorithms.
 */

/// \cond INTERNAL
class Record::Private {
 public:
  bool operator==(const Private &other) const { return values == other.values; }
  std::vector<Value> values;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Record)

/*!
 * Constructs an empty record.
 */
Record::Record() : p(new Private) {}

/*!
 * Constructs a record holding the values as defined in \a data. The values' names are defined by
 * \a names and their types by \a types.
 */
Record::Record(const std::string &data, const std::vector<std::string> &names,
               const std::vector<CrateDataType> &types)
    : p(new Private) {
  rapidjson::Document doc;
  doc.Parse(data);
  if (doc.HasParseError() || !doc.IsArray()) {
    return;
  }

  for (unsigned int i = 0; i < doc.Size(); ++i) {
    const rapidjson::Value &v = doc[i];
    const std::string name = i < names.size() ? names[i] : "";
    const CrateDataType type =
        i < types.size() ? types[i] : CrateDataType(CrateDataType::NotSupported);

    // Because numbers are most likely the standard value process them prioritized.
    if (v.IsNumber()) {
      switch (type.type()) {
        case CppCrate::CrateDataType::Byte:
        case CppCrate::CrateDataType::Short:
          if (v.IsInt()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, static_cast<int16_t>(v.GetInt()));
#else
            p->values.push_back(Value(name, type, static_cast<int16_t>(v.GetInt())));
#endif
            continue;
          }
          break;
        case CppCrate::CrateDataType::Integer:
          if (v.IsInt()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, static_cast<int32_t>(v.GetInt()));
#else
            p->values.push_back(Value(name, type, static_cast<int32_t>(v.GetInt())));
#endif
            continue;
          }
          break;
        case CppCrate::CrateDataType::Long:
          if (v.IsInt64()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, static_cast<int64_t>(v.GetInt64()));
#else
            p->values.push_back(Value(name, type, static_cast<int64_t>(v.GetInt64())));
#endif
            continue;
          }
          break;
        case CppCrate::CrateDataType::Double:
          if (v.IsDouble()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, v.GetDouble());
#else
            p->values.push_back(Value(name, type, v.GetDouble()));
#endif
            continue;
          }
          break;
        case CppCrate::CrateDataType::Float:
          if (v.IsDouble()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, v.GetFloat());
#else
            p->values.push_back(Value(name, type, v.GetFloat()));
#endif
            continue;
          }
          break;
        case CppCrate::CrateDataType::Timestamp:
          if (v.IsInt64()) {
#ifdef ENABLE_CPP11_SUPPORT
            p->values.emplace_back(name, type, static_cast<int64_t>(v.GetInt64()));
#else
            p->values.push_back(Value(name, type, static_cast<int64_t>(v.GetInt64())));
#endif
            continue;
          }
          break;
        default:
          break;
      }
      // Fall trough in order to add an invalid value.
    } else {
      if (v.IsNull()) {
#ifdef ENABLE_CPP11_SUPPORT
        p->values.emplace_back(type, true);
#else
        p->values.push_back(Value(type, true));
#endif
        continue;
      }

      if (v.IsString()) {
#ifdef ENABLE_CPP11_SUPPORT
        p->values.emplace_back(name, type, std::string(v.GetString(), v.GetStringLength()));
#else
        p->values.push_back(Value(name, type, std::string(v.GetString(), v.GetStringLength())));
#endif
        continue;
      }

      if (v.IsBool()) {
#ifdef ENABLE_CPP11_SUPPORT
        p->values.emplace_back(name, type, v.GetBool());
#else
        p->values.push_back(Value(name, type, v.GetBool()));
#endif
        continue;
      }

      if (v.IsObject() || v.IsArray()) {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        v.Accept(writer);
#ifdef ENABLE_CPP11_SUPPORT
        p->values.emplace_back(name, type, std::string(sb.GetString(), sb.GetSize()));
#else
        p->values.push_back(Value(name, type, std::string(sb.GetString(), sb.GetSize())));
#endif
        continue;
      }
      // Fall trough in order to add an invalid value.
    }

    // Fall back: Add "invalid" node
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    v.Accept(writer);
#ifdef ENABLE_CPP11_SUPPORT
    p->values.emplace_back(name, type, std::string(sb.GetString(), sb.GetSize()));
#else
    p->values.push_back(Value(name, type, std::string(sb.GetString(), sb.GetSize())));
#endif
  }
}

/*!
 * Returns a const iterator to the first value contained in this result.
 */
Record::const_iterator Record::begin() { return p->values.begin(); }

/*!
 * Returns a const iterator positioned after the last value contained in this result.
 */
Record::const_iterator Record::end() { return p->values.end(); }

#ifdef ENABLE_CPP11_SUPPORT
/*!
 * Returns a const iterator to the first value contained in this result.
 */
Record::const_iterator Record::cbegin() { return p->values.cbegin(); }

/*!
 * Returns a const iterator positioned after the last value contained in this result.
 */
Record::const_iterator Record::cend() { return p->values.cend(); }
#endif

/*!
 * Returns the size of the values contained in the record.
 */
int Record::size() const { return int(p->values.size()); }

/*!
 * Returns the value at position \a pos or an empty value if \a pos is invalid.
 */
Value Record::value(int pos) const {
  unsigned upos = unsigned(pos);
  if (upos < p->values.size()) return p->values[upos];
  return Value();
}

/*!
 * Returns the value with the name \a name or an empty value if \a name does not exists.
 *
 * \note This value is slower than the index based variant. So if you know the value's position
 *       you might want to use value() with the index.
 */
Value Record::value(const std::string &name) const {
  for (std::vector<Value>::const_iterator it = p->values.begin(), end = p->values.end(); it != end;
       ++it) {
    if (it->name() == name) return *it;
  }
  return Value();
}

}  // namespace CppCrate
