#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/tools/EntityGenerator.h"
#include "ccl/tools/CstNameGenerator.h"

template <typename Generator>
class UTGenerator : public ::testing::Test {
public:
  Generator generator{};
};

using GeneratorTypes = ::testing::Types<ccl::tools::EntityGenerator, ccl::tools::CstNameGenerator>;
#ifndef TYPED_TEST_SUITE
  TYPED_TEST_CASE(UTGenerator, GeneratorTypes);
#else
  TYPED_TEST_SUITE(UTGenerator, GeneratorTypes);
#endif

TYPED_TEST(UTGenerator, SimpleGeneration) {
  EXPECT_NE(this->generator.NewUID(), this->generator.NewUID());
  EXPECT_TRUE(this->generator.IsTaken(this->generator.NewUID()));
}

TYPED_TEST(UTGenerator, Clear) {
  auto newID = this->generator.NewUID();
  EXPECT_TRUE(this->generator.IsTaken(newID));
  this->generator.Clear();
  EXPECT_FALSE(this->generator.IsTaken(newID));
}

TYPED_TEST(UTGenerator, AddUID) {
  auto newID = this->generator.NewUID();
  this->generator.Clear();
  this->generator.AddUID(newID);
  EXPECT_TRUE(this->generator.IsTaken(newID));
}

TYPED_TEST(UTGenerator, FreeUID) {
  auto newID = this->generator.NewUID();
  this->generator.FreeUID(newID);
  EXPECT_FALSE(this->generator.IsTaken(newID));
}
