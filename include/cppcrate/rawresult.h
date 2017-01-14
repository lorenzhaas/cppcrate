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

class CPPCRATE_EXPORT RawResult {
  CPPCRATE_PIMPL_DECLARE_ALL(RawResult)

 public:
  RawResult();
  explicit RawResult(int code);
  explicit RawResult(const std::string& reply);
  RawResult(const std::string& reply, int code);

#ifdef ENABLE_CPP11_SUPPORT
  explicit
#endif
  operator bool() const;
  bool hasError() const;
  bool isEmpty() const;

  int httpStatusCode() const;
  void setHttpStatusCode(int code);

  const std::string& reply() const;
  void setReply(const std::string& reply);
};

}  // namespace CppCrate
