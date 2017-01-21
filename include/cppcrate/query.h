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
#include <vector>

namespace CppCrate {

class CPPCRATE_EXPORT Query {
  CPPCRATE_PIMPL_DECLARE_ALL(Query)

 public:
  enum Type { SimpleType, ArgumentType, BulkArgumentType };

  explicit Query(const std::string &sql = "");
  Query(const std::string &sql, const std::string &args);
  Query(const std::string &sql, const std::vector<std::string> &bulkArgs);

  bool isEmpty() const;
  Type type() const;

  void setStatement(const std::string &sql);
  const std::string &statement() const;
  bool hasStatement() const;

  void setArguments(const std::string &args);
  const std::string &arguments() const;
  bool hasArguments() const;

  void setBulkArguments(const std::vector<std::string> &bulkArgs);
  const std::vector<std::string> &bulkArguments() const;
  bool hasBulkArguments() const;
};

}  // namespace CppCrate
