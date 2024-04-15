#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/ops/EquationOptions.h"

struct FakeOptions : ccl::ops::Options {
  bool IsEqualTo(const Options& /*opt2*/) const override { return false; }
};

class UTEquationOptions : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using Equation = ccl::ops::Equation;
  using EquationOptions = ccl::ops::EquationOptions;
};

TEST_F(UTEquationOptions, OptionCompare) {
  EXPECT_EQ(Equation{}, Equation{});
  EXPECT_EQ(Equation(Equation::Mode::keepHier, "42"), Equation(Equation::Mode::keepHier, "42"));
  EXPECT_NE(Equation(Equation::Mode::keepHier, "42"), Equation(Equation::Mode::keepHier, "43"));
  EXPECT_NE(Equation(Equation::Mode::keepDel, "42"), Equation(Equation::Mode::keepHier, "42"));
}

TEST_F(UTEquationOptions, Empty) {
  EquationOptions opts{};
  EXPECT_TRUE(std::empty(opts));
  EXPECT_EQ(std::size(opts), 0U);

  opts.Insert(EntityUID{ 42 }, EntityUID{ 43 });
  EXPECT_FALSE(std::empty(opts));
  EXPECT_EQ(std::size(opts), 1U);
}

TEST_F(UTEquationOptions, IsEqualTo) {
  EquationOptions opts{};
  EXPECT_TRUE(opts.IsEqualTo(opts));
  EXPECT_FALSE(opts.IsEqualTo(FakeOptions()));

  opts.Insert(EntityUID{ 42 }, EntityUID{ 43 });
  EXPECT_TRUE(opts.IsEqualTo(EquationOptions(EntityUID{ 42 }, EntityUID{ 43 })));
}

TEST_F(UTEquationOptions, Access) {
  const Equation equation{};
  const EquationOptions opts{ EntityUID{ 42 }, EntityUID{ 43 }, equation };
  EXPECT_FALSE(opts.ContainsKey(EntityUID{ 43 }));
  EXPECT_TRUE(opts.ContainsKey(EntityUID{ 42 }));

  EXPECT_FALSE(opts.ContainsValue(EntityUID{ 42 }));
  EXPECT_TRUE(opts.ContainsValue(EntityUID{ 43 }));

  EXPECT_EQ(opts.PropsFor(EntityUID{ 42 }), equation);
}

TEST_F(UTEquationOptions, SwapKeyVal) {
  EquationOptions opts{};
  EXPECT_FALSE(opts.SwapKeyVal(EntityUID{ 42 }));

  opts.Insert(EntityUID{ 42 }, EntityUID{ 43 });
  EXPECT_TRUE(opts.SwapKeyVal(EntityUID{ 42 }));
  ASSERT_EQ(std::size(opts), 1U);
  EXPECT_EQ(std::begin(opts)->first, EntityUID { 43 });
  EXPECT_EQ(std::begin(opts)->second, EntityUID { 42 });

  opts.Insert(EntityUID{ 42 }, EntityUID{ 44 });
  EXPECT_FALSE(opts.SwapKeyVal(EntityUID{ 43 }));
  EXPECT_TRUE(opts.SwapKeyVal(EntityUID{ 42 }));
  EXPECT_TRUE(opts.SwapKeyVal(EntityUID{ 43 }));
}