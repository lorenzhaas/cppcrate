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
#include <cppcrate/value.h>

#include <memory>
#include <string>
#include <vector>

namespace CppCrate {

class CPPCRATE_EXPORT Record {
  CPPCRATE_PIMPL_DECLARE_ALL(Record)

 public:
  Record();
  Record(const std::string &data, const std::vector<std::string> &names,
         const std::vector<CrateDataType> &types);

  int size() const;

  Value value(int pos) const;
  Value value(const std::string &name) const;
};

}  // namespace CppCrate
