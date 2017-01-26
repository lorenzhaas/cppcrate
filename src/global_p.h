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

#ifdef ENABLE_CPP11_SUPPORT
#define CPPCRATE_PIMPL_IMPLEMENT_ALL(Class) \
  CPPCRATE_PIMPL_IMPLEMENT_PRIVATE(Class)   \
  CPPCRATE_PIMPL_IMPLEMENT_COPY(Class)      \
  CPPCRATE_PIMPL_IMPLEMENT_MOVE(Class)      \
  CPPCRATE_PIMPL_IMPLEMENT_COMPARISON(Class)
#else
#include <sstream>
namespace CppCrate {
template <class T>
inline std::string to_string(const T &t) {
  std::stringstream ss;
  ss << t;
  return ss.str();
}
}
#define CPPCRATE_PIMPL_IMPLEMENT_ALL(Class) \
  CPPCRATE_PIMPL_IMPLEMENT_PRIVATE(Class)   \
  CPPCRATE_PIMPL_IMPLEMENT_COPY(Class)      \
  CPPCRATE_PIMPL_IMPLEMENT_COMPARISON(Class)
#endif

#define CPPCRATE_PIMPL_IMPLEMENT_PRIVATE(Class) \
  Class::~Class() { delete p; }

#define CPPCRATE_PIMPL_IMPLEMENT_COPY(Class)                     \
  void swap(Class &lhs, Class &rhs) {                            \
    using std::swap;                                             \
    swap(lhs.p, rhs.p);                                          \
  }                                                              \
  Class::Class(const Class &other) : p(new Private(*other.p)) {} \
  Class &Class::operator=(Class other) {                         \
    swap(*this, other);                                          \
    return *this;                                                \
  }

#define CPPCRATE_PIMPL_IMPLEMENT_MOVE(Class) \
  Class::Class(Class &&other) : p(other.p) { other.p = CPPCRATE_NULLPTR; }

#define CPPCRATE_PIMPL_IMPLEMENT_COMPARISON(Class)                            \
  bool Class::operator==(const Class &other) const { return *p == *other.p; } \
  bool Class::operator!=(const Class &other) const { return !(*this == other); }
