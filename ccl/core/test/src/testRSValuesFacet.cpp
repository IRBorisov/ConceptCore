#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSModel.h"
#include "RSLHelper.hpp"

class UTRSValues : public ::testing::Test {
protected:
  using RSModel = ccl::semantic::RSModel;
  using CstType = ccl::semantic::CstType;
  using TextInterpretation = ccl::semantic::TextInterpretation;
  using Factory = ccl::object::Factory;
  using StructuredData = ccl::object::StructuredData;
  using rsValuesFacet = ccl::semantic::rsValuesFacet;
  using EvalStatus = ccl::semantic::EvalStatus;

protected:
  RSModel model{};
  rsValuesFacet& values{ model.Values() };
  ccl::EntityUID x1, s1, s2, s3;

  void SetupStructure();
  bool TrySetData(ccl::EntityUID uid, StructuredData data);
};

void UTRSValues::SetupStructure() {
  x1 = model.Emplace(CstType::base);
  s1 = model.Emplace(CstType::structured, "B(X1)"_rs);
  s2 = model.Emplace(CstType::structured, "B(X1*X1)"_rs);
  s3 = model.Emplace(CstType::structured, "X1*X1"_rs);
  values.SetBasicText(x1, TextInterpretation{ { "1", "2", "3" } });
}

bool UTRSValues::TrySetData(const ccl::EntityUID uid, const StructuredData data) {
  if (!values.SetStructureData(uid, data)) {
    return false;
  } else {
    EXPECT_FALSE(values.StatementFor(uid).has_value());
    EXPECT_TRUE(values.TextFor(uid) == nullptr);
    return data == values.SDataFor(uid);
  }
}

TEST_F(UTRSValues, InvalidIDs) {
  ccl::EntityUID invalid{ 42U };
  EXPECT_FALSE(values.SDataFor(invalid).has_value());
  EXPECT_FALSE(values.StatementFor(invalid).has_value());
  EXPECT_TRUE(values.TextFor(invalid) == nullptr);
  EXPECT_FALSE(values.AddBasicElement(invalid, "42").has_value());

  EXPECT_FALSE(values.SetStructureData(invalid, Factory::Val(1)));
  EXPECT_FALSE(values.SDataFor(invalid).has_value());
  EXPECT_FALSE(values.StatementFor(invalid).has_value());

  EXPECT_FALSE(values.SetBasicText(invalid, TextInterpretation{}));
  EXPECT_TRUE(values.TextFor(invalid) == nullptr);
}

TEST_F(UTRSValues, BasicCstDefault) {
  x1 = model.Emplace(CstType::base);
  ASSERT_TRUE(values.SDataFor(x1).has_value());
  EXPECT_TRUE(values.SDataFor(x1)->B().IsEmpty());

  ASSERT_TRUE(values.TextFor(x1) != nullptr);
  EXPECT_TRUE(std::empty(*values.TextFor(x1)));
}

TEST_F(UTRSValues, StructureCstDefaultIncorrect) {
  s1 = model.Emplace(CstType::structured);
  EXPECT_FALSE(values.SDataFor(s1).has_value());
  EXPECT_FALSE(values.StatementFor(s1).has_value());
  EXPECT_TRUE(values.TextFor(s1) == nullptr);
}

TEST_F(UTRSValues, StructureCstDefaultCorrect) {
  SetupStructure();
  ASSERT_TRUE(values.SDataFor(s1).has_value());
  EXPECT_TRUE(values.SDataFor(s1)->B().IsEmpty());
  EXPECT_FALSE(values.StatementFor(s1).has_value());
  EXPECT_TRUE(values.TextFor(s1) == nullptr);
}

TEST_F(UTRSValues, TermCstDefault) {
  const auto d1 = model.Emplace(CstType::term);
  EXPECT_FALSE(values.SDataFor(d1).has_value());
  EXPECT_FALSE(values.StatementFor(d1).has_value());
  EXPECT_TRUE(values.TextFor(d1) == nullptr);
}

TEST_F(UTRSValues, AxiomCstDefault) {
  const auto a1 = model.Emplace(CstType::term);
  EXPECT_FALSE(values.SDataFor(a1).has_value());
  EXPECT_FALSE(values.StatementFor(a1).has_value());
  EXPECT_TRUE(values.TextFor(a1) == nullptr);
}

TEST_F(UTRSValues, AddBasicElement) {
  x1 = model.Emplace(CstType::base);
  const auto c_1 = model.Emplace(CstType::constant);
  const auto tid = values.AddBasicElement(x1, "42");
  ASSERT_TRUE(tid.has_value());
  EXPECT_EQ(values.TextFor(x1)->size(), 1U);
  EXPECT_EQ(values.TextFor(x1)->GetInterpretantFor(tid.value()), std::string{ "42" });
  EXPECT_TRUE(values.AddBasicElement(c_1, "42").has_value());
  EXPECT_FALSE(values.StatementFor(x1).has_value());
}

TEST_F(UTRSValues, BasicTypeRequirement) {
  static const auto data = TextInterpretation({ "a1", "a2", "a3" });

  SetupStructure();
  const auto d1 = model.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  const auto a1 = model.Emplace(CstType::axiom);

  EXPECT_FALSE(values.AddBasicElement(s1, "42").has_value());
  EXPECT_FALSE(values.AddBasicElement(d1, "42").has_value());
  EXPECT_FALSE(values.AddBasicElement(a1, "42").has_value());
  EXPECT_FALSE(values.SetBasicText(s1, data));
  EXPECT_FALSE(values.SetBasicText(d1, data));
  EXPECT_FALSE(values.SetBasicText(a1, data));
}

TEST_F(UTRSValues, SetBasic) {
  static const auto data = TextInterpretation{ {"1", "2", "3"} };

  x1 = model.Emplace(CstType::base);
  const auto c_1 = model.Emplace(CstType::constant);

  ASSERT_TRUE(values.SetBasicText(x1, data));
  EXPECT_FALSE(values.SetBasicText(x1, data));
  ASSERT_TRUE(values.SetBasicText(c_1, data));
  EXPECT_EQ(*values.TextFor(x1), data);
  EXPECT_EQ(*values.TextFor(c_1), data);
}

TEST_F(UTRSValues, SetBasicIncorrectExpression) {
  x1 = model.Emplace(CstType::base, "some invalid X1");
  EXPECT_EQ(model.GetParse(x1).status, ccl::semantic::ParsingStatus::INCORRECT);
  EXPECT_TRUE(values.SetBasicText(x1, TextInterpretation{ {"1", "2", "3"} }));
}

TEST_F(UTRSValues, SetStructure) {
  SetupStructure();
  EXPECT_TRUE(TrySetData(s1, Factory::SetV({ 1 })));
  EXPECT_TRUE(TrySetData(s1, Factory::SetV({ 1, 2 })));
  EXPECT_TRUE(TrySetData(s1, Factory::EmptySet()));

  EXPECT_TRUE(TrySetData(s2, Factory::Set({ Factory::TupleV({ 1, 1 }) })));
  EXPECT_TRUE(TrySetData(s2, Factory::Set({ Factory::TupleV({ 1, 2 }) })));
  EXPECT_TRUE(TrySetData(s2, Factory::EmptySet()));

  EXPECT_TRUE(TrySetData(s3, Factory::TupleV({1, 2 })));
  EXPECT_EQ(model.Calculations()(s3), EvalStatus::HAS_DATA);
}

TEST_F(UTRSValues, ResetDataFor) {
  SetupStructure();
  ASSERT_TRUE(TrySetData(s1, Factory::SetV({ 1 })));
  values.ResetDataFor(s1);
  EXPECT_EQ(values.SDataFor(s1).value(), Factory::EmptySet());

  ASSERT_TRUE(TrySetData(s1, Factory::SetV({ 1 })));
  values.ResetDataFor(x1);
  EXPECT_EQ(values.SDataFor(x1).value(), Factory::EmptySet());
  EXPECT_EQ(values.SDataFor(s1).value(), Factory::EmptySet());
}

TEST_F(UTRSValues, SetStructureErrors) {
  SetupStructure();
  EXPECT_FALSE(TrySetData(s3, Factory::EmptySet()));
  EXPECT_FALSE(TrySetData(s3, Factory::Val(1)));

  EXPECT_TRUE(TrySetData(s1, Factory::Set({ Factory::Val(1) })));
  EXPECT_FALSE(TrySetData(s1, Factory::Set({ Factory::Val(1) })));

  EXPECT_FALSE(TrySetData(s1, Factory::Set({ Factory::Val(4) })));
  EXPECT_FALSE(TrySetData(s2, Factory::Set({ Factory::TupleV({ 1, 4 }) })));
  EXPECT_FALSE(TrySetData(s2, Factory::Set({ Factory::TupleV({ 4, 1 }) })));

  model.SetExpressionFor(s1, "BB(X1)"_rs);
  EXPECT_FALSE(TrySetData(s1, Factory::SetV({ 1 })));

  model.SetExpressionFor(s1, "invalid");
  EXPECT_FALSE(TrySetData(s1, Factory::SetV({ 1 })));
}

TEST_F(UTRSValues, ResetDependantProneStructure) {
  SetupStructure();
  values.SetStructureData(s1, values.SDataFor(x1).value());
  values.SetBasicText(x1, TextInterpretation{ { "41" } });
  EXPECT_EQ(values.SDataFor(s1).value(), values.SDataFor(x1).value());
}

TEST_F(UTRSValues, ResetDependantProneIncorrect) {
  SetupStructure();
  model.Emplace(CstType::structured, "invalid X1");
  ASSERT_NO_THROW(values.SetBasicText(x1, TextInterpretation{ { "41" } }););
}

TEST_F(UTRSValues, LoadData) {
  x1 = model.Emplace(CstType::base);

  const auto basicData = TextInterpretation{ {"1", "2", "3"} };
  values.LoadData(x1, basicData);
  EXPECT_EQ(*values.TextFor(x1), basicData);

  s1 = model.Emplace(CstType::structured, "B(X1)"_rs);
  const auto structData = Factory::SetV({ 1 });
  values.LoadData(s1, structData);
  EXPECT_EQ(*values.SDataFor(s1), structData);
  
  const auto a1 = model.Emplace(CstType::structured, "1=1");
  values.LoadData(a1, false);
  EXPECT_FALSE(values.StatementFor(a1).value());
}