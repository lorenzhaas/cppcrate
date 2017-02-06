#include <gtest/gtest.h>

#include <cppcrate/client.h>
#include <cppcrate/result.h>

TEST(ClientTests, Connections) {
  using namespace CppCrate;

  Client c;

  EXPECT_TRUE(c.connect("localhost:4200"));
  EXPECT_TRUE(c);
  EXPECT_TRUE(c.isConnected());
  c.disconnect();

  EXPECT_TRUE(c.connect(Node("localhost:4200")));
  EXPECT_TRUE(c);
  EXPECT_TRUE(c.isConnected());
  c.disconnect();

  std::vector<Node> nodes;
  nodes.push_back(Node("localhost:4200"));
  nodes.push_back(Node("localhost:4200"));
  EXPECT_TRUE(c.connect(nodes));
  EXPECT_TRUE(c);
  EXPECT_TRUE(c.isConnected());
  c.disconnect();
}

TEST(ClientTests, DisconnectedClient) {
  using namespace CppCrate;

  Client c;
  ASSERT_FALSE(c);
  ASSERT_FALSE(c.isConnected());

  EXPECT_EQ(c.exec("a"), c.exec(Query("a")));
  EXPECT_EQ(c.execRaw("a"), c.execRaw(Query("a")));

  EXPECT_FALSE(c.refresh("a"));
  EXPECT_EQ(c.schemata(), std::vector<std::string>());
  EXPECT_EQ(c.clusterNodes(), std::vector<Node>());

  EXPECT_FALSE(c.createBlobStorage("a", "b"));
  EXPECT_FALSE(c.createBlobStorage("a", 1, 1, "b"));
  EXPECT_FALSE(c.removeBlobStorage("a"));

  std::istringstream is;
  EXPECT_FALSE(c.uploadBlob("a", is));
  EXPECT_FALSE(c.uploadBlob("a", "/tmp/cppcrateblob"));
  std::ostringstream os;
  EXPECT_FALSE(c.downloadBlob("a", "b", os));
  EXPECT_FALSE(c.downloadBlob("a", "b", "/tmp/cppcrateblob"));
  EXPECT_FALSE(c.existsBlob("a", "b"));
  EXPECT_FALSE(c.deleteBlob("a", "b"));
}

TEST(ClientTests, DefaultSchema) {
  using namespace CppCrate;

  Client c;

  EXPECT_EQ(c.defaultSchema(), "");

  c.setDefaultSchema("a");
  EXPECT_EQ(c.defaultSchema(), "a");

  c.setDefaultSchema("b");
  EXPECT_EQ(c.defaultSchema(), "b");

  c.clearDefaultSchema();
  EXPECT_EQ(c.defaultSchema(), "");

  c.setDefaultSchema("a");
  EXPECT_EQ(c.defaultSchema(), "a");

  c.setDefaultSchema("");
  EXPECT_EQ(c.defaultSchema(), "");
}

TEST(ClientTests, UnaccessibleNodesWithAuthentication) {
  using namespace CppCrate;

  Client c;

  std::vector<Node> nodes;
  nodes.push_back(Node("foo://bar"));
  Node n("foo://bar");
  n.setHttpAuthentication("user", "pass");
  nodes.push_back(n);

  std::vector<Client::ConnectionOptions> options;
  options.emplace_back(Client::ConnectToFirstNodeAlways);
  options.emplace_back(Client::ConnectToLastAccessedNode);
  options.emplace_back(Client::ConnectToRandomNode);
  for (std::size_t i = 0, total = options.size(); i < total; ++i) {
    c.connect(nodes, options[i]);
    ASSERT_TRUE(c);
    ASSERT_TRUE(c.isConnected());

    std::vector<Query> queries;
    queries.emplace_back("a");
    queries.emplace_back("a", "b");
    std::vector<std::string> bulk;
    bulk.emplace_back("a");
    bulk.emplace_back("a");
    queries.emplace_back("a", bulk);

    for (std::size_t j = 0, total = queries.size(); j < total; ++j) {
      EXPECT_FALSE(c.exec(queries[j]));
      EXPECT_FALSE(c.execRaw(queries[j]));

      c.setDefaultSchema(j % 2 ? "" : "b");

      EXPECT_FALSE(c.refresh("a"));
      EXPECT_EQ(c.schemata(), std::vector<std::string>());
      EXPECT_EQ(c.clusterNodes(), std::vector<Node>());

      EXPECT_FALSE(c.createBlobStorage("a", "b"));
      EXPECT_FALSE(c.createBlobStorage("a", 1, 1, "b"));
      EXPECT_FALSE(c.removeBlobStorage("a"));

      std::istringstream is;
      EXPECT_FALSE(c.uploadBlob("a", is));
      std::ostringstream os;
      EXPECT_FALSE(c.downloadBlob("a", "b", os));
      EXPECT_FALSE(c.existsBlob("a", "b"));
      EXPECT_FALSE(c.deleteBlob("a", "b"));
    }
  }
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
