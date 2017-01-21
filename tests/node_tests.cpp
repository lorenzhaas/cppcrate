#include <gtest/gtest.h>

#include <cppcrate/node.h>

TEST(NodeTests, Contructors) {
  using CppCrate::Node;

  Node node;
  EXPECT_EQ(node.url(), "");
  EXPECT_EQ(node.httpUser(), "");
  EXPECT_EQ(node.httpPassword(), "");

  Node node2("a");
  EXPECT_EQ(node2.url(), "a");
  node2.setHttpAuthentication("b", "c");
  EXPECT_EQ(node2.httpUser(), "b");
  EXPECT_EQ(node2.httpPassword(), "c");
}

TEST(NodeTests, IsEmpty) {
  using CppCrate::Node;

  EXPECT_TRUE(Node().isEmpty());
  EXPECT_TRUE(Node("").isEmpty());

  Node node("");
  node.setHttpAuthentication("", "");
  EXPECT_TRUE(node.isEmpty());

  node.setUrl("a");
  EXPECT_FALSE(node.isEmpty());

  node.setUrl("");
  node.setHttpAuthentication("a", "");
  EXPECT_FALSE(node.isEmpty());

  node.setHttpAuthentication("", "a");
  EXPECT_FALSE(node.isEmpty());

  node.setHttpAuthentication("a", "a");
  EXPECT_FALSE(node.isEmpty());
}

TEST(NodeTests, HasAuthenticationInformation) {
  using CppCrate::Node;

  EXPECT_FALSE(Node().hasHttpAuthenticationInformation());
  EXPECT_FALSE(Node("").hasHttpAuthenticationInformation());

  Node node("");
  node.setHttpAuthentication("", "");
  EXPECT_FALSE(node.hasHttpAuthenticationInformation());

  node.setUrl("a");
  EXPECT_FALSE(node.hasHttpAuthenticationInformation());

  node.setUrl("");
  node.setHttpAuthentication("a", "");
  EXPECT_TRUE(node.hasHttpAuthenticationInformation());

  node.setHttpAuthentication("", "a");
  EXPECT_TRUE(node.hasHttpAuthenticationInformation());

  node.setHttpAuthentication("a", "a");
  EXPECT_TRUE(node.hasHttpAuthenticationInformation());
}

TEST(NodeTests, SetHttpAuthentication) {
  using CppCrate::Node;
  Node node;
  node.setHttpAuthentication("", "");
  EXPECT_EQ(node.httpUser(), "");
  EXPECT_EQ(node.httpPassword(), "");

  node.setHttpAuthentication("a", "");
  EXPECT_EQ(node.httpUser(), "a");
  EXPECT_EQ(node.httpPassword(), "");

  node.setHttpAuthentication("", "a");
  EXPECT_EQ(node.httpUser(), "");
  EXPECT_EQ(node.httpPassword(), "a");

  node.setHttpAuthentication("a", "b");
  EXPECT_EQ(node.httpUser(), "a");
  EXPECT_EQ(node.httpPassword(), "b");
}

TEST(NodeTests, Url) {
  using CppCrate::Node;

  Node node;
  node.setUrl("a");
  EXPECT_EQ(node.url(), "a");
  EXPECT_EQ(node.url(""), "a");
  EXPECT_EQ(node.url("/b"), "a/b");
  node.setUrl("");
  EXPECT_EQ(node.url(), "");
  EXPECT_EQ(node.url(""), "");
  EXPECT_EQ(node.url("/b"), "/b");
}

TEST(NodeTests, User) {
  using CppCrate::Node;

  Node node;
  node.setHttpUser("a");
  EXPECT_EQ(node.httpUser(), "a");
  node.setHttpUser("");
  EXPECT_EQ(node.httpUser(), "");
}

TEST(NodeTests, Password) {
  using CppCrate::Node;

  Node node;
  node.setHttpPassword("a");
  EXPECT_EQ(node.httpPassword(), "a");
  node.setHttpPassword("");
  EXPECT_EQ(node.httpPassword(), "");
}

TEST(NodeTests, Equal) {
  using CppCrate::Node;

  Node a;
  Node b;
  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);

  a.setUrl("a");
  EXPECT_NE(a, b);
  b.setUrl("a");
  EXPECT_EQ(a, b);

  a.setHttpUser("a");
  EXPECT_NE(a, b);
  b.setHttpUser("a");
  EXPECT_EQ(a, b);

  a.setHttpPassword("a");
  EXPECT_NE(a, b);
  b.setHttpPassword("a");
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
