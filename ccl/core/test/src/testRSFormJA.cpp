#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/api/RSFormJA.h"
#include "ccl/rslang/Literals.h"
#include "ccl/tools/JSON.h"

using JSON = nlohmann::ordered_json;
using ccl::rslang::operator""_rs;

class UTRSFormJA: public ::testing::Test {
protected:
  using RSFormJA = ccl::api::RSFormJA;
  using RSForm = ccl::semantic::RSForm;
  using ConceptRecord = ccl::semantic::ConceptRecord;
  using CstType = ccl::semantic::CstType;
  using Syntax = ccl::rslang::Syntax;

  RSForm schema{};

  RSForm SetupSchema();
};

ccl::semantic::RSForm UTRSFormJA::SetupSchema() {
  RSForm result{};
  result.Emplace(CstType::base); // X1
  result.Emplace(CstType::base); // X2
  result.Emplace(CstType::structured, "B(X1*X1)"_rs); // S1
  result.Emplace(CstType::structured, "B(X1*X2)"_rs); // S2
  return result;
}

TEST_F(UTRSFormJA, ReloadFull) {
  schema.title = "full";
  schema.alias = "short";
  schema.comment = "comment";

  ConceptRecord cst{};
  cst.uid = 42;
  cst.type = CstType::term;
  cst.alias = "D42";

  schema.Load(ConceptRecord{ cst });

  auto wrapper = RSFormJA::FromData(RSForm{ schema });
  const auto json = wrapper.ToJSON();
  auto reloaded = RSFormJA::FromJSON(json);

  const auto& initial = schema;
  const auto& test = reloaded.data();

  ASSERT_EQ(std::size(test.List()), std::size(initial.List()));
  EXPECT_EQ(test.title, initial.title);
  EXPECT_EQ(test.alias, initial.alias);
  EXPECT_EQ(test.comment, initial.comment);
  EXPECT_EQ(test.GetRS(cst.uid).type, cst.type);
  EXPECT_EQ(test.GetRS(cst.uid).alias, cst.alias);
}

TEST_F(UTRSFormJA, ReloadMinimal) {
  schema.title = "full";
  schema.alias = "short";
  schema.comment = "comment";

  ConceptRecord cst{};
  cst.uid = 42;
  cst.type = CstType::term;
  cst.alias = "D42";

  schema.Load(ConceptRecord{ cst });

  auto wrapper = RSFormJA::FromData(RSForm{ schema });
  const auto json = wrapper.ToMinimalJSON();
  auto reloaded = RSFormJA::FromJSON(json);

  const auto& initial = schema;
  const auto& test = reloaded.data();

  ASSERT_EQ(std::size(test.List()), std::size(initial.List()));
  EXPECT_EQ(test.title, initial.title);
  EXPECT_EQ(test.alias, initial.alias);
  EXPECT_EQ(test.comment, initial.comment);
  EXPECT_EQ(test.GetRS(cst.uid).type, cst.type);
  EXPECT_EQ(test.GetRS(cst.uid).alias, cst.alias);
}

TEST_F(UTRSFormJA, ParseExpression) {
  {
    auto reponse = JSON::parse(ccl::api::ParseExpression(""));
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), false);
    EXPECT_EQ(reponse.at("errors").size(), 1U);
  }
  {
    auto reponse = JSON::parse(ccl::api::ParseExpression(R"(X1\X2)", Syntax::MATH));
    EXPECT_EQ(reponse.at("syntax"), "math");
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), true);
    EXPECT_EQ(reponse.at("astText"), R"([\[X1][X2]])");
  }
}

TEST_F(UTRSFormJA, CheckExpression) {
  auto wrapper = RSFormJA::FromData(SetupSchema());
  {
    auto reponse = JSON::parse(wrapper.CheckExpression(""));
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), false);
    EXPECT_EQ(reponse.at("errors").size(), 1U);
  }
  {
    auto reponse = JSON::parse(wrapper.CheckExpression("X1=X2", Syntax::MATH));
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), false);
    EXPECT_EQ(reponse.at("syntax"), "math");
    EXPECT_EQ(reponse.at("astText"), "[=[X1][X2]]");
    EXPECT_EQ(reponse.at("errors").size(), 1U);
    EXPECT_NE(reponse.at("errors")[0].at("position").get<int32_t>(), 0);
    EXPECT_EQ(reponse.at("args").size(), 0U);
  }
  {
    auto reponse = JSON::parse(wrapper.CheckExpression("X1=X1", Syntax::MATH));
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), true);
    EXPECT_EQ(reponse.at("syntax"), "math");
    EXPECT_EQ(reponse.at("typification"), "LOGIC");
    EXPECT_EQ(reponse.at("astText"), "[=[X1][X1]]");
    EXPECT_EQ(reponse.at("errors").size(), 0U);
  }
  {
    auto reponse = JSON::parse(wrapper.CheckExpression(R"([a \in X1] a \eq a)", Syntax::ASCII));
    EXPECT_EQ(reponse.at("parseResult").get<bool>(), true);
    EXPECT_EQ(reponse.at("syntax"), "ascii");
    EXPECT_EQ(reponse.at("args").size(), 1U);
    EXPECT_EQ(reponse.at("args")[0].at("alias"), "a");
    EXPECT_EQ(reponse.at("args")[0].at("typification"), "X1");
  }
}

TEST_F(UTRSFormJA, CheckConstituenta) {
  auto wrapper = RSFormJA::FromData(SetupSchema());
  auto reponse = JSON::parse(wrapper.CheckConstituenta("A100", "X1=X1", "axiom"));
  EXPECT_EQ(reponse.at("parseResult").get<bool>(), true);
  EXPECT_EQ(reponse.at("prefixLen"), 7);
  EXPECT_EQ(reponse.at("syntax"), "math");
  EXPECT_EQ(reponse.at("typification"), "LOGIC");
  EXPECT_EQ(reponse.at("errors").size(), 0U);
  
}