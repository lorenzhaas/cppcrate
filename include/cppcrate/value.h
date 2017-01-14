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

#pragma once

#include <cppcrate/cratedatatype.h>
#include <cppcrate/global.h>

#include <memory>
#include <string>

namespace CppCrate {

class CPPCRATE_EXPORT Value {
  CPPCRATE_PIMPL_DECLARE_ALL(Value)

 public:
  enum Type {
    InvalidType,
    NullType,
    BoolType,
    Int16Type,
    Int32Type,
    Int64Type,
    FloatType,
    DoubleType,
    StringType
  };

  Value();
  explicit Value(const CrateDataType &type);
  Value(const CrateDataType &type, bool);
  Value(const std::string &name, const CrateDataType &type, bool value);
  Value(const std::string &name, const CrateDataType &type, int16_t value);
  Value(const std::string &name, const CrateDataType &type, int32_t value);
  Value(const std::string &name, const CrateDataType &type, int64_t value);
  Value(const std::string &name, const CrateDataType &type, float value);
  Value(const std::string &name, const CrateDataType &type, double value);
  Value(const std::string &name, const CrateDataType &type, const std::string &value);

  const std::string &name() const;
  CrateDataType crateType() const;

  bool isInvalid() const;
  bool isNull() const;

  Type type() const;
  bool asBool() const;
  int16_t asInt16() const;
  int32_t asInt32() const;
  int64_t asInt64() const;
  float asFloat() const;
  double asDouble() const;
  std::string asString() const;
};

}  // namespace CppCrate
