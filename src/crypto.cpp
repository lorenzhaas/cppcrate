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

#include "crypto.h"

#include <sha1/sha1.hpp>

#include <iomanip>
#include <sstream>

namespace CppCrate {

/// \cond INTERNAL
int64_t Crypto::fileSize(std::istream& data) {
  data.seekg(0, std::ios::beg);
  std::istream::pos_type size = data.tellg();
  data.seekg(0, std::ios::end);
  size = data.tellg() - size;
  data.seekg(0, std::ios::beg);
  return size;
}

std::string Crypto::sha1(std::istream& data) {
  SHA1 gen;
  gen.update(data);
  return gen.final();
}
/// \endcond

}  // namespace CppCrate
