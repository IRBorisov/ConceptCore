#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/ValueAuditor.h"
#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ErrorLogger.h"

#include <unordered_map>

class UTValueAuditor : public ::testing::Test {
protected:
  using ErrorLogger = ccl::rslang::ErrorLogger;
  using ValueAuditor = ccl::rslang::ValueAuditor;
  using SemanticEID = ccl::rslang::SemanticEID;
  using Parser = ccl::rslang::Parser;
  using ValueClass = ccl::rslang::ValueClass;
  using ValueClassContext = ccl::rslang::ValueClassContext;
  using Syntax = ccl::rslang::Syntax;

protected:
  UTValueAuditor();

  RSEnvironment env{};
  Parser parser{};
  ValueAuditor auditor{ env.GetValueContext(), env.GetAST(), parser.log.SendReporter() };

  void ExpectNoWarnings(const std::string& input);
  void ExpectClass(const std::string& input, ValueClass vclass);
  void ExpectError(const std::string& input, const SemanticEID id);
  void ExpectError(const std::string& input, const SemanticEID id, const ccl::StrPos pos);
};

UTValueAuditor::UTValueAuditor() {
  env.data["X1"].valueClass = ValueClass::value;
  env.data["D1"].valueClass = ValueClass::value;
  env.data["D2"].valueClass = ValueClass::props;
  env.data["D3"].valueClass = ValueClass::invalid;
}

void UTValueAuditor::ExpectError(const std::string& input, const SemanticEID id, const ccl::StrPos pos) {
  ExpectError(input, id);
  EXPECT_EQ(parser.log.FirstErrorPos(), pos) << input;
}

void UTValueAuditor::ExpectError(const std::string& input, const SemanticEID id) {
  parser.log.Clear();
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  ASSERT_FALSE(auditor.Check(parser.AST())) << input;
  EXPECT_EQ(begin(parser.log.All())->eid, static_cast<uint32_t>(id)) << input;
}

void UTValueAuditor::ExpectNoWarnings(const std::string& input) {
  parser.log.Clear();
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  ASSERT_TRUE(auditor.Check(parser.AST())) << input;
  EXPECT_EQ(ssize(parser.log.All()), 0) << input;
}

void UTValueAuditor::ExpectClass(const std::string& input, const ValueClass vclass) {
  ExpectNoWarnings(input);
  EXPECT_EQ(auditor.VType(), vclass) << input;
}

TEST_F(UTValueAuditor, NumericCorrect) {
  ExpectClass(R"(1)", ValueClass::value);
  ExpectClass(R"(Z)", ValueClass::props);
  ExpectClass(R"(1 \plus 2)", ValueClass::value);
  ExpectClass(R"(1 \multiply 2)", ValueClass::value);
  ExpectClass(R"(card(X1))", ValueClass::value);
  ExpectClass(R"(card(D1))", ValueClass::value);
}

TEST_F(UTValueAuditor, NumericErrors) {
  ExpectError(R"(card(D2))", SemanticEID::invalidPropertyUsage, 5);
  ExpectError(R"(card(D3))", SemanticEID::globalNoValue, 5);
}

TEST_F(UTValueAuditor, LogicCorrect) {
  ExpectClass(R"(1 \ls 1)", ValueClass::value);
  ExpectClass(R"(1 \eq 1)", ValueClass::value);
  ExpectClass(R"(1 \noteq 1)", ValueClass::value);
  ExpectClass(R"(\neg 1 \eq 1)", ValueClass::value);
  ExpectClass(R"(1 \eq 1 \and 1 \eq 1)", ValueClass::value);

  ExpectClass(R"(X1 \eq X1)", ValueClass::value);
  ExpectClass(R"(X1 \eq {})", ValueClass::value);
  ExpectClass(R"(X1 \in D1)", ValueClass::value);
  ExpectClass(R"(X1 \in D2)", ValueClass::value);
  ExpectClass(R"(X1 \notin D1)", ValueClass::value);
  ExpectClass(R"(X1 \subset X1)", ValueClass::value);
  ExpectClass(R"(X1 \subseteq D2)", ValueClass::value);

  ExpectClass(R"(\E a \in X1 a \eq a)", ValueClass::value);
  ExpectClass(R"(\E a \in D1 a \eq a)", ValueClass::value);
  ExpectClass(R"(\A a \in X1 a \eq a)", ValueClass::value);
  ExpectClass(R"(\A a \in D1 a \eq a)", ValueClass::value);
  ExpectClass(R"(\A a,b \in D1 a \eq a)", ValueClass::value);
  ExpectClass(R"(\A (a,b) \in D1 a \eq b)", ValueClass::value);
}

TEST_F(UTValueAuditor, LogicErrors) {
  ExpectError(R"(D2 \eq X1)", SemanticEID::invalidPropertyUsage, 0);
  ExpectError(R"(X1 \eq D2)", SemanticEID::invalidPropertyUsage, 7);
  ExpectError(R"(X1 \in D3)", SemanticEID::globalNoValue, 7);
  ExpectError(R"(D2 \in X1)", SemanticEID::invalidPropertyUsage, 0);
  ExpectError(R"(X1 \subset D2)", SemanticEID::invalidPropertyUsage, 11);
  ExpectError(R"(X1 \notsubset D2)", SemanticEID::invalidPropertyUsage, 14);
  ExpectError(R"(\E a \in D2 a \eq a)", SemanticEID::invalidPropertyUsage, 9);
  ExpectError(R"(\A a \in D3 a \eq a)", SemanticEID::globalNoValue, 9);
  ExpectError(R"(\E a \in D3 a \eq a)", SemanticEID::globalNoValue, 9);
}

TEST_F(UTValueAuditor, TypedCorrect) {
  ExpectClass(R"({})", ValueClass::value);
  ExpectClass(R"(X1)", ValueClass::value);
  ExpectClass(R"(D2)", ValueClass::props);

  ExpectClass(R"(B(X1))", ValueClass::props);
  ExpectClass(R"(B(D2))", ValueClass::props);

  ExpectClass(R"(X1 \intersect X1)", ValueClass::value);
  ExpectClass(R"(D2 \intersect X1)", ValueClass::value);
  ExpectClass(R"(X1 \intersect D2)", ValueClass::value);
  ExpectClass(R"(D2 \intersect D2)", ValueClass::props);

  ExpectClass(R"(X1 \setminus X1)", ValueClass::value);
  ExpectClass(R"(D2 \setminus X1)", ValueClass::props);
  ExpectClass(R"(X1 \setminus D2)", ValueClass::value);
  ExpectClass(R"(D2 \setminus D2)", ValueClass::props);

  ExpectClass(R"(X1 \union X1)", ValueClass::value);
  ExpectClass(R"(D2 \union X1)", ValueClass::props);
  ExpectClass(R"(X1 \union D2)", ValueClass::props);
  ExpectClass(R"(D2 \union D2)", ValueClass::props);

  ExpectClass(R"(X1*X1)", ValueClass::value);
  ExpectClass(R"(X1*D2)", ValueClass::props);
  ExpectClass(R"(D2*X1)", ValueClass::props);

  ExpectClass(R"(pr1(D1))", ValueClass::value);
  ExpectClass(R"(Pr1(D1))", ValueClass::value);
  ExpectClass(R"(Pr1,2(D1))", ValueClass::value);
  ExpectClass(R"(Fi1[X1](D1))", ValueClass::value);
  ExpectClass(R"(Fi1,2[X1,D2](D1))", ValueClass::value);
  ExpectClass(R"(Fi1[X1](D2))", ValueClass::props);
  ExpectClass(R"(Fi1[D2](X1))", ValueClass::value);
  ExpectClass(R"(Fi1[D2](D2))", ValueClass::props);

  ExpectClass(R"(bool(X1))", ValueClass::value);
  ExpectClass(R"(bool(D1))", ValueClass::value);
  ExpectClass(R"(debool(D1))", ValueClass::value);
  ExpectClass(R"(red(D1))", ValueClass::value);
  ExpectClass(R"((X1, D1))", ValueClass::value);
  ExpectClass(R"({D1, X1})", ValueClass::value);

  ExpectClass(R"(D{t \in X1 | t \eq t})", ValueClass::value);
  ExpectClass(R"(D{t \in X1 | t \in D2})", ValueClass::value);
  ExpectClass(R"(D{t \in D2 | t \eq t})", ValueClass::props);
  ExpectClass(R"(R{a \assign X1 | a \setminus a })", ValueClass::value);
  ExpectClass(R"(R{(a,b) \assign (0,1) | a \ls 3 | (a \plus 1, b \plus 1) })", ValueClass::value);
  ExpectClass(R"(I{(a,b) | a \from X1; X1 \setminus X1 \eq {}; b \assign D1 })", ValueClass::value);
}

TEST_F(UTValueAuditor, TypedErrors) {
  ExpectError(R"(B(D3))", SemanticEID::globalNoValue, 2);
  ExpectError(R"(X1*D3)", SemanticEID::globalNoValue, 3);
  ExpectError(R"(D3*X1)", SemanticEID::globalNoValue, 0);

  ExpectError(R"(pr1(D2))", SemanticEID::invalidPropertyUsage, 4);
  ExpectError(R"(pr1(D3))", SemanticEID::globalNoValue, 4);
  ExpectError(R"(Pr1(D2))", SemanticEID::invalidPropertyUsage, 4);
  ExpectError(R"(Pr1(D3))", SemanticEID::globalNoValue, 4);
  ExpectError(R"(Fi1[D3](X1))", SemanticEID::globalNoValue, 4);
  ExpectError(R"(Fi1[X1](D3))", SemanticEID::globalNoValue, 8);

  ExpectError(R"(bool(D2))", SemanticEID::invalidPropertyUsage, 5);
  ExpectError(R"(bool(D3))", SemanticEID::globalNoValue, 5);
  ExpectError(R"(debool(D2))", SemanticEID::invalidPropertyUsage, 7);
  ExpectError(R"(debool(D3))", SemanticEID::globalNoValue, 7);
  ExpectError(R"(red(D2))", SemanticEID::invalidPropertyUsage, 4);
  ExpectError(R"(red(D3))", SemanticEID::globalNoValue, 4);

  ExpectError(R"((X1, D2))", SemanticEID::invalidPropertyUsage, 5);
  ExpectError(R"((D2, X1))", SemanticEID::invalidPropertyUsage, 1);
  ExpectError(R"((X1, D3))", SemanticEID::globalNoValue, 5);
  ExpectError(R"({X1, D2})", SemanticEID::invalidPropertyUsage, 5);
  ExpectError(R"({D2, X1})", SemanticEID::invalidPropertyUsage, 1);
  ExpectError(R"({X1, D3})", SemanticEID::globalNoValue, 5);

  ExpectError(R"(D{t \in D3 | t \eq t})", SemanticEID::globalNoValue, 8);
  ExpectError(R"(R{a \assign X1 | D2 \setminus a})", SemanticEID::invalidPropertyUsage, 17);
  ExpectError(R"(R{a \assign X1 | D3 \setminus a})", SemanticEID::globalNoValue, 17);
  ExpectError(R"(R{a \assign D2 | a \setminus a})", SemanticEID::invalidPropertyUsage, 12);
  ExpectError(R"(R{a \assign D3 | a \setminus a})", SemanticEID::globalNoValue, 12);
  ExpectError(R"(I{(a,b) | a \from D2; X1 \setminus X1 \eq {}; b \assign D1})", SemanticEID::invalidPropertyUsage, 18);
  ExpectError(R"(I{(a,b) | a \from D3; X1 \setminus X1 \eq {}; b \assign D1})", SemanticEID::globalNoValue, 18);
  ExpectError(R"(I{(a,b) | a \from X1; D2 \setminus X1 \eq {}; b \assign D1})", SemanticEID::invalidPropertyUsage, 22);
  ExpectError(R"(I{(a,b) | a \from X1; D3 \setminus X1 \eq {}; b \assign D1})", SemanticEID::globalNoValue, 22);
  ExpectError(R"(I{(a,b) | a \from X1; X1 \setminus X1 \eq {}; b \assign D2})", SemanticEID::invalidPropertyUsage, 56);
  ExpectError(R"(I{(a,b) | a \from X1; X1 \setminus X1 \eq {}; b \assign D3})", SemanticEID::globalNoValue, 56);
}

TEST_F(UTValueAuditor, GlobalDeclCorrect) {
  ExpectClass(R"(X2 \defexpr )", ValueClass::value);

  ExpectClass(R"(S1 \deftype X1)", ValueClass::value);
  ExpectClass(R"(S1 \deftype B(X1))", ValueClass::value);
  ExpectClass(R"(S1 \deftype B(D1))", ValueClass::value);
  ExpectClass(R"(S1 \deftype B(D2))", ValueClass::value);

  ExpectClass(R"(D4 \defexpr X1)", ValueClass::value);
  ExpectClass(R"(D4 \defexpr B(X1))", ValueClass::props);
  ExpectClass(R"(D4 \defexpr D2)", ValueClass::props);

  ExpectClass(R"(A1 \defexpr 1 \eq 1)", ValueClass::value);
  ExpectClass(R"(T1 \defexpr 1 \eq 1)", ValueClass::value);

  ExpectClass(R"(F1 \defexpr [a \in X1] {a} \union X1)", ValueClass::value);
  ExpectClass(R"(F1 \defexpr [a \in R1] {a})", ValueClass::value);
  ExpectClass(R"(F1 \defexpr [a \in D2] {a} \union X1)", ValueClass::value);
  ExpectClass(R"(F1 \defexpr [a \in X1] {a} \union D2)", ValueClass::props);
}

TEST_F(UTValueAuditor, GlobalDeclErrors) {
  ExpectError(R"(S1 \defexpr B(D3))", SemanticEID::globalNoValue, 14);
  ExpectError(R"(D4 \defexpr D3 \setminus D3)", SemanticEID::globalNoValue, 12);
  ExpectError(R"(A1 \defexpr D3 \eq D3)", SemanticEID::globalNoValue, 12);
  ExpectError(R"(F1 \defexpr [a \in X1] {a} \union red(D2))", SemanticEID::invalidPropertyUsage, 38);
  ExpectError(R"(F1 \defexpr [a \in D3] {a} \union X1)", SemanticEID::globalNoValue, 19);
  ExpectError(R"(F1 \defexpr [a \in X1] {a} \union D3)", SemanticEID::globalNoValue, 34);
}

TEST_F(UTValueAuditor, Functions) {
  env.data["F1"].valueClass = ValueClass::value;
  env.data["P1"].valueClass = ValueClass::value;
  env.data["F2"].valueClass = ValueClass::props;
  env.data["F3"].valueClass = ValueClass::value;
  env.data["F4"].valueClass = ValueClass::invalid;

  env.AddAST("F1", R"(F1 \defexpr [a \in X1, b \in X1] a \setminus b)");
  env.AddAST("P1", R"(P1 \defexpr [a \in X1, b \in X1] a \eq b)");
  env.AddAST("F2", R"(F2 \defexpr [a \in X1, b \in X1] D2 \union (red(a) \setminus b))");

  ExpectClass(R"(F1[X1, X1])", ValueClass::value);
  ExpectClass(R"(P1[X1, X1])", ValueClass::value);
  ExpectClass(R"(F2[X1, X1])", ValueClass::props);
  ExpectClass(R"(F3[X1, X1])", ValueClass::value);

  ExpectClass(R"(F1[X1, D2])", ValueClass::value);
  ExpectClass(R"(F1[D2, D2])", ValueClass::props);
  ExpectClass(R"(F2[X1, D2])", ValueClass::props);

  ExpectError(R"(P1[D2, X1])", SemanticEID::globalFuncNoInterpretation, 0);
  ExpectError(R"(P1[X1, D2])", SemanticEID::globalFuncNoInterpretation, 0);

  ExpectError(R"(F3[D2, X1])", SemanticEID::globalMissingAST, 0);
  ExpectError(R"(F2[D2, X1])", SemanticEID::globalFuncNoInterpretation, 0);
  ExpectError(R"(F1[D3, X1])", SemanticEID::globalNoValue, 3);
  ExpectError(R"(F4[X1])", SemanticEID::globalNoValue, 0);
}