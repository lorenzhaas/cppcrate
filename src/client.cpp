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

#include <cppcrate/client.h>
#include "global_p.h"

#ifdef ENABLE_BLOB_SUPPORT
#include "crypto.h"
#endif

#include <curl/curl.h>

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <algorithm>

namespace CppCrate {

/*!
 * \namespace CppCrate
 * \brief Encapsulates all classes of the %CppCrate library.
 */

/*!
 * \class CppCrate::Client
 *
 * \brief Provides the main interface for accessing to Crate.
 *
 * The class %Client provides the main interface for accessing to Crate.
 *
 * To connect to a Crate cluster use connect(). Even tough connecting to a single node is enough
 * %CppCrate offers to define multiple nodes that can be used in case of node failure. The exact
 * behavior of using this internal fall back mechanism see Client::ConnectionOptions.
 *
 * Once connected use exec() or execRaw() to query Crate. While the latter version returns Crate's
 * raw reply the former lets you conveniently access specific reply information. See RawResult and
 * Result for more information. In order to avoid defining the used schema all over the place
 * setDefaultSchema() can be used to minimize the typing effort.
 *
 * \code
 * Client c;
 * c.connect("http://localhost:4200");
 * if (c) {
 *   c.setDefaultSchema("my_schema");
 *   c.exec("SELECT * FROM my_table"); // expands to: SELECT * FROM my_schema.my_table
 * }
 * \endcode
 *
 * Beside custom queries %CppCrate provides some predefined actions that are needed frequently when
 * working with Crate: clusterNodes() for example returns all nodes of the cluster the client is
 * connected to, or refresh() forces Crate to refresh its internal state.
 *
 * \section sec_client_blob Blob support
 *
 * %Client also provides an interface for accessing blob data. See createBlobStorage(),
 * uploadBlob(), existsBlob(), downloadBlob(), deleteBlob(), and removeBlobStorage() for more
 * information.
 */

/*!
 * \enum Client::ConnectionOptions
 *
 * Describes how the client handles node failures. %CppCrate's fallback mechanism is only activated
 * on network errors. Error reports issued by Crate are not considered.
 *
 * \var Client::ConnectionOptions Client::ConnectToFirstNodeAlways
 * The client will always use the first node for queries. If a query fails the client will
 * automatically retry to sent the query to the next node and so on.
 *
 * \var Client::ConnectionOptions Client::ConnectToLastAccessedNode
 * The client use the node for queries, that was used successfully the last time. If the query fails
 * the client will automatically retry to sent the query to the next node and so on.
 *
 * \var Client::ConnectionOptions Client::ConnectToRandomNode
 * The client use a random node for new queries. If the query fails the client will automatically
 * retry to sent the query to the next node and so on.
 */

/// \cond INTERNAL
namespace Internal {
std::size_t writeStringFunction(void* ptr, std::size_t size, std::size_t nmemb, std::string* data) {
  const std::size_t total = size * nmemb;
  data->append(static_cast<char*>(ptr), total);
  return total;
}

#ifdef ENABLE_BLOB_SUPPORT
std::size_t writeStreamFunction(void* ptr, std::size_t size, std::size_t nmemb,
                                std::ostream* data) {
  const std::size_t total = size * nmemb;
  data->write(static_cast<char*>(ptr), static_cast<std::streamsize>(total));
  return total;
}

std::size_t readFunction(void* ptr, std::size_t size, std::size_t nmemb, std::istream* stream) {
  const std::streamsize read =
      stream->readsome(static_cast<char*>(ptr), static_cast<std::streamsize>(size * nmemb));
  return static_cast<std::size_t>(read);
}
#endif
}

class Client::Private {
 public:
  Private() : curl(CPPCRATE_NULLPTR), options(ConnectToFirstNodeAlways), nodePos(0) {}
  ~Private() { disconnect(); }

  bool connect() {
    if (curl) {
      std::vector<Node> tmp = nodes;
      disconnect();
      nodes = tmp;
    }
    curl = curl_easy_init();
    if (!curl) return false;

    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CppCrate");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 25L);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlError);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "");

#ifdef CURL_AT_LEAST_VERSION
#if CURL_AT_LEAST_VERSION(7, 25, 0)
    curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
#endif
#endif

#ifdef ENABLE_BLOB_SUPPORT
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, Internal::readFunction);
#else
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Internal::writeStringFunction);
#endif

    return true;
  }

  void disconnect() {
    if (curl) {
      curl_easy_cleanup(curl);
      curl = CPPCRATE_NULLPTR;
    }
    curlError[0] = '\0';
    nodes.clear();
    nodePos = 0;
  }

  void resetCurl() {
    curlError[0] = '\0';
    curl_easy_setopt(curl, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, CPPCRATE_NULLPTR);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 0L);
    curl_easy_setopt(curl, CURLOPT_PUT, 0L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, CPPCRATE_NULLPTR);

#ifdef ENABLE_BLOB_SUPPORT
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CPPCRATE_NULLPTR);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 0L);
#endif
    curl_easy_setopt(curl, CURLOPT_READDATA, CPPCRATE_NULLPTR);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, CPPCRATE_NULLPTR);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, CPPCRATE_NULLPTR);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, 0L);
  }

  void setAuthentication(const Node& node) {
    if (node.hasHttpAuthenticationInformation()) {
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
      const std::string& user = nodes.front().httpUser();
      curl_easy_setopt(curl, CURLOPT_USERNAME, user.data());
      const std::string& password = nodes.front().httpPassword();
      curl_easy_setopt(curl, CURLOPT_PASSWORD, password.data());
    } else {
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, 0L);
      curl_easy_setopt(curl, CURLOPT_USERNAME, "");
      curl_easy_setopt(curl, CURLOPT_PASSWORD, "");
    }
  }

  Node getNode() { return nodes[nodePos]; }

  // Returns \c true if untried nodes left.
  bool setNodeError() {
    ++nodePos;
    if (nodePos < nodes.size()) {
      return true;
    } else {
      nodePos = 0;
      return false;
    }
  }

  void setNodeSuccess() {
    if (nodePos == 0) return;

    switch (options) {
      case ConnectToFirstNodeAlways:
        break;
      case ConnectToLastAccessedNode:
#ifdef ENABLE_CPP11_SUPPORT
        std::rotate(nodes.begin(), std::next(nodes.begin(), nodePos), nodes.end());
#else
        std::rotate(nodes.begin(), nodes.begin() + nodePos, nodes.end());
#endif
        break;
      case ConnectToRandomNode:
        std::random_shuffle(nodes.begin(), nodes.end());
        break;
    }

    nodePos = 0;
  }

  RawResult exec(const Query& query) {
    RawResult r;
    if (curl) {
      resetCurl();

      curl_slist* curlHeaders = CPPCRATE_NULLPTR;
      if (!defaultSchema.empty()) {
        const std::string header = "Default-Schema: " + defaultSchema;
        curlHeaders = curl_slist_append(curlHeaders, header.data());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
      }

      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("stmt");
      writer.String(query.statement());
      if (query.hasArguments()) {
        writer.Key("args");
        const std::string& args = query.arguments();
        writer.RawValue(args.data(), args.size(), rapidjson::kArrayType);
      } else if (query.hasBulkArguments()) {
        writer.Key("bulk_args");
        const std::vector<std::string>& bulkArgs = query.bulkArguments();
        std::string arg = "[";
        for (std::vector<std::string>::const_iterator it = bulkArgs.begin(), end = bulkArgs.end();
             it != end; ++it) {
          arg += *it + ",";
        }
        arg.replace(arg.size() - 1, 1, "]");
        writer.RawValue(arg.data(), arg.size(), rapidjson::kArrayType);
      }
      writer.EndObject();
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sb.GetSize());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, sb.GetString());

      std::string reply;
#ifdef ENABLE_BLOB_SUPPORT
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Internal::writeStringFunction);
#endif
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &reply);

      const Node& node = getNode();
      setAuthentication(node);
      const std::string& url = node.url("/_sql?types");
      curl_easy_setopt(curl, CURLOPT_URL, url.data());

      const CURLcode code = curl_easy_perform(curl);
      curl_slist_free_all(curlHeaders);
      long responseCode;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
      r.setHttpStatusCode(static_cast<int>(responseCode));

      if (code == CURLE_OK) {
        r.setReply(reply);
        setNodeSuccess();
      } else {
        if (setNodeError()) {
          return exec(query);
        }
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        writer.StartObject();
        writer.Key("error");
        writer.StartObject();
        writer.Key("message");
        writer.String(curlError);
        writer.Key("code");
        writer.Int(code);
        writer.Key("component");
        writer.String("curl");
        writer.EndObject();
        writer.EndObject();
        r.setReply(std::string(sb.GetString(), sb.GetSize()));
      }
    } else {
      rapidjson::StringBuffer sb;
      rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
      writer.StartObject();
      writer.Key("error");
      writer.StartObject();
      writer.Key("message");
      writer.String("");
      writer.Key("CppCrate::Client is not connected.");
      writer.Int(0);
      writer.Key("component");
      writer.String("CppCrate");
      writer.EndObject();
      writer.EndObject();
      r.setReply(std::string(sb.GetString(), sb.GetSize()));
    }
    return r;
  }

#ifdef ENABLE_BLOB_SUPPORT
  BlobResult uploadBlob(const std::string& tableName, const std::string& key, std::istream& data) {
    BlobResult r;
    r.setKey(key);

    if (curl) {
      resetCurl();
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
      curl_easy_setopt(curl, CURLOPT_PUT, 1L);
      curl_easy_setopt(curl, CURLOPT_READDATA, &data);
      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                       static_cast<curl_off_t>(Crypto::fileSize(data)));

      const Node& node = getNode();
      setAuthentication(node);
      const std::string& url = node.url("/_blobs/" + tableName + "/" + key);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());

      const CURLcode code = curl_easy_perform(curl);
      long responseCode;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

      if (code == CURLE_OK) {
        if (responseCode != 201) {
          r.setErrorString("Blob with the key '" + key + "' already exists.",
                           BlobResult::CrateErrorType);
        }
        setNodeSuccess();
      } else {
        if (setNodeError()) {
          return uploadBlob(tableName, key, data);
        }
        r.setErrorString(curlError, BlobResult::HttpErrorType);
      }
    } else {
      r.setErrorString("Client is not connected.", BlobResult::OtherErrorType);
    }

    return r;
  }

  BlobResult existsBlob(const std::string& tableName, const std::string& key) {
    BlobResult r;
    r.setKey(key);

    if (curl) {
      resetCurl();
      curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");

      const Node& node = getNode();
      setAuthentication(node);
      const std::string& url = node.url("/_blobs/" + tableName + "/" + key);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());

      const CURLcode code = curl_easy_perform(curl);
      long responseCode;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

      if (code == CURLE_OK) {
        if (responseCode != 200) {
          r.setErrorString("Blob with the key '" + key + "' does not exist.",
                           BlobResult::CrateErrorType);
        }
        setNodeSuccess();
      } else {
        if (setNodeError()) {
          return existsBlob(tableName, key);
        }
        r.setErrorString(curlError, BlobResult::HttpErrorType);
      }
    } else {
      r.setErrorString("Client is not connected.", BlobResult::OtherErrorType);
    }

    return r;
  }

  BlobResult deleteBlob(const std::string& tableName, const std::string& key) {
    BlobResult r;
    r.setKey(key);

    if (curl) {
      resetCurl();
      curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
      curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

      const Node& node = getNode();
      setAuthentication(node);
      const std::string& url = node.url("/_blobs/" + tableName + "/" + key);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());

      const CURLcode code = curl_easy_perform(curl);
      long responseCode;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

      if (code == CURLE_OK) {
        if (responseCode != 204) {
          r.setErrorString("Blob with the key '" + key + "' does not exist.",
                           BlobResult::CrateErrorType);
        }
        setNodeSuccess();
      } else {
        if (setNodeError()) {
          return deleteBlob(tableName, key);
        }
        r.setErrorString(curlError, BlobResult::HttpErrorType);
      }
    } else {
      r.setErrorString("Client is not connected.", BlobResult::OtherErrorType);
    }

    return r;
  }

  BlobResult downloadBlob(const std::string& tableName, const std::string& key,
                          std::ostream& data) {
    BlobResult r;
    r.setKey(key);

    if (curl) {
      resetCurl();
      curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);

      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Internal::writeStreamFunction);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

      const Node& node = getNode();
      setAuthentication(node);
      const std::string& url = node.url("/_blobs/" + tableName + "/" + key);
      curl_easy_setopt(curl, CURLOPT_URL, url.data());

      const CURLcode code = curl_easy_perform(curl);
      long responseCode;
      curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);

      if (code == CURLE_OK) {
        if (responseCode == 404) {
          r.setErrorString("Blob with the key '" + key + "' was not found.",
                           BlobResult::CrateErrorType);
        }
        setNodeSuccess();
      } else {
        if (setNodeError()) {
          return downloadBlob(tableName, key, data);
        }
        r.setErrorString(curlError, BlobResult::HttpErrorType);
      }
    } else {
      r.setErrorString("Client is not connected.", BlobResult::OtherErrorType);
    }

    return r;
  }
#endif

  std::vector<Node> nodes;
  CURL* curl;
  char curlError[CURL_ERROR_SIZE];
  std::string defaultSchema;
  ConnectionOptions options;
  std::size_t nodePos;
};
/// \endcond

CPPCRATE_PIMPL_IMPLEMENT_PRIVATE(Client)

/*!
 * Constructs a not connected client.
 */
Client::Client() : p(new Private) {}

/*!
 * Connects the client to the Crate cluster node identified by the URL \a url. This is equivalent to
 * \code
 * connect(Node("<url>"));
 * \endcode
 */
bool Client::connect(const std::string& url) {
  std::vector<Node> nodes;
#ifdef ENABLE_CPP11_SUPPORT
  nodes.emplace_back(url);
#else
  nodes.push_back(Node(url));
#endif
  return connect(nodes);
}

/*!
 * Connects the client to the Crate cluster node \a node.
 */
bool Client::connect(const Node& node) {
  std::vector<Node> nodes;
#ifdef ENABLE_CPP11_SUPPORT
  nodes.emplace_back(node);
#else
  nodes.push_back(node);
#endif
  return connect(nodes);
}

/*!
 * Connects the client to the Crate cluster using the provided Crate cluster nodes \a nodes. Which
 * node is used is defined by \a options.
 *
 * \see Client::ConnectionOptions
 */
bool Client::connect(const std::vector<Node>& nodes, ConnectionOptions options) {
  p->nodes = nodes;
  p->options = options;
  return p->connect();
}

/*!
 * Disconnects the client from the Crate cluster.
 */
void Client::disconnect() { p->disconnect(); }

/*!
 * Returns whether the client is connected.
 */
bool Client::isConnected() const { return p->curl != CPPCRATE_NULLPTR; }

/*!
 * Returns whether the client is connected.
 */
CppCrate::Client::operator bool() const { return isConnected(); }

/*!
 * Sets the default schema to \a schema. This allows SQL statements like
 * \code
 * client.setDefaultSchema("my_schema");
 * client.exec("SELECT * FROM my_table");
 * \endcode
 * instead of specifying the schema explicitly
 * \code
 * client.exec("SELECT * FROM my_schema.my_table");
 * \endcode
 */
void Client::setDefaultSchema(const std::string& schema) { p->defaultSchema = schema; }

/*!
 * Resets the default schema.
 */
void Client::clearDefaultSchema() { p->defaultSchema.clear(); }

/*!
 * Returns the default schema.
 */
const std::string& Client::defaultSchema() const { return p->defaultSchema; }

/*!
 * Executes the SQL statement \a sql and returns the result.
 */
Result Client::exec(const std::string& sql) { return Result(p->exec(Query(sql))); }

/*!
 * Executes the query \a query and returns the result.
 */
Result Client::exec(const Query& query) { return Result(p->exec(query)); }

/*!
 * Executes the SQL statement \a sql and returns the raw result.
 */
RawResult Client::execRaw(const std::string& sql) { return p->exec(Query(sql)); }

/*!
 * Executes the query \a query and returns the raw result.
 */
RawResult Client::execRaw(const Query& query) { return p->exec(query); }

/*!
 * Refreshes the table \a table and returns if the refresh was successful.
 *
 * \note See https://crate.io/docs/reference/sql/refresh.html#refresh-data for further information
 *       about Crate's eventually consistency.
 */
bool Client::refresh(const std::string& table) {
  return !execRaw("REFRESH TABLE " + table).hasError();
}

/*!
 * Returns all existing schemata.
 */
std::vector<std::string> Client::schemata() {
  std::vector<std::string> schemata;
  const Result result =
      exec("SELECT schema_name FROM information_schema.schemata ORDER BY schema_name");
  for (int i = 0; i < result.recordSize(); ++i) {
#ifdef ENABLE_CPP11_SUPPORT
    schemata.emplace_back(result.record(i).value(0).asString());
#else
    schemata.push_back(result.record(i).value(0).asString());
#endif
  }
  return schemata;
}

/*!
 * Returns all Crate cluster nodes the client is currently connected to.
 */
std::vector<Node> Client::clusterNodes() {
  std::vector<Node> node;
  const Result result = exec("select rest_url from sys.nodes");
  for (int i = 0; i < result.recordSize(); ++i) {
#ifdef ENABLE_CPP11_SUPPORT
    node.emplace_back(result.record(i).value(0).asString());
#else
    node.push_back(Node(result.record(i).value(0).asString()));
#endif
  }
  return node;
}

#ifdef ENABLE_BLOB_SUPPORT

/*!
 * Creates a new blob table named \a tableName. Additionally the number of shards and replications
 * can be defined using \a shards and \a replicas. If \a shards or \a replicas is lesser than 0 it
 * is ignored. If \a path is set, it is used for the new table's \c path.
 *
 * The returned RawResult can be used to determine if the action was successful.
 *
 * \code
 * CppCrate::RawResult r = client.createBlobStorage("images");
 * if (r) {
 *   // success
 * }
 * // or simply
 * if (client.createBlobStorage("images")) {
 *   // success
 * }
 * \endcode
 *
 * \pre \a path must not contain single quotes (') - except they are properly escaped.
 */
RawResult Client::createBlobStorage(const std::string& tableName, int shards, int replicas,
                                    const std::string& path) {
  std::string sql = "CREATE BLOB TABLE " + tableName;
  if (shards > -1) {
    sql += " CLUSTERED INTO " + CPPCRATE_TO_STRING(shards) + " SHARDS";
  }

  if (replicas > -1) {
    sql += " WITH (number_of_replicas=" + CPPCRATE_TO_STRING(shards);
    if (!path.empty()) {
      sql += ",blobs_path='" + path + "'";
    }
    sql += ")";
  } else if (!path.empty()) {
    sql += " WITH (blobs_path='" + path + "')";
  }

  return execRaw(Query(sql));
}

/*!
 * Creates a new blob table named \a tableName stored at \c path.
 *
 * This is a convenient function for
 *
 * \code
 * createBlobStorage(tableName, -1, -1, path);
 * \endcode
 *
 * \pre \a path must not contain single quotes (') - except they are properly escaped.
 */
RawResult Client::createBlobStorage(const std::string& tableName, const std::string& path) {
  return createBlobStorage(tableName, -1, -1, path);
}

/*!
 * Drops the blob table named \a tableName and returns the corresponding raw result that can be used
 * to determine if the action was successful.
 *
 * \code
 * CppCrate::RawResult r = client.removeBlobStorage("images");
 * if (r) {
 *   // success
 * }
 * // or simply
 * if (client.removeBlobStorage("images")) {
 *   // success
 * }
 * \endcode
 *
 */
RawResult Client::removeBlobStorage(const std::string& tableName) {
  return execRaw(Query("DROP BLOB TABLE " + tableName));
}

/*!
 * Uploads \a data to the table \a tableName. If the operation was successful the result can be used
 * to receive the key of the inserted blob.
 *
 * \code
 * std::ifstream stream;
 * stream.open("/path/to/blob", std::ifstream::in | std::ifstream::binary);
 * if (stream) {
 *   CppCrate::BlobResult result = client.uploadBlob("blobtable", stream);
 *   if (result) {
 *     std::cout << "Key is " << result.key() << "\n";
 *   }
 * }
 * \endcode
 *
 * \pre \a data must be readable and already opened in binary mode.
 */
BlobResult Client::uploadBlob(const std::string& tableName, std::istream& data) {
  std::string key = Crypto::sha1(data);
  if (key.empty()) {
    BlobResult r;
    r.setErrorString("Could not compute SHA1 key.", BlobResult::OtherErrorType);
    return r;
  }
  return p->uploadBlob(tableName, key, data);
}

/*!
 * Returns whether a blob identified by \a key exists in the table \a tableName.
 *
 * \note Be careful interpreting BlobResult::hasError() of the returned result. It does not
 *       automatically mean that the blob does not exists. An error have might occurred because of
 *       network issues. Remember to call BlobResult::isCrateError()!
 */
BlobResult Client::existsBlob(const std::string& tableName, const std::string& key) {
  return p->existsBlob(tableName, key);
}

/*!
 * Downloads the blob identified by \a key of the table \a tableName and stores it to \a data.
 *
 * \code
 * std::ofstream stream;
 * stream.open("/path/to/store/the/blob", std::ifstream::out | std::ifstream::binary);
 * if (stream) {
 *   CppCrate::BlobResult result = client.downloadBlob(
 *     "blobtable", "93390aa9ed64e1e96149ceb0262f34aa2aedcffc", stream);
 *   if (result) {
 *     std::cout << "Download succeeded!\n";
 *   }
 * }
 * \endcode
 *
 * \pre \a data must be writable and already opened in binary mode.
 */
BlobResult Client::downloadBlob(const std::string& tableName, const std::string& key,
                                std::ostream& data) {
  return p->downloadBlob(tableName, key, data);
}

/*!
 * Deletes the blob identified by \a key of the table \a tableName and returns the action's result.
 */
BlobResult Client::deleteBlob(const std::string& tableName, const std::string& key) {
  return p->deleteBlob(tableName, key);
}

#endif

}  // CppCrate
