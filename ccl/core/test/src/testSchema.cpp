#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/Schema.h"
#include "ccl/rslang/Auditor.h"
#include "RSLHelper.hpp"
#include "RandomGenerator.hpp"

class UTSchema : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using Schema = ccl::semantic::Schema;
  using RSConcept = ccl::semantic::RSConcept;
  using CstType = ccl::semantic::CstType;
  using CstTypeIter = ccl::semantic::CstTypeIter;
  using ParsingStatus = ccl::semantic::ParsingStatus;
  using ParsingInfo = ccl::semantic::ParsingInfo;
  using ExpressionType = ccl::rslang::ExpressionType;
  using Typification = ccl::rslang::Typification;
  using LogicT = ccl::rslang::LogicT;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using ValueClass = ccl::rslang::ValueClass;
  using Syntax = ccl::rslang::Syntax;

protected:
  Schema core{};

  static constexpr EntityUID x1{ 42 };
  static constexpr EntityUID x2{ 43 };
  static constexpr EntityUID s1{ 44 };
  static constexpr EntityUID d1{ 45 };
  static constexpr EntityUID d2{ 46 };
  static constexpr EntityUID a1{ 47 };
  static constexpr EntityUID a2{ 48 };
  static constexpr EntityUID f1{ 49 };
  static constexpr EntityUID p1{ 50 };
  static constexpr EntityUID invalid{ 13 };

  void BasicSetup();
};

void UTSchema::BasicSetup() {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(x2, "X2", CstType::base);
  core.Emplace(d1, "D1", CstType::term, R"(X1 \union X1)"_rs);
  core.Emplace(a1, "A1", CstType::axiom, R"(X1=X1)"_rs);
  core.Emplace(a2, "A2", CstType::axiom, R"(X1=X2)"_rs);
  core.Emplace(f1, "F1", CstType::function, "[a \xE2\x88\x88 X1] {a}");
}

TEST_F(UTSchema, ParsingInfoReset) {
  ParsingInfo info{};
  info.status = ParsingStatus::VERIFIED;
  info.valueClass = ValueClass::props;
  info.exprType = LogicT{};
  info.Reset();

  ParsingInfo expected{};
  EXPECT_EQ(info.status, expected.status);
  EXPECT_EQ(info.valueClass, expected.valueClass);
  EXPECT_EQ(info.exprType, expected.exprType);
}

TEST_F(UTSchema, ParsingInfoAccess) {
  ParsingInfo info{};
  EXPECT_FALSE(info.exprType.has_value());
  EXPECT_TRUE(info.TypePtr() == nullptr);
  EXPECT_TRUE(info.Typification() == nullptr);

  info.exprType = LogicT{};
  ASSERT_TRUE(info.exprType.has_value());
  EXPECT_TRUE(std::holds_alternative<LogicT>(*info.TypePtr()));
  EXPECT_TRUE(info.Typification() == nullptr);

  info.exprType = Typification{"X1"};
  ASSERT_TRUE(info.exprType.has_value());
  ASSERT_TRUE(std::holds_alternative<Typification>(*info.TypePtr()));
  EXPECT_EQ(info.Typification()->ToString(), Typification{ "X1" }.ToString());
}

TEST_F(UTSchema, Construction) {
  {
    EXPECT_EQ(std::size(core), 0U);
    EXPECT_FALSE(core.Contains(x1));
    EXPECT_FALSE(core.Contains(a1));
  }

  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  {
    Schema copy{ core };
    ASSERT_EQ(std::size(copy), 2U);
    ASSERT_TRUE(copy.Contains(x1));
    EXPECT_EQ(copy.At(x1), core.At(x1));
    ASSERT_TRUE(copy.Contains(a1));
    EXPECT_EQ(copy.At(a1), core.At(a1));
    EXPECT_TRUE(copy.Graph().ConnectionExists(x1, a1));
  }
  {
    Schema copyCtor{};
    copyCtor = core;
    ASSERT_EQ(std::size(copyCtor), 2U);
    ASSERT_TRUE(copyCtor.Contains(x1));
    EXPECT_EQ(copyCtor.At(x1), core.At(x1));
    ASSERT_TRUE(copyCtor.Contains(a1));
    EXPECT_EQ(copyCtor.At(a1), core.At(a1));
    EXPECT_TRUE(copyCtor.Graph().ConnectionExists(x1, a1));
  }
  {
    Schema temp{ core };
    Schema movedCopy{ std::move(temp) };
    ASSERT_EQ(std::size(movedCopy), 2U);
    ASSERT_TRUE(movedCopy.Contains(x1));
    EXPECT_EQ(movedCopy.At(x1), core.At(x1));
    ASSERT_TRUE(movedCopy.Contains(a1));
    EXPECT_EQ(movedCopy.At(a1), core.At(a1));
    EXPECT_TRUE(movedCopy.Graph().ConnectionExists(x1, a1));
  }
  {
    Schema temp{ core };
    Schema moveAss{};
    moveAss = std::move(temp);
    ASSERT_EQ(std::size(moveAss), 2U);
    ASSERT_TRUE(moveAss.Contains(x1));
    EXPECT_EQ(moveAss.At(x1), core.At(x1));
    ASSERT_TRUE(moveAss.Contains(a1));
    EXPECT_EQ(moveAss.At(a1), core.At(a1));
    EXPECT_TRUE(moveAss.Graph().ConnectionExists(x1, a1));
  }
}

TEST_F(UTSchema, STLapi) {
  {
    EXPECT_TRUE(std::empty(core));
    EXPECT_EQ(std::size(core), 0U);
    EXPECT_EQ(std::begin(core), std::end(core));
  }

  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(x2, "X2", CstType::base);
  {
    ASSERT_EQ(std::size(core), 2U);
    EXPECT_FALSE(std::empty(core));
    EXPECT_NE(std::begin(core), std::end(core));
  }
  {
    std::unordered_map<EntityUID, std::string> expected{};
    expected.emplace(x1, "X1");
    expected.emplace(x2, "X2");

    std::unordered_map<EntityUID, std::string>  result{};
    for (const auto& cst : core) {
      result.emplace(cst.uid, cst.alias);
    }
    EXPECT_EQ(result, expected);
  }
}

TEST_F(UTSchema, Count) {
  RNG_Helper generator{};
  auto size_test = 0U;
  for (const auto aType : CstTypeIter{}) {
    const auto count = generator() % 10;
    size_test += count;
    for (auto i = size_test; i > size_test - count; i--) {
      core.Emplace(EntityUID{ i }, "X1", aType);
    }
    EXPECT_EQ(static_cast<uint32_t>(core.Count(aType)), count) << static_cast<uint32_t>(aType);
  }
  EXPECT_EQ(std::size(core), size_test);
}

TEST_F(UTSchema, Contains) {
  EXPECT_FALSE(core.Contains(x1));

  core.Emplace(x1, "X1", CstType::base);
  EXPECT_TRUE(core.Contains(x1));
  EXPECT_FALSE(core.Contains(invalid));
}

TEST_F(UTSchema, AccessCst) {
  core.Emplace(x1, "X1", CstType::base);
  ASSERT_TRUE(core.Contains(x1));
  EXPECT_EQ(core.At(x1).alias, "X1");
  EXPECT_EQ(core.At(x1).type, CstType::base);
}

TEST_F(UTSchema, AcccessInfo) {
  BasicSetup();
  {
    EXPECT_EQ(core.InfoFor(x1).status, ParsingStatus::VERIFIED);
    EXPECT_EQ(core.InfoFor(d1).status, ParsingStatus::VERIFIED);
    EXPECT_EQ(core.InfoFor(f1).status, ParsingStatus::VERIFIED);
    EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  }
  {
    EXPECT_EQ(core.InfoFor(x1).valueClass, ValueClass::value);
    EXPECT_EQ(core.InfoFor(a2).valueClass, ValueClass::invalid);
  }
  {
    EXPECT_NE(core.InfoFor(d1).ast, nullptr);
    EXPECT_EQ(core.InfoFor(a2).ast, nullptr);
  }
  {
    EXPECT_EQ(std::get<Typification>(core.InfoFor(x1).exprType.value()), Typification("X1").ApplyBool());
    EXPECT_EQ(std::get<Typification>(core.InfoFor(d1).exprType.value()), Typification("X1").ApplyBool());
    EXPECT_EQ(std::get<LogicT>(core.InfoFor(a1).exprType.value()), LogicT{});
    EXPECT_FALSE(core.InfoFor(a2).exprType.has_value());
  }
  {
    EXPECT_FALSE(core.InfoFor(x1).arguments.has_value());
    EXPECT_FALSE(core.InfoFor(a1).arguments.has_value());
    EXPECT_FALSE(core.InfoFor(a2).arguments.has_value());
    EXPECT_FALSE(core.InfoFor(d1).arguments.has_value());
    ASSERT_TRUE(core.InfoFor(f1).arguments.has_value());
    EXPECT_EQ(std::size(core.InfoFor(f1).arguments.value()), 1u);
  }
}

TEST_F(UTSchema, TypeContext) {
  BasicSetup();
  {
    EXPECT_EQ(core.TypeFor("invalid"), nullptr);
    EXPECT_EQ(core.TypeFor("A2"), nullptr);
    ASSERT_NE(core.TypeFor("X1"), nullptr);
    EXPECT_EQ(std::get<Typification>(*core.TypeFor("X1")), Typification("X1").ApplyBool());
    ASSERT_NE(core.TypeFor("D1"), nullptr);
    EXPECT_EQ(std::get<Typification>(*core.TypeFor("D1")), Typification("X1").ApplyBool());
    ASSERT_NE(core.TypeFor("A1"), nullptr);
    EXPECT_EQ(std::get<LogicT>(*core.TypeFor("A1")), LogicT{});
  }
  {
    EXPECT_EQ(core.FunctionArgsFor("invalid"), nullptr);
    EXPECT_EQ(core.FunctionArgsFor("A1"), nullptr);
    EXPECT_EQ(core.FunctionArgsFor("A2"), nullptr);
    EXPECT_EQ(core.FunctionArgsFor("X1"), nullptr);
    EXPECT_EQ(core.FunctionArgsFor("D1"), nullptr);
    ASSERT_NE(core.FunctionArgsFor("F1"), nullptr);
    EXPECT_EQ(std::size(*core.FunctionArgsFor("F1")), 1U);
  }
  {
    EXPECT_FALSE(core.TraitsFor(Typification("invalid")).has_value());
    EXPECT_FALSE(core.TraitsFor(Typification("X3")).has_value());
    EXPECT_TRUE(core.TraitsFor(Typification("X1")).has_value());
  }
}

TEST_F(UTSchema, FindAlias) {
  EXPECT_FALSE(core.FindAlias("X1").has_value());
  core.Emplace(x1, "X1", CstType::base);
  {
    EXPECT_FALSE(core.FindAlias("X2").has_value());
    ASSERT_TRUE(core.FindAlias("X1").has_value());
    EXPECT_EQ(core.FindAlias("X1").value(), x1);
  }
  core.Emplace(x2, "X1", CstType::base);
  {
    EXPECT_FALSE(core.FindAlias("X2").has_value());
    ASSERT_TRUE(core.FindAlias("X1").has_value());
    EXPECT_TRUE(core.FindAlias("X1").value() == x1 || core.FindAlias("X1").value() == x2);
  }
}

TEST_F(UTSchema, FindExpr) {
  using ccl::rslang::Token;
  using ccl::rslang::TokenID;
  EXPECT_FALSE(core.FindExpr("").has_value());

  core.Emplace(x1, "X1", CstType::base);
  {
    const auto found = core.FindExpr({});
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value(), x1);
  }
  {
    Schema test = core;
    static const std::string expr = R"(X1 \union X1)"_rs;
    static const std::string wrongExpr = R"(X1 \intersect X1)"_rs;
    test.Emplace(d1, "D1", CstType::term, expr);
    const auto found = test.FindExpr(expr);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value(), d1);
    EXPECT_FALSE(test.FindExpr(wrongExpr).has_value());
  }
  {
    Schema test = core;
    static const std::string expr{ R"(X1 \setminus X1)"_rs };
    static const std::string wrongExpr = R"(X1 \intersect X1)"_rs;
    static const std::string modifiedExpr{ R"( X1  \setminus X1)"_rs };

    test.Emplace(d1, "D1", CstType::term, expr);
    const auto found = test.FindExpr(expr);
    ASSERT_TRUE(found.has_value());
    EXPECT_EQ(found.value(), d1);
    EXPECT_FALSE(test.FindExpr(wrongExpr).has_value());
    EXPECT_TRUE(test.FindExpr(modifiedExpr).has_value());
  }
}

TEST_F(UTSchema, Emplace) {
  {
    Schema test{};
    EXPECT_TRUE(test.Emplace(x1, "X1", CstType::base));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, x1);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).type, CstType::base);
    EXPECT_EQ(test.At(x1).definition, "");
    EXPECT_EQ(test.At(x1).convention, "");
    EXPECT_EQ(test.InfoFor(x1).status, ParsingStatus::VERIFIED);
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base, "def");
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(x1, test.At(x1).uid);
    EXPECT_EQ("X1", test.At(x1).alias);
    EXPECT_EQ(CstType::base, test.At(x1).type);
    EXPECT_EQ("def", test.At(x1).definition);
    EXPECT_EQ("", test.At(x1).convention);
    EXPECT_EQ(ParsingStatus::INCORRECT, test.InfoFor(x1).status);
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base, "def", "convention");
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, x1);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).type, CstType::base);
    EXPECT_EQ(test.At(x1).definition, "def");
    EXPECT_EQ(test.At(x1).convention, "convention");
    EXPECT_EQ(test.InfoFor(x1).status, ParsingStatus::INCORRECT);
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base);
    EXPECT_FALSE(test.Emplace(x1, "X2", CstType::base));
    EXPECT_FALSE(test.Emplace(x1, "X1", CstType::base));
    EXPECT_FALSE(test.Emplace(x1, "C1", CstType::constant));
    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).type, CstType::base);
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base);
    EXPECT_TRUE(test.Emplace(x2, "X1", CstType::base));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).type, CstType::base);
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).alias, "X1");
    EXPECT_EQ(test.At(x2).type, CstType::base);
  }
}

TEST_F(UTSchema, InsertCopy) {
  const RSConcept cst{ x1, "X1", CstType::base, "def", "con" };
  {
    Schema test{};
    EXPECT_TRUE(test.InsertCopy(cst));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, cst.uid);
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).type, cst.type);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    EXPECT_EQ(test.At(x1).convention, cst.convention);
    EXPECT_EQ(test.InfoFor(x1).status, ParsingStatus::INCORRECT);
  }
  {
    Schema test{};
    test.InsertCopy(cst);
    RSConcept modified = cst;
    modified.definition = "another def";
    EXPECT_FALSE(test.InsertCopy(modified));
    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Schema test{};
    test.InsertCopy(cst);
    RSConcept modified = cst;
    modified.uid = x2;
    modified.definition = "another definition";
    EXPECT_TRUE(test.InsertCopy(modified));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).definition, modified.definition);
  }
}

TEST_F(UTSchema, Insert) {
  const RSConcept cst{ x1, "X1", CstType::base, "def", "con" };
  {
    Schema test{};
    EXPECT_TRUE(test.Insert(RSConcept{ cst }));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, cst.uid);
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).type, cst.type);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    EXPECT_EQ(test.At(x1).convention, cst.convention);
    EXPECT_EQ(test.InfoFor(x1).status, ParsingStatus::INCORRECT);
  }
  {
    Schema test{};
    test.Insert(RSConcept{ cst });
    RSConcept modified = cst;
    modified.definition = "another def";
    EXPECT_FALSE(test.Insert(std::move(modified)));
    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Schema test{};
    test.Insert(RSConcept{ cst });
    RSConcept modified = cst;
    modified.uid = x2;
    modified.definition = "another definition";
    EXPECT_TRUE(test.Insert(std::move(modified)));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).definition, "another definition");
  }
}

TEST_F(UTSchema, Load) {
  const RSConcept cst{ x1, "X1", CstType::base, "def", "con" };
  {
    Schema test{};
    test.Load(RSConcept{ cst });
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).type, cst.type);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    EXPECT_EQ(test.At(x1).convention, cst.convention);
    EXPECT_EQ(test.InfoFor(x1).status, ParsingStatus::UNKNOWN);
  }
  {
    Schema test{};
    test.Insert(RSConcept{ cst });

    RSConcept modified = cst;
    modified.definition = "another definition";
    test.Load(std::move(modified));

    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, "another definition");
  }
}

TEST_F(UTSchema, Erase) {
  ASSERT_NO_THROW(core.Erase(invalid));
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(x2, "X2", CstType::base);
  EXPECT_EQ(std::size(core), 2U);
  EXPECT_TRUE(core.Contains(x1));
  EXPECT_TRUE(core.Contains(x2));

  core.Erase(x1);
  EXPECT_EQ(std::size(core), 1U);
  EXPECT_FALSE(core.Contains(x1));
  EXPECT_TRUE(core.Contains(x2));
}

TEST_F(UTSchema, SetName) {
  {
    EXPECT_FALSE(core.SetAliasFor(invalid, "X1"));
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base);
    EXPECT_FALSE(test.SetAliasFor(x1, "X1"));
    EXPECT_TRUE(test.SetAliasFor(x1, "X2"));
    EXPECT_EQ(test.At(x1).alias, "X2");
  }
  {
    Schema test{};
    test.Emplace(x1, "X1", CstType::base);
    test.Emplace(x2, "X2", CstType::base);
    EXPECT_TRUE(test.SetAliasFor(x1, "X2"));
    EXPECT_EQ(test.At(x1).alias, "X2");
    EXPECT_EQ(test.At(x2).alias, "X2");
  }
}

TEST_F(UTSchema, SetNameSubstitute) {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_EQ(*core.InfoFor(d1).Typification(), "B(X1)"_t);
  {
    Schema test{ core };
    EXPECT_TRUE(test.SetAliasFor(x1, "X3", true));
    EXPECT_EQ(test.At(x1).alias, "X3");
    EXPECT_EQ(test.At(a1).definition, "X3=X3");
    EXPECT_EQ(test.At(d1).definition, "debool({X3})");
    EXPECT_TRUE(test.Graph().ConnectionExists(x1, a1));
    EXPECT_EQ(*test.InfoFor(d1).Typification(), "B(X3)"_t);
  }
  {
    Schema test{ core };
    EXPECT_TRUE(test.SetAliasFor(x1, "X3", false));
    EXPECT_EQ(test.At(x1).alias, "X3");
    EXPECT_EQ(test.At(a1).definition, "X1=X1");
    EXPECT_FALSE(test.Graph().ConnectionExists(x1, a1));
    EXPECT_EQ(test.InfoFor(d1).Typification(), nullptr);
  }
}

TEST_F(UTSchema, SetDefinition) {
  EXPECT_FALSE(core.SetDefinitionFor(invalid, ""));
  EXPECT_FALSE(core.SetDefinitionFor(invalid, "X1"));

  core.Emplace(x1, "X1", CstType::base);
  EXPECT_FALSE(core.SetDefinitionFor(x1, ""));
  EXPECT_TRUE(core.SetDefinitionFor(x1, "X1"));
  EXPECT_EQ(core.At(x1).definition, "X1");
  EXPECT_TRUE(core.SetDefinitionFor(x1, ""));
  EXPECT_EQ(core.At(x1).definition, "");
}

TEST_F(UTSchema, SetDefinitionMinorChange) {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(d1, "D1", CstType::term, R"(X1 \setminus X1)"_rs + "  ");
  EXPECT_FALSE(core.SetDefinitionFor(d1, R"(X1 \setminus X1)"_rs));
  EXPECT_EQ(core.At(d1).definition, R"(X1 \setminus X1)"_rs);
}

TEST_F(UTSchema, SetConvention) {
  EXPECT_FALSE(core.SetConventionFor(invalid, "con"));
  EXPECT_FALSE(core.SetConventionFor(invalid, ""));

  core.Emplace(x1, "X1", CstType::base);
  EXPECT_FALSE(core.SetConventionFor(x1, ""));
  EXPECT_TRUE(core.SetConventionFor(x1, "con"));
  EXPECT_EQ(core.At(x1).convention, "con");
  EXPECT_TRUE(core.SetConventionFor(x1, ""));
  EXPECT_EQ(core.At(x1).convention, "");
}

TEST_F(UTSchema, SetType) {
  {
    EXPECT_FALSE(core.SetTypeFor(invalid, CstType::base));
  }
  core.Emplace(x1, "X1", CstType::base);
  {
    EXPECT_FALSE(core.SetTypeFor(x1, CstType::base));
  }

  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(*core.InfoFor(d1).Typification(), "B(X1)"_t);
  {
    Schema test{ core };
    EXPECT_TRUE(test.SetTypeFor(x1, CstType::term));
    EXPECT_EQ(test.At(x1).type, CstType::term);

    EXPECT_TRUE(test.Graph().ConnectionExists(x1, d1));
    EXPECT_EQ(test.InfoFor(d1).Typification(), nullptr);
  }

  core.Emplace(a1, "A1", CstType::term, "1=1");
  EXPECT_EQ(ParsingStatus::INCORRECT, core.InfoFor(a1).status);
  {
    Schema test{ core };
    EXPECT_TRUE(test.SetTypeFor(a1, CstType::axiom));
    EXPECT_EQ(test.At(a1).type, CstType::axiom);
    EXPECT_EQ(test.InfoFor(a1).status, ParsingStatus::VERIFIED);
  }
}

TEST_F(UTSchema, UpdateOnInsert) {
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  core.Emplace(a2, "A2", CstType::axiom, "X1=X1 & X2=X2");
  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(d1).Typification(), nullptr);

  core.Emplace(x1, "X1", CstType::base);
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  ASSERT_NE(core.InfoFor(d1).Typification(), nullptr);
  EXPECT_EQ(*core.InfoFor(d1).Typification(), "B(X1)"_t);
}

TEST_F(UTSchema, UpdateOnLoad) {
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  core.Emplace(a2, "A2", CstType::axiom, "X1=X1 & X2=X2");
  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(d1).Typification(), nullptr);

  core.Load(RSConcept{ x1, "X1", CstType::base });
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::UNKNOWN);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::UNKNOWN);
  EXPECT_EQ(core.InfoFor(d1).Typification(), nullptr);
}

TEST_F(UTSchema, UpdateOnErase) {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  core.Emplace(a2, "A2", CstType::axiom, "X1=X1 & X2=X2");
  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  ASSERT_NE(core.InfoFor(d1).Typification(), nullptr);
  EXPECT_EQ(*core.InfoFor(d1).Typification(), "B(X1)"_t);

  core.Erase(x1);
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, a2));
  EXPECT_FALSE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(a2).status, ParsingStatus::INCORRECT);
  EXPECT_EQ(core.InfoFor(d1).Typification(), nullptr);
}


TEST_F(UTSchema, ChainUpdateDefinition) {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(d1, "D1", CstType::term);
  // TODO: make functional expressions valid in RSParser
  core.Emplace(f1, "F1", CstType::function, "[a \xE2\x88\x88 \xE2\x84\xAC(X1)] D1 \xE2\x88\xAA a");
  core.Emplace(p1, "P1", CstType::predicate, "[a \xE2\x88\x88 \xE2\x84\xAC(X1)] F1[a]=X1");
  core.Emplace(d2, "D2", CstType::term, R"(D{x \in B(X1) | P1[x]})"_rs);
  core.SetDefinitionFor(d1, R"(X1 \union X1)"_rs);
  EXPECT_EQ(core.InfoFor(d1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(f1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(p1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(d2).status, ParsingStatus::VERIFIED);
}

TEST_F(UTSchema, UpdateState) {
  core.Emplace(x1, "X1", CstType::base);
  core.Load(RSConcept{ a1, "A1", CstType::axiom, "X1=X1" });
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::UNKNOWN);

  core.UpdateState();
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::VERIFIED);
}

TEST_F(UTSchema, Translations) {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(a1, "A1", CstType::axiom, "X1=X1");
  core.Emplace(d1, "D1", CstType::term, "debool({X1})");
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, a1));
  EXPECT_TRUE(core.Graph().ConnectionExists(x1, d1));
  EXPECT_EQ(core.InfoFor(a1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.InfoFor(d1).status, ParsingStatus::VERIFIED);
  ASSERT_NE(core.InfoFor(d1).Typification(), nullptr);
  EXPECT_EQ(*core.InfoFor(d1).Typification(), "B(X1)"_t);

  const ccl::StrSubstitutes substitutes{ {"X1", "X2"}, {"X2", "X3"} };
  const auto translator = ccl::CreateTranslator(substitutes);
  {
    Schema test = core;
    test.SubstitueAliases(translator);
    EXPECT_EQ(test.At(x1).alias, "X2");
    EXPECT_EQ(test.At(a1).definition, "X2=X2");
    EXPECT_EQ("debool({X2})", test.At(d1).definition);
    EXPECT_TRUE(test.Graph().ConnectionExists(x1, a1));
    EXPECT_TRUE(test.Graph().ConnectionExists(x1, d1));
    EXPECT_EQ(test.InfoFor(a1).status, ParsingStatus::VERIFIED);
    EXPECT_EQ(test.InfoFor(d1).status, ParsingStatus::VERIFIED);
    ASSERT_NE(test.InfoFor(d1).Typification(), nullptr);
    EXPECT_EQ(*test.InfoFor(d1).Typification(), "B(X2)"_t);
  }
  {
    Schema test = core;
    test.TranslateAll(translator);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(a1).definition, "X2=X2");
    EXPECT_EQ(test.At(d1).definition, "debool({X2})");
    EXPECT_FALSE(test.Graph().ConnectionExists(x1, a1));
    EXPECT_FALSE(test.Graph().ConnectionExists(x1, d1));
    EXPECT_EQ(test.InfoFor(a1).status, ParsingStatus::INCORRECT);
    EXPECT_EQ(test.InfoFor(d1).status, ParsingStatus::INCORRECT);
    EXPECT_EQ(test.InfoFor(d1).Typification(), nullptr);
  }
  {
    Schema test = core;
    test.Translate(a1, translator);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(a1).definition, "X2=X2");
    EXPECT_EQ(test.At(d1).definition, "debool({X1})");
    EXPECT_FALSE(test.Graph().ConnectionExists(x1, a1));
    EXPECT_TRUE(test.Graph().ConnectionExists(x1, d1));
    EXPECT_EQ(test.InfoFor(a1).status, ParsingStatus::INCORRECT);
    EXPECT_EQ(test.InfoFor(d1).status, ParsingStatus::VERIFIED);
    ASSERT_NE(test.InfoFor(d1).Typification(), nullptr);
    EXPECT_EQ(*test.InfoFor(d1).Typification(), "B(X1)"_t);
  }
}

TEST_F(UTSchema, TypeConsistency) {
  const Typification rsType{"X1"};
  EXPECT_TRUE(Schema::CheckTypeConstistency(rsType, CstType::base));
  EXPECT_TRUE(Schema::CheckTypeConstistency(rsType, CstType::constant));
  EXPECT_TRUE(Schema::CheckTypeConstistency(rsType, CstType::structured));
  EXPECT_TRUE(Schema::CheckTypeConstistency(rsType, CstType::term));
  EXPECT_FALSE(Schema::CheckTypeConstistency(rsType, CstType::axiom));
  EXPECT_FALSE(Schema::CheckTypeConstistency(rsType, CstType::theorem));
  EXPECT_TRUE(Schema::CheckTypeConstistency(rsType, CstType::function));
  EXPECT_FALSE(Schema::CheckTypeConstistency(rsType, CstType::predicate));

  const LogicT logicType{};
  EXPECT_FALSE(Schema::CheckTypeConstistency(logicType, CstType::base));
  EXPECT_FALSE(Schema::CheckTypeConstistency(logicType, CstType::constant));
  EXPECT_FALSE(Schema::CheckTypeConstistency(logicType, CstType::structured));
  EXPECT_FALSE(Schema::CheckTypeConstistency(logicType, CstType::term));
  EXPECT_TRUE(Schema::CheckTypeConstistency(logicType, CstType::axiom));
  EXPECT_TRUE(Schema::CheckTypeConstistency(logicType, CstType::theorem));
  EXPECT_FALSE(Schema::CheckTypeConstistency(logicType, CstType::function));
  EXPECT_TRUE(Schema::CheckTypeConstistency(logicType, CstType::predicate));
}

TEST_F(UTSchema, Evaluate) {
  {
    EXPECT_FALSE(core.Evaluate("X1=X1").has_value());
  }
  core.Emplace(x1, "X1", CstType::base);
  {
    const auto eval = core.Evaluate("X1=X1");
    ASSERT_TRUE(eval.has_value());
    EXPECT_TRUE(std::holds_alternative<LogicT>(eval.value()));
  }
  {
    const auto eval = core.Evaluate("{X1}");
    ASSERT_TRUE(eval.has_value());
    EXPECT_TRUE(std::holds_alternative<Typification>(eval.value()));
  }
}

TEST_F(UTSchema, MakeAuditor) {
  core.Emplace(x1, "X1", CstType::base);
  {
    auto auditor = core.MakeAuditor();
    ASSERT_NE(auditor, nullptr);
    EXPECT_TRUE(auditor->CheckType(R"(X1 \in B(X1))"_rs, Syntax::MATH));
    EXPECT_FALSE(auditor->CheckType(R"(X1 \in B(X1))", Syntax::MATH));
  }
  {
    auto auditor = core.MakeAuditor();
    ASSERT_NE(auditor, nullptr);
    EXPECT_FALSE(auditor->CheckType(R"(X1 \in B(X1))"_rs, Syntax::ASCII));
    EXPECT_TRUE(auditor->CheckType(R"(X1 \in B(X1))", Syntax::ASCII));
  }
}