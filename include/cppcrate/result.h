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
#include <cppcrate/rawresult.h>
#include <cppcrate/record.h>

#include <memory>
#include <string>
#include <vector>

namespace CppCrate {

class CPPCRATE_EXPORT Result {
  CPPCRATE_PIMPL_DECLARE_ALL(Result)

 public:
  explicit Result(const RawResult& raw);

#ifdef ENABLE_CPP11_SUPPORT
  explicit
#endif
  operator bool() const;
  bool hasError() const;
  const std::string& errorString() const;

  const RawResult& rawResult() const;

  double duration() const;
  int rowCount() const;
  const std::vector<std::string>& cols() const;
  const std::vector<CrateDataType>& colTypes() const;
  const std::vector<std::string>& rows() const;

  int recordSize() const;
  Record record(int pos) const;
};

}  // namespace CppCrate
