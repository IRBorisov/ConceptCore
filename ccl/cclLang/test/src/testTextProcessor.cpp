#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/lang/TextProcessor.h"
#include "ccl/lang/Literals.h"

using ccl::lang::operator""_form;

class UTTextProcessor : public ::testing::Test {
protected:
  using TextProcessor = ccl::lang::TextProcessor;
  using Morphology = ccl::lang::Morphology;

  TextProcessor processor{};
};

TEST_F(UTTextProcessor, DefaultImplementations) {
  EXPECT_EQ(processor.Inflect("invalid", Morphology{}), "invalid");
  EXPECT_EQ(processor.Inflect("term", "sing,nomn"_form), "term");
  EXPECT_EQ(processor.InflectDependant("", ""), "");
  EXPECT_EQ(processor.InflectDependant("invalid", ""), "invalid");
}

TEST_F(UTTextProcessor, IsSubstr) {
  EXPECT_TRUE(processor.IsSubstr("", ""));
  EXPECT_TRUE(processor.IsSubstr("", "42"));
  EXPECT_TRUE(processor.IsSubstr("4", "42"));
  EXPECT_TRUE(processor.IsSubstr("2", "42"));
  EXPECT_TRUE(processor.IsSubstr("42", "42"));
  EXPECT_TRUE(processor.IsSubstr("test", "Test"));
  EXPECT_TRUE(processor.IsSubstr("T?est", "Test"));
  EXPECT_TRUE(processor.IsSubstr("T?est", "est"));
  EXPECT_TRUE(processor.IsSubstr("?est", "?est"));
  EXPECT_TRUE(processor.IsSubstr("?est", "?esT"));
  EXPECT_FALSE(processor.IsSubstr("42", ""));
  EXPECT_FALSE(processor.IsSubstr("[a-b][a", "abba"));
}