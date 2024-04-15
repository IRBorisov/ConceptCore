#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"

#include "ccl/semantic/RSForm.h"
#include "ccl/ops/RSEquationProcessor.h"
#include "ccl/lang/Literals.h"

using ccl::lang::operator""_form;

class UTEquationProcessor: public ::testing::Test {
protected:

  using CstType = ccl::semantic::CstType;
  using RSForm = ccl::semantic::RSForm;
  using Equation = ccl::ops::Equation;
  using EquationOptions = ccl::ops::EquationOptions;
  using RSEquationProcessor = ccl::ops::RSEquationProcessor;
  using EntityRef = ccl::lang::EntityRef;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;
  using EntityUID = ccl::EntityUID;

protected:
  RSForm schema{};
  RSEquationProcessor processor{ schema };

  const EntityUID x1{ schema.Emplace(CstType::base) };
  const EntityUID x2{ schema.Emplace(CstType::base) };
  const EntityUID s1{ schema.Emplace(CstType::structured, "B(X1)"_rs) };
  const EntityUID d1{ schema.Emplace(CstType::term, R"(X1 \setminus X1)"_rs) };
};

TEST_F(UTEquationProcessor, Empty) {
  EXPECT_TRUE(std::empty(processor.Translation()));
  EXPECT_FALSE(processor.Evaluate(EquationOptions{}));
  EXPECT_FALSE(processor.Execute(EquationOptions{}));
  EXPECT_TRUE(std::empty(processor.Translation()));
}

TEST_F(UTEquationProcessor, Evaluate) {
  EXPECT_TRUE(processor.Evaluate(EquationOptions{ x1, x2 }));
  EXPECT_TRUE(processor.Evaluate(EquationOptions{ x2, s1 }));
  EXPECT_TRUE(processor.Evaluate(EquationOptions{ x2, d1 }));
  EXPECT_TRUE(processor.Evaluate(EquationOptions{ s1, d1 }));

  EXPECT_FALSE(processor.Evaluate(EquationOptions{ x1, x1 }));
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ d1, x1 }));
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ d1, s1 }));

  schema.Erase(x1);
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ x1, x2 }));
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ x1, x1 }));
}

TEST_F(UTEquationProcessor, EvaluateTracking) {
  const EquationOptions equation{ x1, x2 };
  ASSERT_TRUE(processor.Evaluate(equation));

  schema.Mods().Track(x2);
  EXPECT_TRUE(processor.Evaluate(equation));

  schema.Mods().StopTracking(x2);
  schema.Mods().Track(x1);
  EXPECT_TRUE(processor.Evaluate(equation));

  schema.Mods().Track(x2);
  EXPECT_TRUE(processor.Evaluate(equation));
}

TEST_F(UTEquationProcessor, EvaluateIcompatible) {
  const auto d2 = schema.Emplace(CstType::term, "invalid");
  const auto a1 = schema.Emplace(CstType::axiom, "1=1");
  const auto t1 = schema.Emplace(CstType::theorem, "1=1");
  const auto f1 = schema.Emplace(CstType::function, "");

  EXPECT_FALSE(processor.Evaluate(EquationOptions(x1, s1)));
  const std::vector<EntityUID> incompatibles{ x1, s1, d2, a1, f1, t1 };
  ccl::meta::ForEachPair(incompatibles,
                         [this](const EntityUID uid1, const EntityUID uid2) {
    EXPECT_FALSE(processor.Evaluate(EquationOptions{ uid1, uid2 })) << schema.GetRS(uid1).alias << " ~ " << schema.GetRS(uid2).alias;
  });

  const auto s2 = schema.Emplace(CstType::structured, "B(X1*X1)"_rs);
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ s1, s2 }));

  const auto s3 = schema.Emplace(CstType::structured, schema.GetRS(s1).definition);
  EXPECT_TRUE(processor.Evaluate(EquationOptions{ s1, s3 }));
}

TEST_F(UTEquationProcessor, EvaluateDependable) {
  const auto d2 = schema.Emplace(CstType::term, R"(X1 \setminus S1)"_rs);
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ s1, d2 }));

  schema.SetTermFor(d1, EntityRef(schema.GetRS(s1).alias, "nomn,sing"_form ).ToString());
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ s1, d1 }));
}

TEST_F(UTEquationProcessor, EvaluateChainEquate) {
  const EntityUID s2{ schema.Emplace(CstType::structured, "B(X1)"_rs) };

  EquationOptions opts{};
  opts.Insert(x2, s1);
  opts.Insert(s1, s2);
  EXPECT_FALSE(processor.Evaluate(opts));
}

TEST_F(UTEquationProcessor, EvaluateTerminSelfReference) {
  schema.SetTermFor(x2, "42");
  schema.SetTermFor(x1, tccl::EntityRef("X2") + " test");
  EXPECT_TRUE(processor.Evaluate(EquationOptions{x1, x2}));
  EXPECT_FALSE(processor.Evaluate(EquationOptions{x2, x1}));
}

TEST_F(UTEquationProcessor, EvaluateMultiTypecheck) {
  const auto s2 = schema.Emplace(CstType::structured, "B(X2*X2)"_rs);
  const auto s3 = schema.Emplace(CstType::structured, "B(X2*X2)"_rs);
  EXPECT_FALSE(processor.Evaluate(EquationOptions{ s1, s2 }));

  EquationOptions opts{};
  opts.Insert(x1, s3);
  opts.Insert(s1, s2);
  EXPECT_TRUE(processor.Evaluate(opts));
}

TEST_F(UTEquationProcessor, ExecuteBasics) {
  ASSERT_TRUE(processor.Execute(EquationOptions{ x1, x2 }));

  ccl::EntityTranslation test{};
  test.Insert(x1, x2);
  EXPECT_EQ(processor.Translation(), test);
  EXPECT_FALSE(schema.Contains(x1));
  EXPECT_TRUE(schema.Contains(x2));
  EXPECT_EQ(schema.GetRS(x2).alias, "X2");
  EXPECT_EQ(schema.GetRS(d1).definition, R"(X2 \setminus X2)"_rs);
}

TEST_F(UTEquationProcessor, ExecuteBasicToStructure) {
  const auto s2 = schema.Emplace(CstType::structured, "B(X2)"_rs);
  schema.SetDefinitionFor(s2, "anti duplicate");
  EXPECT_TRUE(processor.Execute(EquationOptions{ x1, s2 }));
  EXPECT_FALSE(schema.Contains(x1));
  EXPECT_TRUE(schema.Contains(s2));
  EXPECT_EQ(schema.GetRS(d1).definition, R"(S2 \setminus S2)"_rs);
  EXPECT_EQ(schema.GetRS(s1).definition, "B(S2)"_rs);
  EXPECT_EQ(schema.RSLang().Count(CstType::axiom), 0U);
}

TEST_F(UTEquationProcessor, ExecuteSubstituteTracking) {
  schema.Mods().Track(d1);
  schema.Mods()(d1)->allowEdit = false;
  EXPECT_TRUE(processor.Execute(EquationOptions{ x1, x2 }));
  EXPECT_EQ(schema.GetRS(d1).definition, R"(X2 \setminus X2)"_rs);
}

TEST_F(UTEquationProcessor, ExecuteSubstituteIncorrect) {
  const auto s2 = schema.Emplace(CstType::structured, "B(X2*X2)"_rs);
  const auto f1 = schema.Emplace(CstType::function, "invalid X1");
  schema.SetConventionFor(s2, "S_2");
  schema.SetExpressionFor(s1, R"(X1 \setminus X1)"_rs);
  EXPECT_EQ(ccl::semantic::ParsingStatus::INCORRECT, schema.GetParse(s1).status);
  ASSERT_TRUE(processor.Execute(EquationOptions{ x1, s2 }));
  EXPECT_EQ(schema.GetRS(s1).definition, R"(S2 \setminus S2)"_rs);
  EXPECT_EQ(schema.GetRS(f1).definition, "invalid S2");
}

TEST_F(UTEquationProcessor, ExecuteSubstituteTermRef) {
  const auto s2 = schema.Emplace(CstType::structured, "B(X2*X2)"_rs);
  schema.SetConventionFor(s2, "S_2");
  schema.SetDefinitionFor(s2, tccl::EntityRef("X1"));
  schema.SetDefinitionFor(s1, tccl::EntityRef("X1"));
  ASSERT_TRUE(processor.Execute(EquationOptions{ x1, s2 }));
  EXPECT_EQ(schema.GetText(s2).definition.Raw(), tccl::EntityRef("S2"));
  EXPECT_EQ(schema.GetText(s1).definition.Raw(), tccl::EntityRef("S2"));
}

TEST_F(UTEquationProcessor, ExecuteSubstituteFunction) {
  const auto funcExpr = R"([a \in X1*X2] {a} \setminus (X1*X2))"_rs;
  const auto testExpr = R"([a \in S2*X2] {a} \setminus (S2*X2))"_rs;
  const auto s2 = schema.Emplace(CstType::structured, "B(X2*X2)"_rs);
  schema.SetConventionFor(s2, "S2");  
  const auto f1 = schema.Emplace(CstType::function, funcExpr);
  ASSERT_TRUE(processor.Execute(EquationOptions{ x1, s2 }));
  EXPECT_EQ(schema.GetRS(f1).definition, testExpr);
}

TEST_F(UTEquationProcessor, ExecuteSubstituteTerm) {
  const auto x3 = schema.Emplace(CstType::base);
  schema.SetTermFor(x1, tccl::EntityRef("X3"));
  schema.SetTermFor(x2, "42");
  schema.SetTermFor(x3, "43");
  ASSERT_TRUE(processor.Execute(EquationOptions{ x3, x2, Equation{ Equation::Mode::keepHier, std::string{} } }));
  EXPECT_EQ(schema.GetText(x1).term.Text().Raw(), tccl::EntityRef("X2"));
  EXPECT_EQ(schema.GetText(x2).term.Text().Raw(), "42");
}

TEST_F(UTEquationProcessor, ExecuteReverseSubstituteTerm) {
  const auto x3 = schema.Emplace(CstType::base);
  schema.SetTermFor(x1, tccl::EntityRef("X3"));
  schema.SetTermFor(x2, "42");
  schema.SetTermFor(x3, "43");
  ASSERT_TRUE(processor.Execute(EquationOptions{ x3, x2, Equation{ Equation::Mode::keepDel, std::string{} } }));
  EXPECT_EQ(schema.GetText(x1).term.Text().Raw(), tccl::EntityRef("X2"));
  EXPECT_EQ(schema.GetText(x2).term.Text().Raw(), "43");
}

TEST_F(UTEquationProcessor, ExecuteNewTerm) {
  const auto x3 = schema.Emplace(CstType::base);
  schema.SetTermFor(x1, tccl::EntityRef("X3"));
  schema.SetTermFor(x2, "42");
  schema.SetTermFor(x3, "43");
  ASSERT_TRUE(processor.Execute(EquationOptions{ x3, x2, Equation{ Equation::Mode::createNew, "44" } }));
  EXPECT_EQ(schema.GetText(x1).term.Text().Raw(), tccl::EntityRef("X2"));
  EXPECT_EQ(schema.GetText(x2).term.Text().Raw(), "44");
}

TEST_F(UTEquationProcessor, ExecuteMultiFreeNames) {
  const auto x3 = schema.Emplace(CstType::base);
  schema.SetTermFor(x2, "42");
  const auto x4 = schema.Emplace(CstType::base);
  schema.SetTermFor(x3, "43");

  EquationOptions eqs{};
  eqs.Insert(x1, x4);
  eqs.Insert(x3, d1);
  ASSERT_TRUE(processor.Execute(eqs));
  EXPECT_EQ(schema.RSLang().Count(CstType::base), 2U);
  EXPECT_EQ(schema.GetRS(schema.Emplace(CstType::base)).alias, "X1");
}

TEST_F(UTEquationProcessor, ExecuteMultiDependantEquations) {
  const auto d2 = schema.Emplace(CstType::term, R"(X2 \setminus X2)"_rs);
  EquationOptions eqs{};
  eqs.Insert(d2, d1);
  eqs.Insert(x1, x2);
  ASSERT_TRUE(processor.Execute(eqs));
  EXPECT_EQ(schema.RSLang().Count(CstType::base), 1U);
  EXPECT_EQ(schema.RSLang().Count(CstType::term), 1U);
}

TEST_F(UTEquationProcessor, ExecuteUnrelatedDuplicates) {
  const auto d2 = schema.InsertCopy(d1, schema.Core());
  ASSERT_TRUE(processor.Execute(EquationOptions{ x2, s1 }));

  const auto& translations = processor.Translation();
  EXPECT_EQ(std::ssize(translations), 2);
  EXPECT_EQ(translations(d2), d1);
  EXPECT_TRUE(schema.Contains(d1));
  EXPECT_FALSE(schema.Contains(d2));
}

TEST_F(UTEquationProcessor, ExecuteDuplicatesAfterEquations) {
  const auto s3 = schema.Emplace(CstType::structured, "X1");
  const auto s4 = schema.Emplace(CstType::structured, "X2");

  ASSERT_TRUE(processor.Execute(EquationOptions{ x1, x2 }));
  const auto& result = processor.Translation();
  EXPECT_EQ(std::ssize(result), 2);
  ASSERT_TRUE(result.ContainsKey(s4));
  EXPECT_EQ(result(s4), s3);
}

TEST_F(UTEquationProcessor, ExecuteEmptyIsNotDuplicates) {
  const auto x3 = schema.Emplace(CstType::base);
  const auto x4 = schema.Emplace(CstType::base);
  ASSERT_TRUE(processor.Execute(EquationOptions{ x3, x4 }));
  EXPECT_TRUE(schema.Contains(x1));
  EXPECT_TRUE(schema.Contains(x2));
  EXPECT_FALSE(schema.Contains(x3));
  EXPECT_TRUE(schema.Contains(x4));
}