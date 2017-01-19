#include <gtest/gtest.h>

#include "../src/crypto.h"

#include <string>

TEST(CryptoTests, FileSize) {
  using CppCrate::Crypto;

  std::string str = "123456";
  std::istringstream stream(str);
  EXPECT_EQ(Crypto::fileSize(stream), 6);
}

TEST(CryptoTests, Sha1) {
  using CppCrate::Crypto;

  std::string str = "123456";
  std::istringstream stream(str);
  EXPECT_EQ(Crypto::sha1(stream), "7c4a8d09ca3762af61e59520943dc26494f8941b");
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
