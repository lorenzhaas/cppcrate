#include <gtest/gtest.h>

#include <cppcrate/blobresult.h>

TEST(BlobResultTests, Contructors) {
  using CppCrate::BlobResult;

  BlobResult r;
  EXPECT_TRUE(r);
  EXPECT_FALSE(r.hasError());
  EXPECT_EQ(r.errorString(), "");
  EXPECT_EQ(r.errorType(), BlobResult::OtherErrorType);
  EXPECT_EQ(r.key(), "");

  BlobResult r2("error", BlobResult::HttpErrorType);
  EXPECT_FALSE(r2);
  EXPECT_TRUE(r2.hasError());
  EXPECT_EQ(r2.errorString(), "error");
  EXPECT_EQ(r2.errorType(), BlobResult::HttpErrorType);
  EXPECT_EQ(r2.key(), "");
}

TEST(BlobResultTests, Error) {
  using CppCrate::BlobResult;

  BlobResult r;
  EXPECT_EQ(r.errorString(), "");
  EXPECT_EQ(r.errorType(), BlobResult::OtherErrorType);

  r.setErrorString("a", BlobResult::CrateErrorType);
  EXPECT_EQ(r.errorString(), "a");
  EXPECT_EQ(r.errorType(), BlobResult::CrateErrorType);

  r.setErrorString("", BlobResult::OtherErrorType);
  EXPECT_EQ(r.errorString(), "");
  EXPECT_EQ(r.errorType(), BlobResult::OtherErrorType);
}

TEST(BlobResultTests, ErrorType) {
  using CppCrate::BlobResult;

  BlobResult r;
  r.setErrorString("a", BlobResult::CrateErrorType);
  EXPECT_TRUE(r.hasError());
  EXPECT_FALSE(r);
  EXPECT_EQ(r.errorType(), BlobResult::CrateErrorType);
  EXPECT_TRUE(r.isCrateError());

  r.setErrorString("a", BlobResult::HttpErrorType);
  EXPECT_TRUE(r.hasError());
  EXPECT_FALSE(r);
  EXPECT_EQ(r.errorType(), BlobResult::HttpErrorType);
  EXPECT_FALSE(r.isCrateError());
}

TEST(BlobResultTests, Key) {
  using CppCrate::BlobResult;

  BlobResult r;
  EXPECT_EQ(r.key(), "");

  r.setKey("a");
  EXPECT_EQ(r.key(), "a");

  r.setKey("");
  EXPECT_EQ(r.key(), "");
}

TEST(BlobResultTests, Equal) {
  using CppCrate::BlobResult;

  BlobResult a;
  BlobResult b;
  EXPECT_EQ(a, b);
  EXPECT_EQ(b, a);

  a.setErrorString("a", a.errorType());
  EXPECT_NE(a, b);
  b.setErrorString("a", b.errorType());
  EXPECT_EQ(a, b);

  a.setErrorString(a.errorString(), BlobResult::CrateErrorType);
  EXPECT_NE(a, b);
  b.setErrorString(b.errorString(), BlobResult::CrateErrorType);
  EXPECT_EQ(a, b);

  a.setKey("a");
  EXPECT_NE(a, b);
  b.setKey("a");
  EXPECT_EQ(a, b);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
