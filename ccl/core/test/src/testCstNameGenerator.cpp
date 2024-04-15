#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/tools/CstNameGenerator.h"

#include <unordered_set>

class UTGenerateCstName : public ::testing::Test {
protected:
  using CstNameGenerator = ccl::tools::CstNameGenerator;
  using CstTypeIter = ccl::semantic::CstTypeIter;

protected:
  CstNameGenerator generator{};
};

TEST_F(UTGenerateCstName, UniqueTypeNames) {
  std::unordered_set<std::string> names;
  for (const auto aType : CstTypeIter{}) {
    names.emplace(generator.NewNameFor(aType));
  }
  EXPECT_EQ(CstTypeIter::size(), size(names));
}

TEST_F(UTGenerateCstName, TypeFromName) {
  for (const auto aType : CstTypeIter{}) {
    auto name = generator.NewNameFor(aType);
    auto testType = CstNameGenerator::GetTypeForName(name);
    ASSERT_TRUE(testType.has_value());
    EXPECT_EQ(testType.value(), aType);
  }
  EXPECT_FALSE(CstNameGenerator::GetTypeForName("invalid").has_value());
}

TEST_F(UTGenerateCstName, InvalidName) {
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("invalid"));
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("1X"));
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("X1X2"));
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("J5"));
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("X1 X2"));
  EXPECT_FALSE(CstNameGenerator::IsNameCorrect("H1"));
}

TEST_F(UTGenerateCstName, InvalidNameForType) {
  for (const auto aType : CstTypeIter{}) {
    auto name = generator.NewNameFor(aType);
    for (const auto testType : CstTypeIter{}) {
      EXPECT_FALSE(testType != aType && CstNameGenerator::GetTypeForName(name).value() == testType);
    }
  }
}