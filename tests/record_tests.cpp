#include <gtest/gtest.h>

#include <cppcrate/record.h>

TEST(RecordTests, Contructors) {
  using CppCrate::Record;
  using CppCrate::CrateDataType;
  using CppCrate::Value;

  Record r;
  EXPECT_EQ(r.size(), 0);
  EXPECT_TRUE(r.value(0).isInvalid());
  EXPECT_TRUE(r.value("a").isInvalid());

  Record r2("[1,\"Hobbes\"]", {"a", "b"},
            {CrateDataType(CrateDataType::Integer), CrateDataType(CrateDataType::String)});
  EXPECT_EQ(r2.size(), 2);

  Value v1 = r2.value(0);
  EXPECT_EQ(v1, r2.value("a"));
  EXPECT_EQ(v1.name(), "a");
  EXPECT_EQ(v1.crateType().type(), CrateDataType::Integer);
  EXPECT_EQ(v1.asInt16(), 1);

  Value v2 = r2.value(1);
  EXPECT_EQ(v2, r2.value("b"));
  EXPECT_EQ(v2.name(), "b");
  EXPECT_EQ(v2.crateType().type(), CrateDataType::String);
  EXPECT_EQ(v2.asString(), "Hobbes");
}

TEST(RecordTests, Size) {
  using CppCrate::Record;
  using CppCrate::CrateDataType;
  using CppCrate::Value;

  const CrateDataType type(CrateDataType::Integer);

  Record r;
  EXPECT_EQ(r.size(), 0);

  r = Record("[1]", {"a"}, {type});
  EXPECT_EQ(r.size(), 1);

  // name and type did not affect size
  r = Record("[1]", {"a", "b"}, {type});
  EXPECT_EQ(r.size(), 1);
  r = Record("[1]", {"a", "b"}, {type, type});
  EXPECT_EQ(r.size(), 1);
  r = Record("[1]", {"a"}, {type, type});
  EXPECT_EQ(r.size(), 1);

  // Mismatch in data, name, and type are irrelevant
  r = Record("[1, 2]", {"a"}, {type});
  EXPECT_EQ(r.size(), 2);
  EXPECT_EQ(r.value(1).type(), Value::StringType);
  EXPECT_EQ(r.value(1).crateType().type(), CrateDataType::NotSupported);
  EXPECT_EQ(r.value(1).name(), "");
}

TEST(RecordTests, Value) {
  using CppCrate::Record;
  using CppCrate::CrateDataType;
  using CppCrate::Value;

  const CrateDataType type(CrateDataType::Integer);
  Record r("[1, 2]", {"a", "b"}, {type, type});
  EXPECT_EQ(r.value(0), r.value("a"));
  EXPECT_EQ(r.value(1), r.value("b"));

  EXPECT_EQ(r.value(-1), Value());
  EXPECT_EQ(r.value(20), Value());
  EXPECT_EQ(r.value(""), Value());
  EXPECT_EQ(r.value("Calvin"), Value());
}

TEST(RecordTests, Equal) {
  using CppCrate::Record;
  using CppCrate::CrateDataType;

  EXPECT_EQ(Record(), Record());

  const CrateDataType type(CrateDataType::Integer);
  Record r("[1]", {"a"}, {type});
  EXPECT_EQ(r, r);
  EXPECT_NE(r, Record("[2]", {"a"}, {type}));
  EXPECT_NE(r, Record("[1]", {"b"}, {type}));
  EXPECT_NE(r, Record("[1]", {"a"}, {CrateDataType(CrateDataType::String)}));
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
