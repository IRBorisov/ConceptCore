#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"

#include "ccl/semantic/RSForm.h"
#include "ccl/ops/RSAggregator.h"
#include "ccl/ops/RSOperations.h"
#include "RSLHelper.hpp"

class UTRSAggregator: public ::testing::Test {
protected:
  using RSForm = ccl::semantic::RSForm;
  using CstType = ccl::semantic::CstType;
  using OpRelativation = ccl::ops::OpRelativation;
  using RSAggregator = ccl::ops::RSAggregator;
  using EntityTranslation = ccl::EntityTranslation;

protected:
  RSForm schema{};
};

TEST_F(UTRSAggregator, SelfExtrapolate) {
  schema.Emplace(CstType::base);
  schema.Emplace(CstType::term);
  EXPECT_NO_THROW(RSAggregator(schema).Merge(schema, EntityTranslation{}));
}

TEST_F(UTRSAggregator, InheritUnchangedCst) {
  const auto x1 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);

  RSForm newSchema = schema;
  const auto identity = newSchema.Ops().Identity();
  ASSERT_EQ(RSAggregator(newSchema).Merge(schema, identity), identity);
  EXPECT_EQ(std::ssize(newSchema.Core()), 1);
}

TEST_F(UTRSAggregator, InheritChangedCst) {
  const auto x1 = schema.Emplace(CstType::base);
  RSForm newSchema = schema;

  schema.Mods().Track(x1);
  schema.Mods()(x1)->allowEdit = true;
  schema.SetExpressionFor(x1, { "test_old" });
  schema.SetTermFor(x1, "modifiedOld");

  newSchema.SetExpressionFor(x1, { "test_new" });
  newSchema.SetTermFor(x1, "newTerm");
  newSchema.Mods().Track(x1);
  newSchema.Mods()(x1)->allowEdit = true;

  RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity());
  EXPECT_TRUE(newSchema.Mods()(x1)->allowEdit);
  EXPECT_TRUE(newSchema.Mods()(x1)->term);
  EXPECT_EQ(newSchema.GetRS(x1).definition, "test_new");
  EXPECT_EQ(newSchema.GetText(x1).term.Nominal(), "modifiedOld");
}

TEST_F(UTRSAggregator, InheritAddedCst) {
  RSForm newSchema = schema;
  const auto x1 = schema.Emplace(CstType::base);

  RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity());
  EXPECT_TRUE(newSchema.Contains(x1));
}

TEST_F(UTRSAggregator, DontInheritOldTracked) {
  RSForm newSchema = schema;
  const auto x1 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);
  RSAggregator(newSchema).Merge(schema, EntityTranslation{});
  EXPECT_TRUE(std::empty(newSchema.Core()));
}

TEST_F(UTRSAggregator, DontInheritOldChangedTracked) {
  RSForm newSchema = schema;
  const auto x1 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);
  schema.Mods()(x1)->allowEdit = true;
  schema.SetTermFor(x1, "X1");
  RSAggregator(newSchema).Merge(schema, EntityTranslation{});
  EXPECT_TRUE(std::empty(newSchema.Core()));
}

TEST_F(UTRSAggregator, TranslationInvalidBoth) {
  RSForm newSchema = schema;
  EntityTranslation translation{};
  translation.Insert(42U, 42U);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, translation).has_value());
}

TEST_F(UTRSAggregator, TranslationInvalidLeft) {
  RSForm newSchema = schema;
  const auto x1 = schema.Emplace(CstType::base);

  EntityTranslation translation{};
  translation.Insert(x1, x1 + 1);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, translation).has_value());
}

TEST_F(UTRSAggregator, TranslationInvalidRight) {
  RSForm newSchema = schema;
  const auto x1 = newSchema.Emplace(CstType::base);

  EntityTranslation translation{};
  translation.Insert(x1 + 1, x1);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, translation).has_value());
}

TEST_F(UTRSAggregator, TranslationAxiomToTerm) {
  RSForm newSchema = schema;
  const auto a_1 = schema.Emplace(CstType::axiom);
  const auto x1 = newSchema.Emplace(CstType::base);

  EntityTranslation translation{};
  translation.Insert(a_1, x1);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, translation).has_value());
}

TEST_F(UTRSAggregator, TranslationInvalidTracking) {
  const auto x1 = schema.Emplace(CstType::base);
  RSForm newSchema = schema;
  schema.Mods().Track(x1);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity()).has_value());

  newSchema.Mods().Track(x1);
  schema.Mods().StopTracking(x1);
  EXPECT_FALSE(RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity()).has_value());
}

TEST_F(UTRSAggregator, UpdateExpr) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto x2 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);
  schema.Mods().Track(x2);

  RSForm newSchema = schema;
  newSchema.SetAliasFor(x1, "X3");

  const auto s1 = schema.Emplace(CstType::structured, "X1 X2");
  const auto trans = RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity());
  ASSERT_TRUE(trans.has_value());
  EXPECT_EQ(std::ssize(trans.value()), 3);
  EXPECT_EQ(std::ssize(newSchema.Core()), 3);
  EXPECT_EQ(newSchema.GetRS(trans.value()(s1)).definition, "X3 X2");
}

TEST_F(UTRSAggregator, UpdateExprDangling) {
  RSForm newSchema = schema;

  const auto x1 = schema.Emplace(CstType::base, "X2");
  const auto trans = RSAggregator(newSchema).Merge(schema, EntityTranslation{});
  ASSERT_TRUE(trans.has_value());
  EXPECT_EQ(newSchema.GetRS(trans.value()(x1)).definition, "X2_ERROR");
}

TEST_F(UTRSAggregator, UpdateExprSelfReference) {
  RSForm newSchema = schema;
  newSchema.Emplace(CstType::base);

  const auto x1 = schema.Emplace(CstType::base);
  schema.SetExpressionFor(x1, schema.GetRS(x1).alias);

  const auto trans = RSAggregator(newSchema).Merge(schema, EntityTranslation{});
  ASSERT_TRUE(trans.has_value());
  const auto& newCst = newSchema.GetRS(trans.value()(x1));
  EXPECT_EQ(newCst.alias, newCst.definition);
}

TEST_F(UTRSAggregator, UpdateTransferedText) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto d1 = schema.Emplace(CstType::term);
  RSForm newSchema = schema;

  schema.Mods().Track(x1);
  schema.Mods().Track(d1);
  schema.Mods()(x1)->allowEdit = true;
  schema.Mods()(d1)->allowEdit = true;
  schema.SetTermFor(x1, "Test1");
  schema.SetConventionFor(x1, "TestConvention");
  schema.SetTermFor(d1, "X1 " + tccl::EntityRef("X1"));

  newSchema.SetAliasFor(x1, "X3", false);
  newSchema.Mods().Track(x1);
  newSchema.Mods().Track(d1);

  RSAggregator(newSchema).Merge(schema, newSchema.Ops().Identity());
  EXPECT_EQ(newSchema.GetText(x1).term.Nominal(), "Test1");
  EXPECT_EQ(newSchema.GetRS(x1).convention, "TestConvention");
  EXPECT_EQ(newSchema.GetText(d1).term.Text().Raw(), "X1 " + tccl::EntityRef("X3"));
}

TEST_F(UTRSAggregator, TransferDanglingTextRef) {
  const auto x1 = schema.Emplace(CstType::base);
  RSForm newSchema = schema;

  schema.Mods().Track(x1);
  schema.Mods()(x1)->allowEdit = true;
  schema.SetDefinitionFor(x1, "X2 " + tccl::EntityRef("X2"));

  newSchema.Emplace(CstType::base);
  newSchema.Mods().Track(x1);

  RSAggregator(newSchema).Merge(schema, schema.Ops().Identity());
  EXPECT_EQ(newSchema.GetText(x1).definition.Raw(), "X2 " + tccl::EntityRef("X2_ERROR"));
}

TEST_F(UTRSAggregator, UpdateTypifications) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto x2 = schema.Emplace(CstType::base);
  RSForm newSchema = schema;

  schema.Mods().Track(x1);
  schema.Mods().Track(x2);
  const auto d2 = schema.Emplace(CstType::term, R"(X2 \setminus X2)"_rs);
  const auto d3 = schema.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);

  newSchema.Mods().Track(x1);
  newSchema.Erase(x2);
  const auto translation = newSchema.Ops().Identity();
  const auto d1 = newSchema.Emplace(CstType::term, R"(D3 \setminus X1)"_rs);

  const auto old2New = RSAggregator(newSchema).Merge(schema, translation);
  ASSERT_TRUE(old2New.has_value());
  ASSERT_EQ(std::ssize(old2New.value()), 3);
  ASSERT_TRUE(newSchema.GetParse(d1).exprType.has_value());
  EXPECT_FALSE(newSchema.GetParse(old2New.value()(d2)).exprType.has_value());
  ASSERT_TRUE(newSchema.GetParse(old2New.value()(d3)).exprType.has_value());
  EXPECT_EQ(newSchema.GetParse(x1).exprType.value(), newSchema.GetParse(d1).exprType.value());
  EXPECT_EQ(newSchema.GetParse(x1).exprType.value(), newSchema.GetParse(old2New.value()(d3)).exprType.value());
}

TEST_F(UTRSAggregator, NewTracking) {
  RSForm newSchema = schema;
  const auto x1 = newSchema.Emplace(CstType::base);
  newSchema.Mods().Track(x1);
  newSchema.Mods()(x1)->allowEdit = true;

  RSAggregator(newSchema).Merge(schema, EntityTranslation{});
  EXPECT_TRUE(newSchema.Contains(x1));
  EXPECT_TRUE(newSchema.Mods().IsTracking(x1));
}

TEST_F(UTRSAggregator, InheritEditEnable) {
  const auto x1 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);
  schema.Mods()(x1)->allowEdit = true;

  RSForm newSchema = schema;
  newSchema.Mods()(x1)->allowEdit = false;

  RSAggregator(newSchema).Merge(schema, schema.Ops().Identity());
  EXPECT_TRUE(newSchema.Contains(x1));
  EXPECT_TRUE(newSchema.Mods().IsTracking(x1));
  EXPECT_TRUE(newSchema.Mods()(x1)->allowEdit);
}

TEST_F(UTRSAggregator, InheritConvention) {
  const auto x1 = schema.Emplace(CstType::base);
  schema.SetConventionFor(x1, "test");
  schema.Mods().Track(x1);
  schema.Mods()(x1)->allowEdit = true;
  
  RSForm newSchema = schema;
  newSchema.SetConventionFor(x1, "42");
  RSAggregator(newSchema).Merge(schema, schema.Ops().Identity());
  EXPECT_EQ(newSchema.GetRS(x1).convention, "42");
}