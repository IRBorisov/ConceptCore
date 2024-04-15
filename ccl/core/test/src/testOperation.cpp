#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/Operation.hpp"

class UTOperation : public ::testing::Test {
protected:
  using Type = ccl::ops::Type;
};

TEST_F(UTOperation, HasOptions) {
  using ccl::ops::HasOptions;
  EXPECT_TRUE(HasOptions(Type::rsSynt));
  EXPECT_FALSE(HasOptions(Type::tba));
  EXPECT_FALSE(HasOptions(Type::rsMerge));
}