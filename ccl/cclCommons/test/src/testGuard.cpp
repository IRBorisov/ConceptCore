#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/cclTypes.hpp"

namespace ct = ccl::types;

TEST(UTGuard, CounterIncrement) {
  auto c = 0;
  const auto guard = ct::CounterGuard(c);
  EXPECT_EQ(c, 1);
}

TEST(UTGuard, CounterDecrementOnDestroy) {
  auto c = 0;
  {
    const auto guard = ct::CounterGuard(c);
    EXPECT_EQ(c, 1);
  }
  EXPECT_EQ(c, 0);
}

TEST(UTGuard, CounterDoubleGuard) {
  auto c = 0;
  {
    const auto guard1 = ct::CounterGuard(c);
    EXPECT_EQ(c, 1);
    {
      const auto guard2 = ct::CounterGuard(c);
      EXPECT_EQ(c, 2);
    }
    EXPECT_EQ(c, 1);
  }
  EXPECT_EQ(c, 0);
}

TEST(UTGuard, CounterRelease) {
  auto c = 0;
  {
    auto guard = ct::CounterGuard(c);
    EXPECT_EQ(c, 1);
    guard.Release();
    EXPECT_EQ(c, 0);
  }
  EXPECT_EQ(c, 0);
}

TEST(UTGuard, GuardableDefault) {
  const auto val = ct::GuardableBool{ true };
  EXPECT_TRUE(val);
  EXPECT_FALSE(val.IsGuarded());

  const auto val2 = ct::GuardableBool{ false };
  EXPECT_FALSE(val2);
  EXPECT_FALSE(val2.IsGuarded());
}

TEST(UTGuard, GuardableSpawnGuard) {
  auto val = ct::GuardableBool{ true };
  const auto guard = val.CreateGuard();
  EXPECT_FALSE(val);
  EXPECT_TRUE(val.IsGuarded());

  auto val2 = ct::GuardableBool{ false };
  const auto guard2 = val2.CreateGuard();
  EXPECT_TRUE(val2);
  EXPECT_TRUE(val2.IsGuarded());
}