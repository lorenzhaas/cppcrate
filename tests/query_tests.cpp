#include <gtest/gtest.h>

#include <cppcrate/query.h>

TEST(QueryTests, Contructors) {
  using CppCrate::Query;

  Query q("a");
  EXPECT_EQ(q.statement(), "a");
  EXPECT_EQ(q.arguments(), "");
  EXPECT_EQ(q.bulkArguments(), std::vector<std::string>());
  EXPECT_FALSE(q.hasArguments());
  EXPECT_FALSE(q.hasBulkArguments());

  Query q2("a", "b");
  EXPECT_EQ(q2.statement(), "a");
  EXPECT_EQ(q2.arguments(), "b");
  EXPECT_EQ(q2.bulkArguments(), std::vector<std::string>());
  EXPECT_TRUE(q2.hasArguments());
  EXPECT_FALSE(q2.hasBulkArguments());

  std::vector<std::string> ba;
  ba.emplace_back("a");
  ba.emplace_back("b");

  Query q3("a", ba);
  EXPECT_EQ(q3.statement(), "a");
  EXPECT_EQ(q3.arguments(), "");
  EXPECT_EQ(q3.bulkArguments(), ba);
  EXPECT_FALSE(q3.hasArguments());
  EXPECT_TRUE(q3.hasBulkArguments());
}

TEST(QueryTests, Type) {
  using CppCrate::Query;

  Query q("a");
  EXPECT_EQ(q.type(), Query::SimpleType);

  Query q2("a", "b");
  EXPECT_EQ(q2.type(), Query::ArgumentType);

  std::vector<std::string> ba;
  ba.emplace_back("a");
  ba.emplace_back("b");
  Query q3("a", ba);
  EXPECT_EQ(q3.type(), Query::BulkArgumentType);
}

TEST(QueryTests, Statement) {
  using CppCrate::Query;

  Query q("");
  EXPECT_EQ(q.statement(), "");
  q.setStatement("a");
  EXPECT_EQ(q.statement(), "a");
  q.setStatement("");
  EXPECT_EQ(q.statement(), "");
}

TEST(QueryTests, Arguments) {
  using CppCrate::Query;

  Query q("", "");
  EXPECT_EQ(q.arguments(), "");
  q.setArguments("a");
  EXPECT_EQ(q.arguments(), "a");
  q.setArguments("");
  EXPECT_EQ(q.arguments(), "");
}

TEST(QueryTests, BulkArguments) {
  using CppCrate::Query;

  const std::vector<std::string> emptyBa;
  std::vector<std::string> ba;
  ba.emplace_back("a");
  ba.emplace_back("b");

  Query q("", emptyBa);
  EXPECT_EQ(q.bulkArguments(), emptyBa);
  q.setBulkArguments(ba);
  EXPECT_EQ(q.bulkArguments(), ba);
  q.setBulkArguments(emptyBa);
  EXPECT_EQ(q.bulkArguments(), emptyBa);
}

TEST(QueryTests, BulkArgumentsAndArguments) {
  using CppCrate::Query;

  // Test if setArguments() and setBulkArguments() clear each other

  const std::vector<std::string> emptyBa;
  std::vector<std::string> ba;
  ba.emplace_back("a");
  ba.emplace_back("b");

  Query q("", "a");
  EXPECT_EQ(q.arguments(), "a");
  EXPECT_EQ(q.bulkArguments(), emptyBa);
  EXPECT_EQ(q.type(), Query::ArgumentType);
  EXPECT_TRUE(q.hasArguments());
  EXPECT_FALSE(q.hasBulkArguments());

  q.setBulkArguments(ba);
  EXPECT_EQ(q.arguments(), "");
  EXPECT_EQ(q.bulkArguments(), ba);
  EXPECT_EQ(q.type(), Query::BulkArgumentType);
  EXPECT_FALSE(q.hasArguments());
  EXPECT_TRUE(q.hasBulkArguments());

  q.setArguments("a");
  EXPECT_EQ(q.arguments(), "a");
  EXPECT_EQ(q.bulkArguments(), emptyBa);
  EXPECT_EQ(q.type(), Query::ArgumentType);
  EXPECT_TRUE(q.hasArguments());
  EXPECT_FALSE(q.hasBulkArguments());
}

TEST(QueryTests, Equal) {
  using CppCrate::Query;

  std::vector<std::string> ba;
  ba.emplace_back("a");
  ba.emplace_back("b");

  Query a("");
  Query b("");
  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);

  a.setStatement("a");
  EXPECT_NE(a, b);
  b.setStatement("a");
  EXPECT_EQ(a, b);

  a.setArguments("a");
  EXPECT_NE(a, b);
  b.setArguments("a");
  EXPECT_EQ(a, b);

  a.setBulkArguments(ba);
  EXPECT_NE(a, b);
  b.setBulkArguments(ba);
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
