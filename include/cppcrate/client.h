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

#include <cppcrate/node.h>
#include <cppcrate/query.h>
#include <cppcrate/rawresult.h>
#include <cppcrate/result.h>

#ifdef ENABLE_BLOB_SUPPORT
#include <cppcrate/blobresult.h>

#include <iostream>
#endif

#include <memory>
#include <string>
#include <vector>

namespace CppCrate {

class CPPCRATE_EXPORT Client {
  CPPCRATE_PIMPL_DECLARE_PRIVATE(Client)

 public:
  enum ConnectionOptions {
    ConnectToFirstNodeAlways,
    ConnectToLastAccessedNode,
    ConnectToRandomNode
  };

  Client();

  bool connect(const std::string &url);
  bool connect(const Node &node);
  bool connect(const std::vector<Node> &nodes,
               ConnectionOptions options = ConnectToLastAccessedNode);
  void disconnect();
  bool isConnected() const;

#ifdef ENABLE_CPP11_SUPPORT
  explicit
#endif
  operator bool() const;

  void setDefaultSchema(const std::string &schema);
  void clearDefaultSchema();
  const std::string &defaultSchema() const;

  Result exec(const std::string &sql);
  Result exec(const Query &query);
  RawResult execRaw(const std::string &sql);
  RawResult execRaw(const Query &query);

  bool refresh(const std::string &table);
  std::vector<std::string> schemata();
  std::vector<Node> clusterNodes();

#ifdef ENABLE_BLOB_SUPPORT
  RawResult createBlobStorage(const std::string &tableName, int shards = -1, int replicas = -1,
                              const std::string &path = std::string());
  RawResult createBlobStorage(const std::string &tableName, const std::string &path);
  RawResult removeBlobStorage(const std::string &tableName);

  BlobResult uploadBlob(const std::string &tableName, std::istream &data);
  BlobResult existsBlob(const std::string &tableName, const std::string &key);
  BlobResult downloadBlob(const std::string &tableName, const std::string &key, std::ostream &data);
  BlobResult deleteBlob(const std::string &tableName, const std::string &key);
#endif
};

}  // CppCrate
