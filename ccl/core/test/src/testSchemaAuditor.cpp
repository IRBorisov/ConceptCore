#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/SchemaAuditor.h"
#include "ccl/semantic/Schema.h"
#include "RSLHelper.hpp"

class UTSchemaAuditor : public ::testing::Test {
protected:
  UTSchemaAuditor();

protected:
  using EntityUID = ccl::EntityUID;
  using Schema = ccl::semantic::Schema;
  using CstType = ccl::semantic::CstType;
  using SchemaAuditor = ccl::semantic::SchemaAuditor;
  using Syntax = ccl::rslang::Syntax;
  using ValueClass = ccl::rslang::ValueClass;
  using Typification = ccl::rslang::Typification;
  using LogicT = ccl::rslang::LogicT;
  using ExpressionType = ccl::rslang::ExpressionType;
  using CstTypeEID = ccl::semantic::CstTypeEID;

  using ParsingStatus = ccl::semantic::ParsingStatus;
  using ParsingInfo = ccl::semantic::ParsingInfo;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using TypeID = ccl::rslang::TypedID;

protected:
  Schema core{};
  std::unique_ptr<SchemaAuditor> auditor;

  static constexpr EntityUID x1{ 42 };
  static constexpr EntityUID s1{ 44 };
  static constexpr EntityUID d1{ 45 };
  static constexpr EntityUID a1{ 46 };
  static constexpr EntityUID f1{ 47 };
  static constexpr EntityUID p1{ 48 };

  void ExpectError(const std::string& alias, const std::string& input, CstType type, CstTypeEID errorCode);
};

UTSchemaAuditor::UTSchemaAuditor() : auditor{core.MakeAuditor()} {
  core.Emplace(x1, "X1", CstType::base);
  core.Emplace(s1, "S1", CstType::structured, "B(X1*X1)"_rs);
  core.UpdateState();
}

void UTSchemaAuditor::ExpectError(
  const std::string& alias,
  const std::string& input,
  const CstType type,
  const CstTypeEID errorCode
) {
  ASSERT_FALSE(auditor->CheckConstituenta(alias, input, type)) << input;
  EXPECT_EQ(begin(auditor->Errors().All())->eid, static_cast<uint32_t>(errorCode)) << input;
}

TEST_F(UTSchemaAuditor, CheckExression) {
  EXPECT_TRUE(auditor->CheckExpression(R"(X1=X1)", Syntax::MATH));
  EXPECT_TRUE(auditor->CheckExpression(R"(X1 \eq X1)", Syntax::ASCII));
  EXPECT_FALSE(auditor->CheckExpression(R"(X2=X2)", Syntax::MATH));
  EXPECT_FALSE(auditor->CheckExpression(R"(X1=X1)", Syntax::ASCII));
}

TEST_F(UTSchemaAuditor, CheckConstituentaCorrect) {
  EXPECT_TRUE(auditor->CheckConstituenta("X2", "", CstType::base));
  EXPECT_TRUE(auditor->CheckConstituenta("C2", "", CstType::constant));
  EXPECT_TRUE(auditor->CheckConstituenta("S2", R"(X1)", CstType::structured));
  EXPECT_TRUE(auditor->CheckConstituenta("D2", R"(Pr1(S1))", CstType::term));
  EXPECT_TRUE(auditor->CheckConstituenta("A2", R"(X1=X1)", CstType::axiom));
  EXPECT_TRUE(auditor->CheckConstituenta("T2", R"(X1=X1)", CstType::theorem));
  EXPECT_TRUE(auditor->CheckConstituenta("F2", R"([a \in X1] a)"_rs, CstType::function));
  EXPECT_TRUE(auditor->CheckConstituenta("P2", R"([a \in X1] a \eq a)"_rs, CstType::predicate));
}

TEST_F(UTSchemaAuditor, CheckConstituentaErrors) {
  ExpectError("X2", "1", CstType::base, CstTypeEID::cstNonemptyBase);
  ExpectError("D2", "", CstType::term, CstTypeEID::cstEmptyDerived);
  ExpectError("F2", "1=1", CstType::function, CstTypeEID::cstCallableNoArgs);
  ExpectError("P2", "1=1", CstType::predicate, CstTypeEID::cstCallableNoArgs);
  ExpectError("D2", R"([a \in X1] a \eq a)"_rs, CstType::term, CstTypeEID::cstNonCallableHasArgs);
  ExpectError("A2", R"([a \in X1] a \eq a)"_rs, CstType::axiom, CstTypeEID::cstNonCallableHasArgs);
  ExpectError("A2", R"(X1 \union X1)"_rs, CstType::axiom, CstTypeEID::cstExpectedLogical);
  ExpectError("T2", R"(X1 \union X1)"_rs, CstType::theorem, CstTypeEID::cstExpectedLogical);
  ExpectError("P2", R"([a \in X1] a)"_rs, CstType::predicate, CstTypeEID::cstExpectedLogical);
  ExpectError("D2", R"(1 \eq 1)"_rs, CstType::term, CstTypeEID::cstExpectedTyped);
  ExpectError("F2", R"([a \in X1] a \eq a)"_rs, CstType::function, CstTypeEID::cstExpectedTyped);
}