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

#include <cppcrate/global.h>

#include <memory>
#include <string>

namespace CppCrate {

class CPPCRATE_EXPORT CrateDataType {
  CPPCRATE_PIMPL_DECLARE_ALL(CrateDataType)

 public:
  enum Type {
    Null = 0,
    NotSupported = 1,
    Byte = 2,
    Boolean = 3,
    String = 4,
    Ip = 5,
    Double = 6,
    Float = 7,
    Short = 8,
    Integer = 9,
    Long = 10,
    Timestamp = 11,
    Object = 12,
    GeoPoint = 13,
    GeoShape = 14,
    Array = 100,
    Set = 101
  };

  explicit CrateDataType(Type type = NotSupported, const std::string& definition = std::string());

  void setType(int type);
  void setType(Type type);
  Type type() const;

  void setDefinition(const std::string& definition);
  const std::string& definition() const;

  static Type convert(int type);
};

}  // namespace CppCrate
