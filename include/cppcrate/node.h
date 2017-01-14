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

class CPPCRATE_EXPORT Node {
  CPPCRATE_PIMPL_DECLARE_ALL(Node)

 public:
  explicit Node(const std::string &url = "http://localhost:4200");
  Node(const std::string &url, const std::string &user, const std::string &password);

  const std::string &url() const;
  std::string url(const std::string &path) const;
  void setUrl(const std::string &url);

  const std::string &user() const;
  void setUser(const std::string &user);

  const std::string &password() const;
  void setPassword(const std::string &password);
};

}  // namespace CppCrate
