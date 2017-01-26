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

#include <cppcrate/cratedatatype.h>
#include "global_p.h"

namespace CppCrate {

/*!
 * \class CppCrate::CrateDataType
 *
 * \brief  Provides information about a Crate table's column type.
 *
 * The class %CrateDataType provides information about a Crate table's column type of a query.
 */

/*!
 * \enum CrateDataType::Type
 * Describes the data type. The underlying integer corresponds to the one Crate is using:
 * https://crate.io/docs/reference/protocols/http.html#column-types.
 *
 * \var CrateDataType::Type CrateDataType::Null
 * A null value.
 *
 * \var CrateDataType::Type CrateDataType::NotSupported
 * Unknown data type.
 *
 * \var CrateDataType::Type CrateDataType::Byte
 * A byte value (8-bit signed).
 *
 * \var CrateDataType::Type CrateDataType::Boolean
 * A boolean value.
 *
 * \var CrateDataType::Type CrateDataType::String
 * A string value.
 *
 * \var CrateDataType::Type CrateDataType::Ip
 * An IP value. (Returned as a string.)
 *
 * \var CrateDataType::Type CrateDataType::Double
 * A double value (64-bit IEEE 754 floating point).
 *
 * \var CrateDataType::Type CrateDataType::Float
 * A float value (32-bit IEEE 754 floating point).
 *
 * \var CrateDataType::Type CrateDataType::Short
 * A short value (16-bit signed).
 *
 * \var CrateDataType::Type CrateDataType::Integer
 * An integer value (32-bit signed).
 *
 * \var CrateDataType::Type CrateDataType::Long
 * A long value (64-bit signed).
 *
 * \var CrateDataType::Type CrateDataType::Timestamp
 * A timestamp value. (Returned as long.)
 *
 * \var CrateDataType::Type CrateDataType::Object
 * An JSON object.
 *
 * \var CrateDataType::Type CrateDataType::GeoPoint
 * A geographic point. (Returned as an array of two double values.)
 *
 * \var CrateDataType::Type CrateDataType::GeoShape
 * A geographic shape. (Returned as an object.)
 *
 * \var CrateDataType::Type CrateDataType::Array
 * An array value.
 *
 * \var CrateDataType::Type CrateDataType::Set
 * A set value.
 */

/// \cond INTERNAL
class CrateDataType::Private {
 public:
  bool operator==(const Private &other) const {
    return type == other.type && definition == other.definition;
  }

  Type type;
  std::string definition;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(CrateDataType)

/*!
 * Constructs a data type with the type \a type and its original definition \a definition.
 */
CrateDataType::CrateDataType(CrateDataType::Type type, const std::string &definition)
    : p(new Private) {
  p->type = type;
  p->definition = definition;
}

/*!
 * Sets the data type to \a type.
 */
void CrateDataType::setType(CrateDataType::Type type) { p->type = type; }

/*!
 * Sets the data type to \a type.
 * \see convert()
 */
void CrateDataType::setType(int type) { p->type = convert(type); }

/*!
 * Returns the data type.
 */
CrateDataType::Type CrateDataType::type() const { return p->type; }

/*!
 * Sets the definition to \a definition.
 */
void CrateDataType::setDefinition(const std::string &definition) { p->definition = definition; }

/*!
 * Returns the original definition extracted from the key \c col_types.
 */
const std::string &CrateDataType::definition() const { return p->definition; }

/*!
 * Returns the integer \a type converted into the type CrateDataType::Type. If \a type is not valid,
 * CrateDataType::NotSupported is returned.
 */
CrateDataType::Type CrateDataType::convert(int type) {
  if ((-1 < type && type < 15) || type == 100 || type == 101) return static_cast<Type>(type);
  return NotSupported;
}

}  // namespace CppCrate
