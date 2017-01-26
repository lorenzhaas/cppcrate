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
#include "global_p.h"

namespace CppCrate {

/*!
 * \class CppCrate::Node
 *
 * \brief Encapsulates the information needed to connect to a Crate cluster node.
 *
 * The class %Node encapsulates the information needed to connect to a Crate cluster node.
 * If a HTTP user and HTTP password is set CURL picks the best suitable authentication automatically
 * ([CURLAUTH_ANY](https://curl.haxx.se/libcurl/c/CURLOPT_HTTPAUTH.html#CURLAUTHANY)).
 *
 * The setup for a node that is HTTP password protected could -- for example -- look like:
 *
 * \code
 * Node node;
 * node.setUrl("http://localhost:4200");
 * node.setHttpUser("Hobbes");
 * node.setHttpPassword("magic_tiger");
 * \endcode
 */

/// \cond INTERNAL
class Node::Private {
 public:
  bool operator==(const Private &other) const {
    return url == other.url && httpUser == other.httpUser && httpPassword == other.httpPassword;
  }
  std::string url;
  std::string httpUser;
  std::string httpPassword;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_ALL(Node)

/*!
 * Constructs an empty node.
 */
Node::Node() : p(new Private) {}

/*!
 * Constructs a node that connects to the URL \a url.
 *
 * \todo As soon as Crate supports authentication extend this constructor by user and password.
 */
Node::Node(const std::string &url) : p(new Private) { p->url = url; }

/*!
 * Returns \c true if neither the URL, nor a HTTP user, nor a HTTP password is defined.
 */
bool Node::isEmpty() const {
  return p->url.empty() && p->httpUser.empty() && p->httpPassword.empty();
}

/*!
 * Returns \c true if at least HTTP user or HTTP password is defined.
 */
bool Node::hasHttpAuthenticationInformation() const {
  return !p->httpUser.empty() || !p->httpPassword.empty();
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
 * Sets the HTTP user used to connect to the node to \a user and the HTTP password used to connect
 * to the node to \a password.
 *
 * This is a short hand for setHttpUser() and setHttpPassword().
 */
void Node::setHttpAuthentication(const std::string &user, const std::string &password) {
  p->httpUser = user;
  p->httpPassword = password;
}

/*!
 * Returns the HTTP user used to connect to the node.
 */
const std::string &Node::httpUser() const { return p->httpUser; }

/*!
 * Sets the HTTP user used to connect to the node to \a user.
 */
void Node::setHttpUser(const std::string &user) { p->httpUser = user; }

/*!
 * Returns the HTTP password used to connect to the node.
 */
const std::string &Node::httpPassword() const { return p->httpPassword; }

/*!
 * Sets the HTTP password used to connect to the node to \a password.
 */
void Node::setHttpPassword(const std::string &password) { p->httpPassword = password; }

}  // namespace CppCrate
