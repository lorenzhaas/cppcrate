#include <gtest/gtest.h>

#include <cppcrate/rawresult.h>

TEST(RawResultTests, Contructors) {
  using CppCrate::RawResult;

  RawResult r;
  EXPECT_FALSE(r);
  EXPECT_TRUE(r.hasError());
  EXPECT_TRUE(r.isEmpty());
  EXPECT_EQ(r.httpStatusCode(), -1);
  EXPECT_EQ(r.reply(), "");

  RawResult r2(500);
  EXPECT_FALSE(r2);
  EXPECT_TRUE(r2.hasError());
  EXPECT_FALSE(r2.isEmpty());
  EXPECT_EQ(r2.httpStatusCode(), 500);
  EXPECT_EQ(r2.reply(), "");

  RawResult r3("{}");
  EXPECT_TRUE(r3);
  EXPECT_FALSE(r3.hasError());
  EXPECT_FALSE(r3.isEmpty());
  EXPECT_EQ(r3.httpStatusCode(), -1);
  EXPECT_EQ(r3.reply(), "{}");

  RawResult r4("{}", 500);
  EXPECT_TRUE(r4);
  EXPECT_FALSE(r4.hasError());
  EXPECT_FALSE(r4.isEmpty());
  EXPECT_EQ(r4.httpStatusCode(), 500);
  EXPECT_EQ(r4.reply(), "{}");
}

TEST(RawResultTests, HttpStatusCode) {
  using CppCrate::RawResult;

  RawResult r;
  r.setHttpStatusCode(1);
  EXPECT_EQ(r.httpStatusCode(), 1);
  r.setHttpStatusCode(0);
  EXPECT_EQ(r.httpStatusCode(), 0);
  r.setHttpStatusCode(-1);
  EXPECT_EQ(r.httpStatusCode(), -1);
}

TEST(RawResultTests, Reply) {
  using CppCrate::RawResult;

  RawResult r;
  r.setReply("a");
  EXPECT_EQ(r.reply(), "a");
  r.setReply("");
  EXPECT_EQ(r.reply(), "");
}

TEST(RawResultTests, IsEmpty) {
  using CppCrate::RawResult;

  RawResult r;
  EXPECT_TRUE(r.isEmpty());

  r.setHttpStatusCode(0);
  EXPECT_FALSE(r.isEmpty());
  r.setHttpStatusCode(-1);
  EXPECT_TRUE(r.isEmpty());

  r.setReply("a");
  EXPECT_FALSE(r.isEmpty());
  r.setReply("");
  EXPECT_TRUE(r.isEmpty());

  r.setHttpStatusCode(0);
  r.setReply("a");
  EXPECT_FALSE(r.isEmpty());
}

TEST(RawResultTests, ErrorDetection) {
  using CppCrate::RawResult;

  RawResult r;

  // Empty reply is considered an error
  r.setReply("");
  EXPECT_TRUE(r.hasError());

  // No error, normal JSON
  r.setReply("{\"a\":0}");
  EXPECT_FALSE(r.hasError());

  // No valid JSON
  r.setReply("a");
  EXPECT_TRUE(r.hasError());

  // Crate errors contain a member "error"
  r.setReply("{\"error\": 0}");
  EXPECT_TRUE(r.hasError());

  // No bulk errors
  r.setReply("{\"results\":[{\"rowcount\":1},{\"rowcount\":1}]}");
  EXPECT_FALSE(r.hasError());

  // Bulk errors even if some operations succeeded
  r.setReply("{\"results\":[{\"rowcount\":-2},{\"rowcount\":1}]}");
  EXPECT_TRUE(r.hasError());
  r.setReply("{\"results\":[{\"rowcount\":1},{\"rowcount\":-2}]}");
  EXPECT_TRUE(r.hasError());
  r.setReply("{\"results\":[{\"rowcount\":-2},{\"rowcount\":-2}]}");
  EXPECT_TRUE(r.hasError());
}

TEST(RawResultTests, Equal) {
  using CppCrate::RawResult;

  RawResult a;
  RawResult b;
  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);

  a.setHttpStatusCode(100);
  EXPECT_NE(a, b);
  b.setHttpStatusCode(100);
  EXPECT_EQ(a, b);

  a.setReply("a");
  EXPECT_NE(a, b);
  b.setReply("a");
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
