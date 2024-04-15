#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"
#include "ccl/tools/EntityGenerator.h"

class UTGenerateID : public ::testing::Test {
protected:
  using EntityGenerator = ccl::tools::EntityGenerator;
};

TEST_F(UTGenerateID, CoGeneration) {
  EntityGenerator gen1{};
  EntityGenerator gen2{};
  auto countEqual = 0;
  for (auto i = 0; i < 10; ++i) {
    if (gen1.NewUID() == gen2.NewUID()) {
      ++countEqual;
    }
  }
  EXPECT_NE(countEqual, 10);
}

TEST_F(UTGenerateID, LargeGeneration) {
  EntityGenerator gen{};
  ccl::SetOfEntities generated{};
  for (auto i = 0; i < 1000; ++i) {
    generated.emplace(gen.NewUID());
  }
  EXPECT_EQ(ssize(generated), 1000);
}