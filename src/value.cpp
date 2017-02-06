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

#include <cppcrate/value.h>
#include "global_p.h"

#include <sstream>

namespace CppCrate {

/*!
 * \class CppCrate::Value
 *
 * \brief Holds the actual value inside a record and provides convenience functions to get the value
 *        as different types.
 *
 * The class %Value holds the actual value inside a Record and provides convenience functions to get
 * the value as different types.
 *
 * The original column name can be accessed via name() and crateType() returns the value's data type
 * inside Crate. In contrast type() returns the value's real data storage type inside %Value.
 *
 * The relation between Crate's data type and %Values's own data storage type is as follows:
 *
 * | %CrateDataType::Type | %Value::Type | Description          |
 * | -------------------- | ------------ | -------------------- |
 * | Null                 | NullType     |                      |
 * | NotSupported         | InvalidType  |                      |
 * | Byte                 | Int16Type    |                      |
 * | Boolean              | BoolType     |                      |
 * | String               | StringType   |                      |
 * | Ip                   | StringType   |                      |
 * | Double               | DoubleType   |                      |
 * | Float                | FloatType    |                      |
 * | Short                | Int16Type    |                      |
 * | Integer              | Int32Type    |                      |
 * | Long                 | Int64Type    |                      |
 * | Timestamp            | Int64Type    |                      |
 * | Object               | StringType   | as received by Crate |
 * | GeoPoint             | StringType   | as received by Crate |
 * | GeoShape             | StringType   | as received by Crate |
 * | Array                | StringType   | as received by Crate |
 * | Set                  | StringType   | as received by Crate |
 *
 * \section sec_class_value_conversion Conversion
 *
 * Even if each value is bound to it's own type -- type() -- the value can be retrieved converted
 * into different types using asBool(), asInt(), asLong(), asLongLong(), asFloat(), asDouble() and
 * asString(). If a conversion is not possible a default value will be returned. Note, however, that
 * it is advised to get the value according it's own data type.
 */

/*!
 * \enum Value::Type
 * Describes the %Value's own data storage type.
 *
 * \var Value::Type Value::InvalidType
 * An invalid value.
 *
 * \var Value::Type Value::NullType
 * An empty value.
 *
 * \var Value::Type Value::StringType
 * The value was constructed using a \c std::string.
 *
 * \var Value::Type Value::Int16Type
 * The value was constructed using a \c int.
 *
 * \var Value::Type Value::Int32Type
 * The value was constructed using a \c long.
 *
 * \var Value::Type Value::Int64Type
 * The value was constructed using a \c long \c long.
 *
 * \var Value::Type Value::BoolType
 * The value was constructed using a \c bool.
 *
 * \var Value::Type Value::DoubleType
 * The value was constructed using a \c double.
 *
 * \var Value::Type Value::FloatType
 * The value was constructed using a \c float.
 */

/// \cond INTERNAL
class Value::Private {
 public:
  Private() : type(CrateDataType::NotSupported), dataType(InvalidType) {}

  ~Private() {
    if (dataType == StringType) delete data.sp;
  }

  Private(const Private &other) {
    type = other.type;
    dataType = other.dataType;
    if (dataType == StringType)
      data.sp = new std::string(*other.data.sp);
    else
      data = other.data;
    name = other.name;
  }

  Private &operator=(const Private &other) {
    if (this != &other) {
      if (dataType == StringType) delete data.sp;
      *this = Private(other);
    }
    return *this;
  }

#ifdef ENABLE_CPP11_SUPPORT
  Private(Private &&other)
      : type(std::move(other.type)),
        dataType(std::move(other.dataType)),
        name(std::move(other.name)) {
    if (dataType == StringType) {
      data.sp = std::move(other.data.sp);
      other.data.sp = CPPCRATE_NULLPTR;
    } else {
      data = std::move(other.data);
    }
  }

  Private &operator=(Private &&other) {
    if (this != &other) {
      if (dataType == StringType) delete data.sp;

      type = std::move(other.type);
      dataType = std::move(other.dataType);
      if (dataType == StringType) {
        data.sp = std::move(other.data.sp);
        other.data.sp = CPPCRATE_NULLPTR;
      } else {
        data = std::move(other.data);
      }
      name = std::move(other.name);
    }
    return *this;
  }
#endif

  bool operator==(const Private &other) const {
    if (type != other.type || dataType != other.dataType || name != other.name) {
      return false;
    }
    switch (dataType) {
      case InvalidType:  // [[fallthrough]]
      case NullType:
        break;
      case BoolType:
        return data.b == other.data.b;
      case Int16Type:
        return data.i == other.data.i;
      case Int32Type:
        return data.l == other.data.l;
      case Int64Type:
        return data.ll == other.data.ll;
      case FloatType:
        return data.f == other.data.f;
      case DoubleType:
        return data.d == other.data.d;
      case StringType:
        return *data.sp == *other.data.sp;
    }
    return true;
  }

  template <typename T>
  T asNumeric() const {
    switch (dataType) {
      case InvalidType:  // [[fallthrough]]
      case NullType:
        break;
      case StringType: {
        T number;
        if (!(std::istringstream(*data.sp) >> number)) number = static_cast<T>(0);
        return number;
      }
      case Int16Type:
        return static_cast<T>(data.i);
      case Int32Type:
        return static_cast<T>(data.l);
      case Int64Type:
        return static_cast<T>(data.ll);
      case FloatType:
        return static_cast<T>(data.f);
      case DoubleType:
        return static_cast<T>(data.d);
      case BoolType:
        return static_cast<T>(data.b);
    }

    return static_cast<T>(0);
  }

  CrateDataType type;
  Type dataType;
  union Data {
    bool b;
    int16_t i;
    int32_t l;
    int64_t ll;
    float f;
    double d;
    std::string *sp;
  } data;
  std::string name;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Value)

/*!
 * Constructs an invalid value.
 */
Value::Value() : p(new Private) {}

/*!
 * Constructs an invalid value with Crate's original data type \a type.
 */
Value::Value(const CrateDataType &type) : p(new Private) {
  p->type = type;
  p->dataType = InvalidType;
}

/*!
 * Constructs a null value with Crate's original data type \a type.
 */
Value::Value(const CrateDataType &type, bool) : p(new Private) {
  p->type = type;
  p->dataType = NullType;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c bool with it's own type Value::BoolType.
 */
Value::Value(const std::string &name, const CrateDataType &type, bool value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = BoolType;
  p->data.b = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c int with it's own type Value::Int16Type.
 */
Value::Value(const std::string &name, const CrateDataType &type, int16_t value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = Int16Type;
  p->data.i = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c long with it's own type Value::Int32Type.
 */
Value::Value(const std::string &name, const CrateDataType &type, int32_t value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = Int32Type;
  p->data.l = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c long \c long with it's own type Value::Int64Type.
 */
Value::Value(const std::string &name, const CrateDataType &type, int64_t value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = Int64Type;
  p->data.ll = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c float with it's own type Value::FloatType.
 */
Value::Value(const std::string &name, const CrateDataType &type, float value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = FloatType;
  p->data.f = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c double with it's own type Value::DoubleType.
 */
Value::Value(const std::string &name, const CrateDataType &type, double value) : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = DoubleType;
  p->data.d = value;
}

/*!
 * Constructs a value with the column name \a name and Crate's original data type \a type.
 * The \a value is internally stored as a \c std::string with it's own type Value::StringType.
 */
Value::Value(const std::string &name, const CrateDataType &type, const std::string &value)
    : p(new Private) {
  p->type = type;
  p->name = name;
  p->dataType = StringType;
  p->data.sp = new std::string(value);
}

/*!
 * Returns the value's original column name.
 */
const std::string &Value::name() const { return p->name; }

/*!
 * Returns the value's original Crate type.
 */
CrateDataType Value::crateType() const { return p->type; }

/*!
 * Returns whether the value is invalid.
 */
bool Value::isInvalid() const { return p->dataType == InvalidType; }

/*!
 * Returns whether the value is NULL.
 */
bool Value::isNull() const { return p->dataType == NullType; }

/*!
 * Returns the value's own storage type.
 */
Value::Type Value::type() const { return p->dataType; }

/*!
 * Returns the value as a \c std::string or an empty string if the value couldn't be converted.
 */
std::string Value::asString() const {
  switch (p->dataType) {
    case InvalidType:  // [[fallthrough]]
    case NullType:
      break;
    case StringType:
      return *p->data.sp;
    case Int16Type:
      return CPPCRATE_TO_STRING(p->data.i);
    case Int32Type:
      return CPPCRATE_TO_STRING(p->data.l);
    case Int64Type:
      return CPPCRATE_TO_STRING(p->data.ll);
    case FloatType:
      return CPPCRATE_TO_STRING(p->data.f);
    case DoubleType:
      return CPPCRATE_TO_STRING(p->data.d);
    case BoolType:
      return (p->data.b) ? std::string("true") : std::string("false");
  }
  return std::string();
}

/*!
 * Returns the value as a signed 16 bit integer or 0 if the value couldn't be converted.
 */
int16_t Value::asInt16() const { return p->asNumeric<int16_t>(); }

/*!
 * Returns the value as a signed 32 bit integer or 0 if the value couldn't be converted.
 */
int32_t Value::asInt32() const { return p->asNumeric<int32_t>(); }

/*!
 * Returns the value as a signed 64 bit integer or 0 if the value couldn't be converted.
 */
int64_t Value::asInt64() const { return p->asNumeric<int64_t>(); }

/*!
 * Returns the value as a \c float or 0 if the value couldn't be converted.
 */
float Value::asFloat() const { return p->asNumeric<float>(); }

/*!
 * Returns the value as a \c double or 0 if the value couldn't be converted.
 */
double Value::asDouble() const { return p->asNumeric<double>(); }

/*!
 * Returns the value as a \c bool or \c false if the value couldn't be converted.
 */
bool Value::asBool() const {
  return (p->dataType == StringType)
             ? !(*p->data.sp == "false" || *p->data.sp == "0" || p->data.sp->empty())
             : (p->asNumeric<int>() != 0);
}

}  // namespace CppCrate
