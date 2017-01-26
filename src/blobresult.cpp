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

#include <cppcrate/blobresult.h>
#include "global_p.h"

namespace CppCrate {

/*!
 * \class CppCrate::BlobResult
 *
 * \brief Encapsulates Crate's HTTP endpoint reply of a blob operation.
 *
 * The class %BlobResult encapsulates Crate's HTTP endpoint reply of a blob operation.
 *
 * A blob operation can fail because of a network error or an error issued by Crate. To determine
 * if a blob operation was successful you can investigate the result the Client returns:
 *
 * \code
 * BlobResult result = client.anyBlobOperation();
 * if (result) {
 *   // The operation succeeded.
 * } else {
 *   if (result.isCrateError()) {
 *     // The operation was denied by Crate
 *   } else {
 *     // A network or client related error occurred.
 *   }
 * }
 *
 * \endcode
 */

/*!
 * \enum BlobResult::ErrorType
 * Describes the blob operation's error type.
 *
 * \var BlobResult::ErrorType BlobResult::HttpErrorType
 * An error due to connection problems.
 *
 * \var BlobResult::ErrorType BlobResult::CrateErrorType
 * An error issued by Crate.
 *
 * \var BlobResult::ErrorType BlobResult::OtherErrorType
 * An error of unknown source.
 */

/// \cond INTERNAL
class BlobResult::Private {
 public:
  Private() : errorType(OtherErrorType) {}

  bool operator==(const Private &other) const {
    return errorType == other.errorType && errorString == other.errorString && key == other.key;
  }

  ErrorType errorType;
  std::string errorString;
  std::string key;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(BlobResult)

/*!
 * Constructs an empty result.
 */
BlobResult::BlobResult() : p(new Private) {}

/*!
 * Constructs a result with the error string \a error and the type \a type.
 */
BlobResult::BlobResult(const std::string &error, BlobResult::ErrorType type) : p(new Private) {
  p->errorString = error;
  p->errorType = type;
}

/*!
 * Returns whether the result is valid.
 */
CppCrate::BlobResult::operator bool() const { return !hasError(); }

/*!
 * Returns whether the result has an error.
 */
bool BlobResult::hasError() const { return !p->errorString.empty(); }

/*!
 * Returns the error string.
 */
const std::string &BlobResult::errorString() const { return p->errorString; }

/*!
 * Sets the error string to \a error and it's type to \a type.
 */
void BlobResult::setErrorString(const std::string &error, ErrorType type) {
  p->errorString = error;
  p->errorType = type;
}

/*!
 * Returns whether the error is Crate related.
 * \see BlobResult::ErrorType
 */
bool BlobResult::isCrateError() const { return p->errorType == CrateErrorType; }

/*!
 * Returns the error type.
 * \see BlobResult::ErrorType
 */
BlobResult::ErrorType BlobResult::errorType() const { return p->errorType; }

/*!
 * Returns the key of the processed blob.
 */
const std::string &BlobResult::key() const { return p->key; }

/*!
 * Sets the key of the processed blob to \a key.
 */
void BlobResult::setKey(const std::string &key) { p->key = key; }

}  // namespace CppCrate
