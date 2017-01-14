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

#include <cppcrate/node.h>

namespace CppCrate {

/*!
 * \class CppCrate::Node
 *
 * \brief Encapsulates the information needed to connect to a Crate cluster node.
 *
 * The class %Node encapsulates the information needed to connect to a Crate cluster node.
 * If a user and password is set CURL picks the best suitable authentication automatically
 * ([CURLAUTH_ANY](https://curl.haxx.se/libcurl/c/CURLOPT_HTTPAUTH.html#CURLAUTHANY)).
 *
 * The setup for a node that is password protected could -- for example -- look like:
 *
 * \code
 * Node node;
 * node.setUrl("http://localhost:4200");
 * node.setUser("Hobbes");
 * node.setPassword("magic_tiger");
 * \endcode
 */

/// \cond INTERNAL
class Node::Private {
 public:
  bool operator==(const Private &other) const {
    return url == other.url && user == other.user && password == other.password;
  }
  std::string url;
  std::string user;
  std::string password;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Node)

/*!
 * Constructs a node that connects to the URL \a url.
 */
Node::Node(const std::string &url) : p(new Private) { p->url = url; }

/*!
 * Constructs a node that connects to the URL \a url with the credentials \a user and \a password.
 */
Node::Node(const std::string &url, const std::string &user, const std::string &password)
    : p(new Private) {
  p->url = url;
  p->user = user;
  p->password = password;
}

/*!
 * Returns the node's URL.
 */
const std::string &Node::url() const { return p->url; }

/*!
 * Returns the node's URL extended by \a path.
 *
 * \code
 * Node node("http://localhost:4200");
 * node.url("/_sql"); // http://localhost:4200/_sql
 * \endcode
 */
std::string Node::url(const std::string &path) const { return p->url + path; }

/*!
 * Sets the URL used to connect to the node to \a url.
 */
void Node::setUrl(const std::string &url) { p->url = url; }

/*!
 * Returns the user used to connect to the node.
 */
const std::string &Node::user() const { return p->user; }

/*!
 * Sets the user used to connect to the node to \a user.
 */
void Node::setUser(const std::string &user) { p->user = user; }

/*!
 * Returns the password used to connect to the node.
 */
const std::string &Node::password() const { return p->password; }

/*!
 * Sets the password used to connect to the node to \a password.
 */
void Node::setPassword(const std::string &password) { p->password = password; }

}  // namespace CppCrate
