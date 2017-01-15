#include <gtest/gtest.h>

#include <cppcrate/node.h>

TEST(NodeTests, Contructors) {
  using CppCrate::Node;

  Node node;
  EXPECT_EQ(node.url(), "http://localhost:4200");
  EXPECT_EQ(node.user(), "");
  EXPECT_EQ(node.password(), "");

  Node node2("a", "b", "c");
  EXPECT_EQ(node2.url(), "a");
  EXPECT_EQ(node2.user(), "b");
  EXPECT_EQ(node2.password(), "c");
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
  node.setUser("a");
  EXPECT_EQ(node.user(), "a");
  node.setUser("");
  EXPECT_EQ(node.user(), "");
}

TEST(NodeTests, Password) {
  using CppCrate::Node;

  Node node;
  node.setPassword("a");
  EXPECT_EQ(node.password(), "a");
  node.setPassword("");
  EXPECT_EQ(node.password(), "");
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

  a.setUser("a");
  EXPECT_NE(a, b);
  b.setUser("a");
  EXPECT_EQ(a, b);

  a.setPassword("a");
  EXPECT_NE(a, b);
  b.setPassword("a");
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
