#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/CstType.hpp"

class UTCstType : public ::testing::Test {
protected:
  using CstType = ccl::semantic::CstType;
};

TEST_F(UTCstType, IsBasic) {
  EXPECT_TRUE(ccl::semantic::IsBasic(CstType::base));
  EXPECT_TRUE(ccl::semantic::IsBasic(CstType::constant));
  EXPECT_TRUE(ccl::semantic::IsBasic(CstType::structured));
  EXPECT_FALSE(ccl::semantic::IsBasic(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsBasic(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsBasic(CstType::function));
  EXPECT_FALSE(ccl::semantic::IsBasic(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsBasic(CstType::predicate));
}

TEST_F(UTCstType, IsRSObject) {
  EXPECT_TRUE(ccl::semantic::IsRSObject(CstType::base));
  EXPECT_TRUE(ccl::semantic::IsRSObject(CstType::constant));
  EXPECT_TRUE(ccl::semantic::IsRSObject(CstType::structured));
  EXPECT_FALSE(ccl::semantic::IsRSObject(CstType::axiom));
  EXPECT_TRUE(ccl::semantic::IsRSObject(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsRSObject(CstType::function));
  EXPECT_FALSE(ccl::semantic::IsRSObject(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsRSObject(CstType::predicate));
}

TEST_F(UTCstType, IsBaseSet) {
  EXPECT_TRUE(ccl::semantic::IsBaseSet(CstType::base));
  EXPECT_TRUE(ccl::semantic::IsBaseSet(CstType::constant));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::structured));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::function));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsBaseSet(CstType::predicate));
}

TEST_F(UTCstType, IsBaseNotion) {
  EXPECT_TRUE(ccl::semantic::IsBaseNotion(CstType::base));
  EXPECT_TRUE(ccl::semantic::IsBaseNotion(CstType::constant));
  EXPECT_TRUE(ccl::semantic::IsBaseNotion(CstType::structured));
  EXPECT_FALSE(ccl::semantic::IsBaseNotion(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsBaseNotion(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsBaseNotion(CstType::function));
  EXPECT_FALSE(ccl::semantic::IsBaseNotion(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsBaseNotion(CstType::predicate));
}

TEST_F(UTCstType, IsCalculable) {
  EXPECT_FALSE(ccl::semantic::IsCalculable(CstType::base));
  EXPECT_FALSE(ccl::semantic::IsCalculable(CstType::constant));
  EXPECT_FALSE(ccl::semantic::IsCalculable(CstType::structured));
  EXPECT_TRUE(ccl::semantic::IsCalculable(CstType::axiom));
  EXPECT_TRUE(ccl::semantic::IsCalculable(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsCalculable(CstType::function));
  EXPECT_TRUE(ccl::semantic::IsCalculable(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsCalculable(CstType::predicate));
}

TEST_F(UTCstType, IsCallable) {
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::base));
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::constant));
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::structured));
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::term));
  EXPECT_TRUE(ccl::semantic::IsCallable(CstType::function));
  EXPECT_FALSE(ccl::semantic::IsCallable(CstType::theorem));
  EXPECT_TRUE(ccl::semantic::IsCallable(CstType::predicate));
}

TEST_F(UTCstType, IsStatement) {
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::base));
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::constant));
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::structured));
  EXPECT_TRUE(ccl::semantic::IsStatement(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::function));
  EXPECT_TRUE(ccl::semantic::IsStatement(CstType::theorem));
  EXPECT_FALSE(ccl::semantic::IsStatement(CstType::predicate));
}

TEST_F(UTCstType, IsLogical) {
  EXPECT_FALSE(ccl::semantic::IsLogical(CstType::base));
  EXPECT_FALSE(ccl::semantic::IsLogical(CstType::constant));
  EXPECT_FALSE(ccl::semantic::IsLogical(CstType::structured));
  EXPECT_TRUE(ccl::semantic::IsLogical(CstType::axiom));
  EXPECT_FALSE(ccl::semantic::IsLogical(CstType::term));
  EXPECT_FALSE(ccl::semantic::IsLogical(CstType::function));
  EXPECT_TRUE(ccl::semantic::IsLogical(CstType::theorem));
  EXPECT_TRUE(ccl::semantic::IsLogical(CstType::predicate));
}