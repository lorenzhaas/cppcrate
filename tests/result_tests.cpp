#include <gtest/gtest.h>

#include <cppcrate/client.h>
#include <cppcrate/result.h>

TEST(NodeTests, ContructorNormal) {
  using CppCrate::Result;
  using CppCrate::Record;
  using CppCrate::RawResult;
  using CppCrate::CrateDataType;

  const RawResult rawGood(
      "{\"cols\":[\"age\",\"name\"],\"col_types\":[9,4],\"rows\":[[7,\"Calvin\"],[5,\"Hobbes\"]],"
      "\"rowcount\":2,\"duration\":5.341253}");

  Result result(rawGood);
  EXPECT_TRUE(result);
  EXPECT_FALSE(result.hasError());
  EXPECT_EQ(result.errorString(), "");
  EXPECT_EQ(result.rawResult(), rawGood);
  EXPECT_DOUBLE_EQ(result.duration(), 5.341253);
  EXPECT_EQ(result.rowCount(), 2);

  std::vector<std::string> cols;
  cols.emplace_back("age");
  cols.emplace_back("name");
  EXPECT_EQ(result.cols(), cols);

  std::vector<CrateDataType> colTypes;
  colTypes.emplace_back(CrateDataType(CrateDataType::Integer, "9"));
  colTypes.emplace_back(CrateDataType(CrateDataType::String, "4"));
  EXPECT_EQ(result.colTypes(), colTypes);

  std::vector<std::string> rows;
  rows.emplace_back("[7,\"Calvin\"]");
  rows.emplace_back("[5,\"Hobbes\"]");
  EXPECT_EQ(result.rows(), rows);

  EXPECT_EQ(result.recordSize(), 2);
  EXPECT_EQ(result.record(0), Record(rows.front(), cols, colTypes));
  EXPECT_EQ(result.record(1), Record(rows.back(), cols, colTypes));
  EXPECT_EQ(result.record(2), Record());
  EXPECT_EQ(result.record(-1), Record());

  // An array as column type (code coverage)
  result = Result(RawResult("{\"cols\":[\"set\"],\"col_types\":[[100,9]]}"));
  EXPECT_TRUE(result);
}

TEST(NodeTests, ContructorErroneous) {
  using CppCrate::Result;
  using CppCrate::RawResult;

  // Invalid JSON
  Result result(RawResult("a"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  // This is to please code coverage...
  result = Result(RawResult("{\"cols\":[\"age\",42]}"));
  EXPECT_TRUE(result);
  EXPECT_EQ(result.errorString(), "");

  // Crate Error
  result = Result(RawResult("{\"error\":{\"message\":\"foo\",\"code\":42}}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  result = Result(RawResult("{\"error\":{\"message\":null,\"code\":false}}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  result = Result(RawResult("{\"error\":true}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  // CppCrate Error
  result = Result(RawResult("{\"error\":{\"message\":\"foo\",\"code\":42,\"component\":\"bar\"}}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  result = Result(RawResult("{\"error\":{\"message\":\"foo\",\"code\":42,\"component\":42}}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");

  // Bulk errors
  result = Result(RawResult("{\"results\":[{\"rowcount\":-2},{\"rowcount\":-2}]}"));
  EXPECT_FALSE(result);
  EXPECT_NE(result.errorString(), "");
}

TEST(NodeTests, Equal) {
  using CppCrate::Result;
  using CppCrate::RawResult;

  const RawResult raw1(
      "{\"cols\":[\"a\"],\"col_types\":[9],\"rows\":[[1]],\"rowcount\":1,\"duration\":1}");
  const RawResult raw2(
      "{\"cols\":[\"b\"],\"col_types\":[9],\"rows\":[[1]],\"rowcount\":1,\"duration\":1}");

  EXPECT_EQ(Result(raw1), Result(raw1));
  EXPECT_NE(Result(raw1), Result(raw2));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
