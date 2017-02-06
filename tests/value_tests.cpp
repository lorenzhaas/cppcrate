#include <gtest/gtest.h>

#include <cppcrate/value.h>

TEST(ValueTests, Contructors) {
  using CppCrate::CrateDataType;
  using CppCrate::Value;

  Value v;
  EXPECT_EQ(v.name(), "");
  EXPECT_EQ(v.crateType().type(), CrateDataType::NotSupported);
  EXPECT_EQ(v.type(), Value::InvalidType);
  EXPECT_TRUE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value(CrateDataType(CrateDataType::Boolean)));

  v = Value(CrateDataType(CrateDataType::Boolean));
  EXPECT_EQ(v.name(), "");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Boolean);
  EXPECT_EQ(v.type(), Value::InvalidType);
  EXPECT_TRUE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value(CrateDataType(CrateDataType::Integer)));

  v = Value(CrateDataType(CrateDataType::Null), true);
  EXPECT_EQ(v.name(), "");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::NullType);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_TRUE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value(CrateDataType(CrateDataType::Integer), true));

  v = Value("a", CrateDataType(CrateDataType::Null), true);
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::BoolType);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), false));

  v = Value("a", CrateDataType(CrateDataType::Null), static_cast<int16_t>(1));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::Int16Type);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), static_cast<int16_t>(2)));

  v = Value("a", CrateDataType(CrateDataType::Null), static_cast<int32_t>(1));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::Int32Type);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), static_cast<int32_t>(2)));

  v = Value("a", CrateDataType(CrateDataType::Null), static_cast<int64_t>(1));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::Int64Type);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), static_cast<int64_t>(2)));

  v = Value("a", CrateDataType(CrateDataType::Null), static_cast<float>(1));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::FloatType);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), static_cast<float>(2)));

  v = Value("a", CrateDataType(CrateDataType::Null), static_cast<double>(1));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::DoubleType);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), static_cast<double>(2)));

  v = Value("a", CrateDataType(CrateDataType::Null), std::string("Calvin"));
  EXPECT_EQ(v.name(), "a");
  EXPECT_EQ(v.crateType().type(), CrateDataType::Null);
  EXPECT_EQ(v.type(), Value::StringType);
  EXPECT_FALSE(v.isInvalid());
  EXPECT_FALSE(v.isNull());
  EXPECT_EQ(v, v);
  EXPECT_NE(v, Value("a", CrateDataType(CrateDataType::Null), std::string("Hobbes")));
}

TEST(ValueTests, PrivateMove) {
  using CppCrate::CrateDataType;
  using CppCrate::Value;

  {
    Value origin = Value("a", CrateDataType(CrateDataType::Null), std::string("Calvin"));
    Value destination(std::move(origin));
    origin.~Value();
    EXPECT_EQ(destination.name(), "a");
    EXPECT_EQ(destination.crateType().type(), CrateDataType::Null);
    EXPECT_EQ(destination.type(), Value::StringType);
    EXPECT_FALSE(destination.isInvalid());
    EXPECT_FALSE(destination.isNull());
    EXPECT_EQ(destination.asString(), "Calvin");
  }

  {
    Value destination;
    {
      Value origin = Value("a", CrateDataType(CrateDataType::Null), std::string("Calvin"));
      destination = std::move(origin);
    }
    EXPECT_EQ(destination.name(), "a");
    EXPECT_EQ(destination.crateType().type(), CrateDataType::Null);
    EXPECT_EQ(destination.type(), Value::StringType);
    EXPECT_FALSE(destination.isInvalid());
    EXPECT_FALSE(destination.isNull());
    EXPECT_EQ(destination.asString(), "Calvin");
  }
}

struct ConvData {
  CppCrate::Value value;
  bool asBool;
  int16_t asInt16;
  int32_t asInt32;
  int64_t asInt64;
  float asFloat;
  double asDouble;
  std::string asString;
};

class ValueTests : public testing::TestWithParam<ConvData> {};

TEST_P(ValueTests, ConvData) {
  ConvData data = GetParam();

  EXPECT_EQ(data.asBool, data.value.asBool());
  EXPECT_EQ(data.asInt16, data.value.asInt16());
  EXPECT_EQ(data.asInt32, data.value.asInt32());
  EXPECT_EQ(data.asInt64, data.value.asInt64());
  EXPECT_NEAR(data.asFloat, data.value.asFloat(), 0.000009);
  EXPECT_NEAR(data.asDouble, data.value.asDouble(), 0.000009);

  // Do not take trailing 0 into account.
  std::string str = data.value.asString();
  data.asString.erase(data.asString.find_last_not_of('0') + 1, std::string::npos);
  str.erase(str.find_last_not_of('0') + 1, std::string::npos);
  if (str.empty()) {
    str = "0";
  } else {
    if ((*str.rbegin()) == '.') {
      str.erase(str.size() - 1, std::string::npos);
    }
  }
  if (data.asString.empty()) {
    data.asString = "0";
  } else {
    if ((*data.asString.rbegin()) == '.') {
      data.asString.erase(data.asString.size() - 1, std::string::npos);
    }
  }
  EXPECT_EQ(data.asString, str);
}

INSTANTIATE_TEST_CASE_P(
    ConvData, ValueTests,
    ::testing::Values(
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null), true),
                 true, 1, 1, 1, 1.0f, 1.0, "true"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null), false),
                 false, 0, 0, 0, 0.0f, 0.0, "false"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 static_cast<int16_t>(1)),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 static_cast<int32_t>(1)),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 static_cast<int64_t>(1)),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 static_cast<float>(1)),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 static_cast<double>(1)),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 std::string("1")),
                 true, 1, 1, 1, 1.0f, 1.0, "1"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 std::string("true")),
                 true, 0, 0, 0, 0.0f, 0.0, "true"},
        ConvData{CppCrate::Value("", CppCrate::CrateDataType(CppCrate::CrateDataType::Null),
                                 std::string("false")),
                 false, 0, 0, 0, 0.0f, 0.0, "false"},
        ConvData{CppCrate::Value(), false, 0, 0, 0, 0.0f, 0.0, ""},
        ConvData{CppCrate::Value(CppCrate::CrateDataType(CppCrate::CrateDataType::Boolean)), false,
                 0, 0, 0, 0.0f, 0.0, ""},
        ConvData{CppCrate::Value(CppCrate::CrateDataType(CppCrate::CrateDataType::Null), true),
                 false, 0, 0, 0, 0.0f, 0.0, ""}));

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
