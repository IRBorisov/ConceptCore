#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/RSExpr.h"
#include "ccl/rslang/SyntaxTree.h"

class UTRSExpr : public ::testing::Test {
protected:
  using TFFactory = ccl::rslang::TFFactory;
  using TokenID = ccl::rslang::TokenID;

protected:
  UTRSExpr();

  ccl::StrSubstitutes idMap{};
};

UTRSExpr::UTRSExpr() {
  idMap.insert({ "X1", "X42" });
  idMap.insert({ "a", "b" });
  idMap.insert({ "X42", "X43" });
  idMap.insert({ "ab", "ba" });
}

TEST_F(UTRSExpr, TransitionTranslator) {
  const auto trans = TFFactory::GetTransition(idMap);
  EXPECT_EQ(trans("X1").value(), "X42");
  EXPECT_EQ(trans("ab").value(), "ba");
  EXPECT_EQ(trans("b").value(), "b_ERROR");
  EXPECT_EQ(trans("X43").value(), "X43_ERROR");
  EXPECT_EQ(trans("b_ERROR").value(), "b_ERROR");
}

TEST_F(UTRSExpr, CreateFilter) {
  auto filter = TFFactory::GetFilter({});
  EXPECT_FALSE(filter(TokenID::ID_LOCAL));
  EXPECT_FALSE(filter(TokenID::ID_GLOBAL));
  EXPECT_FALSE(filter(TokenID::ID_RADICAL));
  EXPECT_FALSE(filter(TokenID::INTERRUPT));

  filter = TFFactory::GetFilter({ TokenID::ID_GLOBAL });
  EXPECT_FALSE(filter(TokenID::ID_LOCAL));
  EXPECT_TRUE(filter(TokenID::ID_GLOBAL));
  EXPECT_FALSE(filter(TokenID::ID_RADICAL));
  EXPECT_FALSE(filter(TokenID::INTERRUPT));
}

TEST_F(UTRSExpr, FilterGlobals) {
  const auto& filter = TFFactory::FilterGlobals();
  EXPECT_FALSE(filter(TokenID::ID_LOCAL));
  EXPECT_TRUE(filter(TokenID::ID_GLOBAL));
  EXPECT_FALSE(filter(TokenID::ID_RADICAL));
  EXPECT_TRUE(filter(TokenID::ID_FUNCTION));
  EXPECT_TRUE(filter(TokenID::ID_PREDICATE));
  EXPECT_FALSE(filter(TokenID::INTERRUPT));
}

TEST_F(UTRSExpr, FilterIDs) {
  const auto& filter = TFFactory::FilterIdentifiers();
  EXPECT_TRUE(filter(TokenID::ID_LOCAL));
  EXPECT_TRUE(filter(TokenID::ID_GLOBAL));
  EXPECT_FALSE(filter(TokenID::ID_RADICAL));
  EXPECT_TRUE(filter(TokenID::ID_FUNCTION));
  EXPECT_TRUE(filter(TokenID::ID_PREDICATE));
  EXPECT_FALSE(filter(TokenID::INTERRUPT));
}

TEST_F(UTRSExpr, Translate) {
  using ccl::operator""_c17;
  std::string input{ u8"\u212C X1 X2 ab ba"_c17 };
  const auto repCount = ccl::rslang::TranslateRS(
    input,
    TFFactory::FilterIdentifiers(),
    ccl::CreateTranslator(idMap)
  );
  EXPECT_EQ(repCount, 2);
  EXPECT_EQ(input, u8"\u212C X42 X2 ba ba"_c17);
}

TEST_F(UTRSExpr, SubstituteGlobals) {
  std::string input{ "X1 X2 ab ba" };
  const auto repCount = ccl::rslang::SubstituteGlobals(input, idMap);
  EXPECT_EQ(repCount, 1);
  EXPECT_EQ(input, "X42 X2 ab ba");
}

TEST_F(UTRSExpr, GenerateAST) {
  EXPECT_TRUE(ccl::rslang::GenerateAST("X1=") == nullptr);
  EXPECT_EQ(ccl::rslang::AST2String::Apply(*ccl::rslang::GenerateAST("1=2")), "[=[1][2]]");
}

TEST_F(UTRSExpr, ExtractUniqueGlobals) {
  const std::unordered_set<std::string> globals{"X1", "X42", "X2_ERROR" };
  EXPECT_EQ(ccl::rslang::ExtractUGlobals("a X1 b X42, X1 X2_ERROR"), globals);

  const std::unordered_set<std::string> globals2{ "X1" };
  EXPECT_EQ(ccl::rslang::ExtractUGlobals("X1"), globals2);

  const std::unordered_set<std::string> globals3{ "X2" };
  EXPECT_EQ(ccl::rslang::ExtractUGlobals("X2"), globals3);
}

TEST_F(UTRSExpr, ExtractULocals) {
  const std::unordered_set<std::string> globals{ "a", "b" };
  EXPECT_EQ(ccl::rslang::ExtractULocals("a X1 b X42, X1"), globals);
}
