#include <gtest/gtest.h>

#include <cppcrate/cratedatatype.h>

TEST(CrateDataTypeTests, Contructors) {
  using CppCrate::CrateDataType;

  CrateDataType dt;
  EXPECT_EQ(dt.type(), CrateDataType::NotSupported);
  EXPECT_EQ(dt.definition(), "");

  CrateDataType dt2(CrateDataType::Boolean);
  EXPECT_EQ(dt2.type(), CrateDataType::Boolean);
  EXPECT_EQ(dt2.definition(), "");

  CrateDataType dt3(CrateDataType::Boolean, "a");
  EXPECT_EQ(dt3.type(), CrateDataType::Boolean);
  EXPECT_EQ(dt3.definition(), "a");
}

TEST(CrateDataTypeTests, Type) {
  using CppCrate::CrateDataType;

  CrateDataType dt;
  EXPECT_EQ(dt.type(), CrateDataType::NotSupported);

  dt.setType(CrateDataType::Boolean);
  EXPECT_EQ(dt.type(), CrateDataType::Boolean);

  // Type number 4 == String
  dt.setType(4);
  EXPECT_EQ(dt.type(), CrateDataType::String);
}

TEST(CrateDataTypeTests, Definition) {
  using CppCrate::CrateDataType;

  CrateDataType dt;
  EXPECT_EQ(dt.definition(), "");

  dt.setDefinition("a");
  EXPECT_EQ(dt.definition(), "a");

  dt.setDefinition("");
  EXPECT_EQ(dt.definition(), "");
}

TEST(CrateDataTypeTests, TypeConversion) {
  using CppCrate::CrateDataType;

  EXPECT_EQ(CrateDataType::convert(0), CrateDataType::Null);
  EXPECT_EQ(CrateDataType::convert(4), CrateDataType::String);

  // Invalid numbers result in NotSupported
  EXPECT_EQ(CrateDataType::convert(-1), CrateDataType::NotSupported);
  EXPECT_EQ(CrateDataType::convert(25), CrateDataType::NotSupported);
  EXPECT_EQ(CrateDataType::convert(110), CrateDataType::NotSupported);
}

TEST(CrateDataTypeTests, Equal) {
  using CppCrate::CrateDataType;

  CrateDataType a;
  CrateDataType b;
  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);

  a.setType(100);
  EXPECT_NE(a, b);
  b.setType(100);
  EXPECT_EQ(a, b);

  a.setDefinition("a");
  EXPECT_NE(a, b);
  b.setDefinition("a");
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
