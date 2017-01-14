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

#include <cppcrate/rawresult.h>

#include <rapidjson/document.h>

namespace CppCrate {

/*!
 * \class CppCrate::RawResult
 *
 * \brief Encapsulates Crate's HTTP endpoint reply.
 *
 * The class %RawResult encapsulates the plain reply of a Crate HTTP endpoint request.
 * It consists only of two information: the HTTP status code and the server's reply.
 *
 * The reply is typically the JSON data received from the endpoint. In case the cluster
 * could not be reached due to network problems this error is also wrapped in a Crate like error.
 * In addition, however, the object is extended by the key "component" with the value "curl":
 *
 * \code
 * {
 *   "error" :
 *   {
 *     "message"   : "Protocol \"httx\" not supported or disabled in libcurl",
 *     "code"      : 1,
 *     "component" : "curl"
 *   }
 * }
 * \endcode
 *
 * \sa Result
 */

/// \cond INTERNAL
class RawResult::Private {
 public:
  Private() : httpStatusCode(-1) {}

  bool operator==(const Private &other) const {
    return httpStatusCode == other.httpStatusCode && reply == other.reply;
  }

  int httpStatusCode;
  std::string reply;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(RawResult)

/*!
 * Constructs an empty result.
 */
RawResult::RawResult() : p(new Private) {}

/*!
 * Constructs a result with the HTTP status code \a code.
 */
RawResult::RawResult(int code) : p(new Private) { p->httpStatusCode = code; }

/*!
 * Constructs a result with the reply \a reply.
 */
RawResult::RawResult(const std::string &reply) : p(new Private) { p->reply = reply; }

/*!
 * Constructs a result with the reply \a reply and the HTTP status code \a code.
 */
RawResult::RawResult(const std::string &reply, int code) : p(new Private) {
  p->reply = reply;
  p->httpStatusCode = code;
}

/*!
 * Returns whether the result is empty.
 */
bool RawResult::isEmpty() const { return p->httpStatusCode == -1 && p->reply.empty(); }

/*!
 * Returns the server's reply.
 */
const std::string &RawResult::reply() const { return p->reply; }

/*!
 * Sets the reply to \a reply.
 */
void RawResult::setReply(const std::string &reply) { p->reply = reply; }

/*!
 * Returns the HTTP status code.
 */
int RawResult::httpStatusCode() const { return p->httpStatusCode; }

/*!
 * Sets the HTTP status code to \a code.
 */
void RawResult::setHttpStatusCode(int code) { p->httpStatusCode = code; }

/*!
 * Returns whether the reply contains an error.
 *
 * \see hasError()
 */
CppCrate::RawResult::operator bool() const { return !hasError(); }

/*!
 * Returns whether the reply contains an error. An empty reply or ill formed JSON are considered
 * erroneous. For bulk operations \c true is returned if at least one operation failed.
 *
 * \note This function parses the reply so you might cache the result if it's needed more than once.
 */
bool RawResult::hasError() const {
  if (p->reply.empty()) return true;
  rapidjson::Document doc;
  doc.Parse(p->reply);
  if (doc.HasParseError() || doc.HasMember("error")) {
    return true;
  } else if (doc.HasMember("results") && doc["results"].IsArray()) {
    rapidjson::Value &results = doc["results"];
    for (rapidjson::Value::ConstValueIterator it = results.Begin(), end = results.End(); it != end;
         ++it) {
      if (it->HasMember("rowcount")) {
        const rapidjson::Value &rowcount = (*it)["rowcount"];
        if (rowcount.IsInt() && rowcount.GetInt() == -2) return true;
      }
    }
  }
  return false;
}

}  // namespace CppCrate
