#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/Auditor.h"
#include "ccl/rslang/RSGenerator.h"

class UTAuditor : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using Auditor = ccl::rslang::Auditor;
  using LogicT = ccl::rslang::LogicT;
  using ValueClass = ccl::rslang::ValueClass;

protected:
  UTAuditor();

  RSEnvironment env{};
  Auditor checker{ env, env.GetValueContext(), env.GetAST() };
};

UTAuditor::UTAuditor() {
  env.InsertBase("X1");
  env.Insert("S1", "B(X1*X1)"_t);
}

TEST_F(UTAuditor, EmptyInput) {
  EXPECT_FALSE(checker.CheckType({}));
  EXPECT_TRUE(std::holds_alternative<ccl::rslang::LogicT>(checker.GetType()));
  EXPECT_FALSE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::invalid);
}

TEST_F(UTAuditor, CheckCorrect) {
  ASSERT_TRUE(checker.CheckType(R"(1 \eq 1)"));
  EXPECT_TRUE(std::holds_alternative<LogicT>(checker.GetType()));
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::value);

  ASSERT_TRUE(checker.CheckType(R"(1 \plus 1)"));
  EXPECT_EQ(std::get<Typification>(checker.GetType()), "Z"_t);
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::value);

  ASSERT_TRUE(checker.CheckType(R"(1 \plus 1)"));
  EXPECT_EQ(std::get<Typification>(checker.GetType()), "Z"_t);
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::value);

  ASSERT_TRUE(checker.CheckType("Pr2(S1)"));
  EXPECT_EQ(std::get<Typification>(checker.GetType()), "B(X1)"_t);
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::value);

  ASSERT_TRUE(checker.CheckType(R"(X1 \setminus X1)"));
  EXPECT_EQ(std::get<Typification>(checker.GetType()), "B(X1)"_t);
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::value);

  ASSERT_TRUE(checker.CheckType(R"(D{ a \in B(X1) | a \eq a})"));
  EXPECT_EQ(std::get<Typification>(checker.GetType()), "BB(X1)"_t);
  ASSERT_TRUE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::props);
}

TEST_F(UTAuditor, CheckInvalid) {
  EXPECT_FALSE(checker.CheckType(R"(BB(1)"));
  EXPECT_FALSE(checker.CheckValue());

  EXPECT_FALSE(checker.CheckType(R"(Pr3(S1))"));
  EXPECT_FALSE(checker.CheckValue());

  ASSERT_TRUE(checker.CheckType(R"(card(B(X1) \setminus B(X1)))"));
  ASSERT_FALSE(checker.CheckValue());
  EXPECT_EQ(checker.GetValueClass(), ValueClass::invalid);
}