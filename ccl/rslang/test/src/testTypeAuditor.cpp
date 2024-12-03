#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ErrorLogger.h"

class UTTypeAuditor : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using SemanticEID = ccl::rslang::SemanticEID;
  using ErrorStatus = ccl::rslang::ErrorStatus;
  using Parser = ccl::rslang::Parser;
  using TypeAuditor = ccl::rslang::TypeAuditor;
  using ErrorLogger = ccl::rslang::ErrorLogger;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using TypedID = ccl::rslang::TypedID;
  using LogicT = ccl::rslang::LogicT;
  using Syntax = ccl::rslang::Syntax;

protected:
  UTTypeAuditor();

  RSEnvironment env{};
  Parser parser{};
  TypeAuditor analyse{ env, parser.log.SendReporter() };

  void SetupConstants();

  void ExpectNoWarnings(const std::string& input);
  void ExpectLogic(const std::string& input);
  void ExpectTypification(const std::string& input, const Typification& type);
  void ExpectError(const std::string& input, const SemanticEID errorCode);
  void ExpectError(const std::string& input, const SemanticEID errorCode, const ccl::StrPos pos);
};

UTTypeAuditor::UTTypeAuditor() {
  using ccl::rslang::TypedID;
  env.InsertBase("X1");
  env.Insert("S1", "B(X1*X1)"_t);
  env.Insert("S2", "BB(X1)"_t);
  env.Insert("F1", "B(X1)"_t);

  env.data["X1"].traits = ccl::rslang::TraitsNominal;
  env.data["F1"].arguments = { TypedID{ "a", "B(X1)"_t }, TypedID{ "b", "B(X1)"_t } };
}

void UTTypeAuditor::SetupConstants() {
  env.InsertBase("C1");
  env.InsertBase("C2");
  env.InsertBase("C3");

  env.data["C1"].traits = ccl::rslang::TraitsIntegral;
  env.data["C2"].traits = ccl::rslang::TraitsOrdered;
  env.data["C3"].traits = ccl::rslang::TypeTraits{ true, true, true, false };

  env.data["S4"].type = "C1"_t;
  env.data["S5"].type = "C2"_t;
  env.data["S6"].type = "C3"_t;
}

void UTTypeAuditor::ExpectError(const std::string& input, const SemanticEID errorCode, const ccl::StrPos pos) {
  ExpectError(input, errorCode);
  EXPECT_EQ(parser.log.FirstErrorPos(), pos) << input;
}

void UTTypeAuditor::ExpectError(const std::string& input, const SemanticEID errorCode) {
  parser.log.Clear();
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  ASSERT_FALSE(analyse.CheckType(parser.AST())) << input;
  EXPECT_EQ(begin(parser.log.All())->eid, static_cast<uint32_t>(errorCode)) << input;
}

void UTTypeAuditor::ExpectNoWarnings(const std::string& input) {
  parser.log.Clear();
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  ASSERT_TRUE(analyse.CheckType(parser.AST())) << input;
  EXPECT_EQ(ssize(parser.log.All()), 0) << input;
}

void UTTypeAuditor::ExpectTypification(const std::string& input, const Typification& type) {
  ExpectNoWarnings(input);
  const auto* resultType = std::get_if<Typification>(&analyse.GetType());
  ASSERT_TRUE(resultType != nullptr) << input;
  EXPECT_EQ(resultType->ToString(), type.ToString()) << input;
}

void UTTypeAuditor::ExpectLogic(const std::string& input) {
  ExpectNoWarnings(input);
  const auto* resultType = std::get_if<LogicT>(&analyse.GetType());
  ASSERT_TRUE(resultType != nullptr);
}

TEST_F(UTTypeAuditor, DefinitionsCorrect) {
  ExpectTypification(R"(X1 \defexpr )", "B(X1)"_t);
  ExpectTypification(R"(D1 \defexpr X1 \union X1)", "B(X1)"_t);
  ExpectLogic(R"(A1 \defexpr 1 \eq 1)");
  ExpectTypification(R"(D1 \defexpr {})", "B(R0)"_t);

  ExpectTypification(R"(S1 \deftype X1)", "X1"_t);
  ExpectTypification(R"(S1 \deftype Z)", "Z"_t);
  ExpectTypification(R"(S1 \deftype {X1, X1})", "B(X1)"_t);
  ExpectTypification(R"(S1 \deftype X1*X1)", "X1*X1"_t);

  ExpectTypification(R"(F42 \defexpr [a \in X1] {a})", "B(X1)"_t);
  const auto& args = analyse.GetDeclarationArgs();
  ASSERT_EQ(size(args), 1U);
  EXPECT_EQ(args[0].name, "a");
  EXPECT_EQ(args[0].type, "X1"_t);

  ExpectTypification(R"(F42 \defexpr [a \in R1] {a})", "B(R1)"_t);
  const auto& args2 = analyse.GetDeclarationArgs();
  ASSERT_EQ(size(args2), 1U);
  EXPECT_EQ(args2[0].name, "a");
  EXPECT_EQ(args2[0].type, "R1"_t);

  ExpectTypification(R"(F42 \defexpr [a \in D{ b \in X1 | b \eq b}] {a})", "B(X1)"_t);
  const auto& args3 = analyse.GetDeclarationArgs();
  ASSERT_EQ(size(args3), 1U);
  EXPECT_EQ(args3[0].name, "a");
  EXPECT_EQ(args3[0].type, "X1"_t);
}

TEST_F(UTTypeAuditor, DefinitionsErrors) {
  ExpectError(R"(S1 \deftype R1)", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype {})", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype 1)", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype 1 \eq 1)", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype [a \in X1] a \eq a)", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype X1 \union X1)", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype B(X1 \union X1))", SemanticEID::globalStructure);
  ExpectError(R"(S1 \deftype X1*(X1 \union X1))", SemanticEID::globalStructure);

  ExpectError(R"(F42 \defexpr [a \in R1] {a} \union R1)", SemanticEID::radicalUsage, 35);

  ExpectError(R"(a)", SemanticEID::localUndeclared, 0);
}

TEST_F(UTTypeAuditor, NumericCorrect) {
  SetupConstants();

  ExpectTypification(R"(1)", Typification::Integer());
  ExpectTypification(R"(Z)", Typification::Integer().Bool());
  ExpectTypification(R"(card(X1))", "Z"_t);
  ExpectTypification(R"(card(C1))", "Z"_t);

  ExpectTypification(R"(1 \plus 1)", "Z"_t);
  ExpectTypification(R"(1 \minus 1)", "Z"_t);
  ExpectTypification(R"(1 \multiply 1)", "Z"_t);

  ExpectTypification(R"(S4 \plus S4)", "C1"_t);
  ExpectTypification(R"(S4 \plus 1)", "C1"_t);
  ExpectTypification(R"(1 \plus S4)", "C1"_t);
  ExpectTypification(R"(S6 \plus S6)", "C3"_t);

  ExpectLogic(R"(1 \gr 1)");
  ExpectLogic(R"(1 \ge 1)");
  ExpectLogic(R"(1 \ls 1)");
  ExpectLogic(R"(1 \le 1)");

  ExpectLogic(R"(S4 \gr S4)");
  ExpectLogic(R"(S4 \gr 1)");
  ExpectLogic(R"(1 \ls S4)");
  ExpectLogic(R"(S5 \ls S5)");
  ExpectLogic(R"(S6 \ls S6)");

  ExpectLogic(R"(1 \eq 1)");
  ExpectLogic(R"(X1 \eq X1)");
  ExpectLogic(R"(S4 \eq S4)");
  ExpectLogic(R"(S4 \eq 1)");
  ExpectLogic(R"(1 \eq S4)");
  ExpectLogic(R"(S5 \eq S5)");
  ExpectLogic(R"(S6 \eq S6)");
}

TEST_F(UTTypeAuditor, NumericErrors) {
  SetupConstants();

  ExpectError(R"(card(debool(X1)))", SemanticEID::invalidCard, 5);
  ExpectError(R"(card(S4))", SemanticEID::invalidCard, 5);
  ExpectError(R"(card(1))", SemanticEID::invalidCard, 5);
  ExpectError(R"(card({}))", SemanticEID::invalidEmptySetUsage, 5);
  ExpectError(R"(card((1,2)))", SemanticEID::invalidCard, 5);

  ExpectError(R"(debool(X1) \plus 1)", SemanticEID::arithmeticNotSupported, 0);
  ExpectError(R"(1 \plus debool(X1))", SemanticEID::arithmeticNotSupported, 8);
  ExpectError(R"(S5 \plus S5)", SemanticEID::arithmeticNotSupported, 0);
  ExpectError(R"(S5 \plus 1)", SemanticEID::arithmeticNotSupported, 0);
  ExpectError(R"(1 \plus S5)", SemanticEID::arithmeticNotSupported, 8);
  ExpectError(R"(S6 \plus S4)", SemanticEID::typesNotCompatible, 9);
  ExpectError(R"(S6 \plus 1)", SemanticEID::typesNotCompatible, 9);

  ExpectError(R"(debool(X1) \ls 1)", SemanticEID::orderingNotSupported, 0);
  ExpectError(R"(1 \ls debool(X1))", SemanticEID::orderingNotSupported, 6);

  ExpectError(R"(S1 \eq S2)", SemanticEID::typesNotCompatible, 7);
  ExpectError(R"(debool(X1) \eq 1)", SemanticEID::typesNotCompatible, 15);
  ExpectError(R"(1 \eq debool(X1))", SemanticEID::typesNotCompatible, 6);
}

TEST_F(UTTypeAuditor, LogicCorrect) {
  ExpectLogic(R"(\neg 1 \eq 1)");
  ExpectLogic(R"(1 \noteq 1)");
  ExpectLogic(R"(1 \eq 1 \and 1 \eq 1)");

  ExpectLogic(R"(\A a \in X1 a \eq a)");
  ExpectLogic(R"(\A a,b \in X1 a \eq b)");
  ExpectLogic(R"(\A (a,b) \in S1 a \eq b)");
  ExpectLogic(R"(\A a \in X1*X1 a \eq a)");

  ExpectLogic(R"(\A a \in S1 (R{(a1, a2) \assign a | (a1, a2)} \eq a))");
  ExpectLogic(R"(\A a \in X1 R{b \assign a | b} \eq a)");
}

TEST_F(UTTypeAuditor, LogicErrors) {
  ExpectError(R"(\A a \in X1 a \eq {})", SemanticEID::typesNotCompatible, 18);
  ExpectError(R"(\A a \in X1*X1 a \eq {})", SemanticEID::typesNotCompatible, 21);

  ExpectError(R"(\A (a,a) \in S1 pr2(a) \in X1)", SemanticEID::localShadowing, 6);
  ExpectError(R"(\A (a,b) \in X1 1 \eq 1)", SemanticEID::invalidBinding, 4);
  ExpectError(R"(\A (a,b,c) \in S1 1 \eq 1)", SemanticEID::invalidBinding, 4);

  ExpectError(R"(\A t \in X1 {t \in X1 | t \eq t} \eq X1)", SemanticEID::localShadowing, 13);
  ExpectError(R"(\A t \in X1 t \eq t \and t \eq X1)", SemanticEID::localOutOfScope, 25);
}

TEST_F(UTTypeAuditor, ConstructorsCorrect) {
  SetupConstants();

  ExpectTypification(R"({X1, X1})", "BB(X1)"_t);
  ExpectTypification(R"({{}, X1})", "BB(X1)"_t);
  ExpectTypification(R"({X1, {}})", "BB(X1)"_t);
  ExpectTypification(R"({{}})", "BB(R0)"_t);
  ExpectTypification(R"({1})", "B(Z)"_t);
  ExpectTypification(R"({1, card(X1)})", "B(Z)"_t);
  ExpectTypification(R"({S4, 1})", "B(C1)"_t);
  ExpectTypification(R"({1, S4})", "B(C1)"_t);

  ExpectTypification(R"((X1, X1))", "B(X1)*B(X1)"_t);
  ExpectTypification(R"((1, 2))", "Z*Z"_t);
  ExpectTypification(R"({(1, S4), (S4, 1)})", "B(C1*C1)"_t);
  
  ExpectTypification(R"(D{t \in B(X1) | t \eq t})", "BB(X1)"_t);
  ExpectTypification(R"(D{t \in Z | t \gr 2})", "B(Z)"_t);
  ExpectTypification(R"(D{t \in S1 | pr2(t) \eq pr1(t)})", "B(X1*X1)"_t);

  ExpectTypification(R"(R{a \assign S1 | card(a) \ls 10 | a \union a})", "B(X1*X1)"_t);
  ExpectTypification(R"(R{a \assign S1 | a \union a})", "B(X1*X1)"_t);
  ExpectTypification(R"(R{a \assign 1 | a \ls 10 | a \plus 1})", "Z"_t);
  ExpectTypification(R"(R{a \assign 1 | a \ls S4 | a \plus S4})", "C1"_t);
  ExpectTypification(R"(R{a \assign {} | a \union {S4}})", "B(C1)"_t);
  ExpectTypification(R"(R{a \assign {} | Pr1(a) \eq Pr2(a) | a \union (X1*X1)})", "B(X1*X1)"_t);
  ExpectTypification(R"(R{a \assign {} | a \union D{x \in X1 | x \eq x}})", "B(X1)"_t);
  ExpectTypification(R"(R{a \assign {} | red(a) \eq {} | a \union {X1}})", "BB(X1)"_t);
  ExpectTypification(
    R"(R{(a, b) \assign (I{(x, {}) | x \from X1}, {}) | (X1 * B(X1*X1), b \union Fi1[X1](a))})",
    "B(X1*B(X1*X1))*B(X1*B(X1*X1))"_t
  );

  ExpectTypification(R"(I{(a, b) | a \from X1; b \assign a})", "B(X1*X1)"_t);
  ExpectTypification(R"(I{(a, b) | a \from X1; b \assign a; 1 \eq 1})", "B(X1*X1)"_t);
  ExpectTypification(R"(I{(a, c) | (a,c) \from X1*X1; (b,d) \assign (a,a); d \eq b})", "B(X1*X1)"_t);
}

TEST_F(UTTypeAuditor, ConstructorsErrors) {
  SetupConstants();

  ExpectError(R"({X1, S1})", SemanticEID::invalidEnumeration, 5);
  ExpectError(R"({S1, X1})", SemanticEID::invalidEnumeration, 5);
  ExpectError(R"({1, X1})", SemanticEID::invalidEnumeration, 4);
  ExpectError(R"({X1, 1})", SemanticEID::invalidEnumeration, 5);
  ExpectError(R"({1, S5})", SemanticEID::invalidEnumeration, 4);
  ExpectError(R"({S5, S6})", SemanticEID::invalidEnumeration, 5);
  ExpectError(R"({1, S6})", SemanticEID::invalidEnumeration, 4);
  ExpectError(R"({S6, 1})", SemanticEID::invalidEnumeration, 5);

  ExpectError(R"(R{a \assign S1 | {a}})", SemanticEID::typesNotEqual, 17);
  ExpectError(R"(R{a \assign {} | a \union S4})", SemanticEID::invalidTypeOperation, 26);
  ExpectError(R"(R{a \assign {} | Pr1(a) \eq Pr2(a) | a \union X1})", SemanticEID::invalidProjectionSet, 21);
  ExpectError(R"(R{a \assign {} | red(a) \eq {} | a \union (X1*X1)})", SemanticEID::invalidReduce, 22);
  ExpectError(R"(\A a \in S1 R{(a1, a2) \assign a | a1} \eq a)", SemanticEID::typesNotEqual, 35);

  ExpectError(R"(I{(a, b) | a \from X1; b \assign {a}; a \noteq b})", SemanticEID::typesNotCompatible, 47);
  ExpectError(R"(I{(a, b) | a \from X1; g \in a \setminus X2; b \assign {a}})", SemanticEID::invalidTypeOperation, 29);
  ExpectError(R"(I{(a, c) | a \from X1; (a,c) \assign (a,a)})", SemanticEID::localShadowing, 24);
  ExpectError(R"(I{(a, c) | a \from X1; (a,c) \from X1*X1})", SemanticEID::localShadowing, 24);
}

TEST_F(UTTypeAuditor, TypedPredicatesCorrect) {
  SetupConstants();

  ExpectLogic(R"(X1 \eq X1)");
  ExpectLogic(R"(X1 \eq {})");
  ExpectLogic(R"(Z \eq {})");

  ExpectLogic(R"(1 \in Z)");
  ExpectLogic(R"(S4 \in C1)");
  ExpectLogic(R"(S4 \in Z)");
  ExpectLogic(R"(X1 \in S2)");

  ExpectLogic(R"(X1 \subset X1)");

  ExpectLogic(R"(X1 \notin S2)");
  ExpectLogic(R"(X1 \subseteq X1)");
  ExpectLogic(R"(X1 \notsubset X1)");
}

TEST_F(UTTypeAuditor, TypedPredicatesError) {
  SetupConstants();

  ExpectError(R"(X1 \in X1)", SemanticEID::invalidElementPredicate, 7);
  ExpectError(R"(S4 \in C2)", SemanticEID::invalidElementPredicate, 7);
  ExpectError(R"(S5 \in Z)", SemanticEID::invalidElementPredicate, 7);
  ExpectError(R"(S6 \in Z)", SemanticEID::invalidElementPredicate, 7);
  ExpectError(R"((1,2) \in Z)", SemanticEID::invalidElementPredicate, 10);
  ExpectError(R"({1} \in Z)", SemanticEID::invalidElementPredicate, 8);
  ExpectError(R"((1,2) \in Z)", SemanticEID::invalidElementPredicate, 10);

  ExpectError(R"(X1 \subset S2)", SemanticEID::typesNotEqual, 11);

  ExpectError(R"(X1 \notin X1)", SemanticEID::invalidElementPredicate, 10);
  ExpectError(R"(X1 \subseteq S2)", SemanticEID::typesNotEqual, 13);
  ExpectError(R"(X1 \notsubset S2)", SemanticEID::typesNotEqual, 14);
}

TEST_F(UTTypeAuditor, TypedOperationsCorrect) {
  SetupConstants();

  ExpectTypification(R"(X1 \union X1)", "B(X1)"_t);
  ExpectTypification(R"(X1 \setminus X1)", "B(X1)"_t);
  ExpectTypification(R"(X1 \intersect X1)", "B(X1)"_t);
  ExpectTypification(R"(X1 \symmdiff X1)", "B(X1)"_t);
  
  ExpectTypification(R"(C1 \union C1)", "B(C1)"_t);
  ExpectTypification(R"({1} \union C1)", "B(C1)"_t);
  ExpectTypification(R"(C1 \union {1})", "B(C1)"_t);
  ExpectTypification(R"({(S4, 1)} \union {(1, S4)})", "B(C1*C1)"_t);

  ExpectTypification(R"(X1)", "B(X1)"_t);
  ExpectTypification(R"(B(X1))", "BB(X1)"_t);
  ExpectTypification(R"(X1*X1)", "B(X1*X1)"_t);
  ExpectTypification(R"(X1*{})", "B(X1*R0)"_t);
  ExpectTypification(R"(Pr1(S1))", "B(X1)"_t);
  ExpectTypification(R"(Fi1[X1](S1))", "B(X1*X1)"_t);
  ExpectTypification(R"(Fi1,2[X1, X1](S1))", "B(X1*X1)"_t);
  ExpectTypification(R"(Fi1,2[X1, X1](X1*X1))", "B(X1*X1)"_t);
  ExpectTypification(R"(Fi1,2[X1 * X1](S1))", "B(X1*X1)"_t);
  ExpectTypification(R"(Fi1[{1,2,3}](Z*X1))", "B(Z*X1)"_t);
  ExpectTypification(R"(Fi1[{1,2,3}](C1*X1))", "B(C1*X1)"_t);
  ExpectTypification(R"(Pr1,2(S1))", "B(X1*X1)"_t);
  ExpectTypification(R"(bool(X1))", "BB(X1)"_t);
  ExpectTypification(R"(debool({X1}))", "B(X1)"_t);
  ExpectTypification(R"(red(S2))", "B(X1)"_t);
}

TEST_F(UTTypeAuditor, TypedOperationsErrors) {
  SetupConstants();

  ExpectError(R"(X1 \union S2)", SemanticEID::typesNotEqual, 10);
  ExpectError(R"(X1 \union {})", SemanticEID::invalidEmptySetUsage, 10);
  ExpectError(R"(S2 \union X1)", SemanticEID::typesNotEqual, 10);

  ExpectError(R"(Pr1(X1))", SemanticEID::invalidProjectionSet, 4);
  ExpectError(R"(Pr1({}))", SemanticEID::invalidEmptySetUsage, 4);
  ExpectError(R"(Pr3(S1))", SemanticEID::invalidProjectionSet, 4);
  ExpectError(R"(pr1(debool(X1)))", SemanticEID::invalidProjectionTuple, 4);
  ExpectError(R"(pr1({}))", SemanticEID::invalidEmptySetUsage, 4);
  ExpectError(R"(Fi1[X1](B(X1)))", SemanticEID::invalidFilterArgumentType, 8);
  ExpectError(R"(Fi1[1](B(X1)))", SemanticEID::invalidFilterArgumentType, 7);
  ExpectError(R"(Fi1[S4](B(X1)))", SemanticEID::invalidFilterArgumentType, 8);
  ExpectError(R"(Fi1[Z](B(X1)))", SemanticEID::invalidFilterArgumentType, 7);
  ExpectError(R"(Fi3[X1](X1*X1))", SemanticEID::invalidFilterArgumentType, 8);
  ExpectError(R"(Fi1[X1](B(X1)*X1))", SemanticEID::typesNotEqual, 4);
  ExpectError(R"(Fi1,2[X1,{X1}](X1*X1))", SemanticEID::typesNotEqual, 9);
  ExpectError(R"(Fi1,2[X1*{X1}](B(X1)*X1))", SemanticEID::typesNotEqual, 6);
  ExpectError(R"(Fi1,2,1[X1, X1](X1*X1))", SemanticEID::invalidFilterArity, 0);
  ExpectError(R"(\A a \in X1 Fi1[a](B(X1)*X1) \eq X1)", SemanticEID::typesNotEqual, 16);
  ExpectError(R"(\A a \in X1*X1 Fi1[a](B(X1)*X1) \eq X1)", SemanticEID::typesNotEqual, 19);
  ExpectError(R"(red(X1))", SemanticEID::invalidReduce, 5);
  ExpectError(R"(red({}))", SemanticEID::invalidEmptySetUsage, 4);

  ExpectError(R"(\A a \in X1 B(a) \eq X1)", SemanticEID::invalidBoolean, 14);
  ExpectError(R"(\A a \in S1 B(a) \eq X1)", SemanticEID::invalidBoolean, 14);
  ExpectError(R"(\A a \in X1 a*a \in S1)", SemanticEID::invalidDecart, 12);
  ExpectError(R"(\A a \in X1 debool(a) \eq X1)", SemanticEID::invalidDebool, 19);
  ExpectError(R"(\A a \in S1 pr3(a) \in X1)", SemanticEID::invalidProjectionTuple, 16);
}

TEST_F(UTTypeAuditor, TypedFunctions) {
  ExpectTypification(R"(F1[X1, X1])", "B(X1)"_t);
  ExpectTypification(R"(F1[{}, X1])", "B(X1)"_t);
  ExpectTypification(R"(F1[X1, {}])", "B(X1)"_t);
  ExpectTypification(R"(F1[X1 \union X1, X1])", "B(X1)"_t);
  ExpectTypification(R"(F1[Pr1(S1), Pr2(S1)])", "B(X1)"_t);

  ExpectError(R"(F1)", SemanticEID::globalFuncWithoutArgs, 0);
  ExpectError(R"(F1[X1])", SemanticEID::invalidArgsArity, 3);
  EXPECT_EQ(parser.log.All()[0].params[0], "1");
  EXPECT_EQ(parser.log.All()[0].params[1], "2");
  ExpectError(R"(F1[X1, X1, X1])", SemanticEID::invalidArgsArity, 3);
  EXPECT_EQ(parser.log.All()[0].params[0], "3");
  EXPECT_EQ(parser.log.All()[0].params[1], "2");

  ExpectError(R"(F1[S1, X1])", SemanticEID::invalidArgumentType, 3);
  ExpectError(R"(F1[X1, S1])", SemanticEID::invalidArgumentType, 7);
}

TEST_F(UTTypeAuditor, LogicFunctions) {
  env.Insert("P1", ccl::rslang::LogicT{});

  ccl::rslang::FunctionArguments p1Args{};
  p1Args.emplace_back(ccl::rslang::TypedID{ "a", "B(X1)"_t });
  p1Args.emplace_back(ccl::rslang::TypedID{ "b", "B(X1)"_t });
  env.data["P1"].arguments  = p1Args;

  ExpectLogic(R"(P1[X1, X1])");
  ExpectLogic(R"(P1[X1 \union X1, X1])");
  ExpectLogic(R"(P1[Pr1(S1), Pr2(S1)])");

  ExpectError(R"(P1)", SemanticEID::globalFuncWithoutArgs, 0);
  ExpectError(R"(P1[X1])", SemanticEID::invalidArgsArity, 3);
  ExpectError(R"(P1[X1, X1, X1])", SemanticEID::invalidArgsArity, 3);

  ExpectError(R"(P1[S1, X1])", SemanticEID::invalidArgumentType, 3);
  ExpectError(R"(P1[X1, S1])", SemanticEID::invalidArgumentType, 7);
}

TEST_F(UTTypeAuditor, TemplatedFunctions) {
  SetupConstants();

  env.Insert("F2", "B(R1F2)"_t);
  FunctionArguments f2Args{};
  f2Args.emplace_back("a", "B(R1F2)"_t);
  f2Args.emplace_back("b", "R1F2"_t);
  env.data["F2"].arguments = f2Args;

  ExpectTypification(R"(F2[B(X1), X1])", "BB(X1)"_t);
  ExpectTypification(R"(F2[{}, X1])", "BB(X1)"_t);
  ExpectTypification(R"(F2[B(X1), {}])", "BB(X1)"_t);
  ExpectTypification(R"(F2[{}, {}])", "BB(R0)"_t);
  ExpectTypification(R"(F2[Z, 1])", "B(Z)"_t);
  ExpectTypification(R"(F2[Z, S4])", "B(C1)"_t);
  ExpectTypification(R"(F2[{S4}, 1])", "B(C1)"_t);
  ExpectError(R"(F2[X1, B(X1)])", SemanticEID::invalidArgumentType, 7);

  env.Insert("F3", "R1*R2"_t);
  FunctionArguments f3Args{};
  f3Args.emplace_back("a", "B(R1)"_t);
  f3Args.emplace_back("b", "B(R1*R2)"_t);
  env.data["F3"].arguments = f3Args;

  ExpectTypification(R"(F3[X1, X1*B(X1)])", "X1*B(X1)"_t);
  ExpectError(R"(F3[X1, X1])", SemanticEID::invalidArgumentType, 7);
  ExpectError(R"(F3[X1, X1*X1*X1])", SemanticEID::invalidArgumentType, 7);
  ExpectError(R"(F3[X1, (X1*X1)*X1])", SemanticEID::invalidArgumentType, 7);
}

TEST_F(UTTypeAuditor, TemplatedFunctionsNesting) {
  env.Insert("F2", "B(R1)"_t);
  FunctionArguments f2Args{};
  f2Args.emplace_back(TypedID{ "a", "R1"_t });
  f2Args.emplace_back(TypedID{ "b", "R1*R2"_t });
  env.data["F2"].arguments = f2Args;

  ExpectTypification(R"(F3 \defexpr [a \in R2, b \in R2*R1] F2[a, b])", "B(R2)"_t);
  ExpectTypification(R"(F3 \defexpr [a \in R3, b \in R3*R4] F2[a, b])", "B(R3)"_t);
  ExpectError(R"(F3 \defexpr [a \in R2, b \in R1*R2] F2[a, b])", SemanticEID::invalidArgumentType, 42);
  ExpectError(R"(F3 \defexpr [a \in R3, b \in R1*R2] F2[a, b])", SemanticEID::invalidArgumentType, 42);
}

TEST_F(UTTypeAuditor, ErrorPopup) {
  ExpectError(R"(1 \ls card(X42))", SemanticEID::globalNotTyped, 11);
  ExpectError(R"(card(X42) \ls 1)", SemanticEID::globalNotTyped, 5);
  ExpectError(R"(1 \plus card(X42))", SemanticEID::globalNotTyped, 13);
  ExpectError(R"(card(X42) \plus 1)", SemanticEID::globalNotTyped, 5);

  ExpectError(R"(X42 \in X1)", SemanticEID::globalNotTyped, 0);
  ExpectError(R"(X1 \in X42)", SemanticEID::globalNotTyped, 7);
  ExpectError(R"(X42 \eq X1)", SemanticEID::globalNotTyped, 0);
  ExpectError(R"(X42 \eq {})", SemanticEID::globalNotTyped, 0);
  ExpectError(R"(X1 \eq X42)", SemanticEID::globalNotTyped, 7);
  ExpectError(R"(1 \eq 1 \and X1 \eq X42)", SemanticEID::globalNotTyped, 20);

  ExpectError(R"(Pr1(X42))", SemanticEID::globalNotTyped, 4);
  ExpectError(R"(pr1(X42))", SemanticEID::globalNotTyped, 4);
  ExpectError(R"(Fi1[X1](X42))", SemanticEID::globalNotTyped, 8);
  ExpectError(R"(Fi1[X42](X1*X1))", SemanticEID::globalNotTyped, 4);
  ExpectError(R"(bool(X42))", SemanticEID::globalNotTyped, 5);
  ExpectError(R"(debool(X42))", SemanticEID::globalNotTyped, 7);
  ExpectError(R"(red(X42))", SemanticEID::globalNotTyped, 4);
  ExpectError(R"((X1, X42))", SemanticEID::globalNotTyped, 5);
  ExpectError(R"((X42, X1))", SemanticEID::globalNotTyped, 1);
  ExpectError(R"({X42})", SemanticEID::globalNotTyped, 1);
  ExpectError(R"({X1, X42})", SemanticEID::globalNotTyped, 5);
  ExpectError(R"({X42, X1})", SemanticEID::globalNotTyped, 1);

  ExpectError(R"(S4 \defexpr X42)", SemanticEID::globalNotTyped, 12);
  ExpectError(R"(D1 \defexpr X42)", SemanticEID::globalNotTyped, 12);
  ExpectError(R"(A1 \defexpr X42 \eq X42)", SemanticEID::globalNotTyped, 12);

  ExpectError(R"(D{t \in X42 | t \eq t})", SemanticEID::globalNotTyped, 8);
  ExpectError(R"(I{(t, X42) | t \from X1})", SemanticEID::globalNotTyped, 6);
  ExpectError(R"(I{t | t \from X42})", SemanticEID::globalNotTyped, 14);
  ExpectError(R"(R{t \assign X42 | t})", SemanticEID::globalNotTyped, 12);
  ExpectError(R"(R{t \assign X1 | t \union X42})", SemanticEID::globalNotTyped, 26);
  ExpectError(R"(R{t \assign X1 | 1 \eq 1 | t \union X42})", SemanticEID::globalNotTyped, 36);
  ExpectError(R"(R{t \assign X1 | X42 \eq X42 | t \union X1})", SemanticEID::globalNotTyped, 17);

  ExpectError(R"(F42[X1])", SemanticEID::globalNotTyped, 0);
  ExpectError(R"(F42 \defexpr [a \in X50] {a})", SemanticEID::globalNotTyped, 20);
  ExpectError(R"(F1 \defexpr [a \in X1, a \in X1] {a})", SemanticEID::localShadowing, 23);

  ExpectError(R"(F1[F21[X1], X1])", SemanticEID::globalNotTyped, 3);
  ExpectError(R"(F1[X1, F21[X1]])", SemanticEID::globalNotTyped, 7);
}

TEST_F(UTTypeAuditor, WarningLocalNotUsed) {
  const auto input = R"(D{t \in X1 | \A a \in X1 1 \eq 1})";
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII));
  ASSERT_TRUE(analyse.CheckType(parser.AST()));
  EXPECT_EQ(parser.log.Count(ErrorStatus::WARNING), 2); // Note: 1 warning to 'a' and 1 warning for 't'
  EXPECT_EQ(begin(parser.log.All())->eid, static_cast<uint32_t>(SemanticEID::localNotUsed));
}

TEST_F(UTTypeAuditor, WarningParameterNotUsed) {
  const auto input = R"([a \in X1] 1 \eq 1)";
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII));
  ASSERT_TRUE(analyse.CheckType(parser.AST()));
  EXPECT_EQ(parser.log.Count(ErrorStatus::WARNING), 1);
  EXPECT_EQ(begin(parser.log.All())->eid, static_cast<uint32_t>(SemanticEID::localNotUsed));
}

TEST_F(UTTypeAuditor, NoWarningAfterError) {
  const auto input = R"(D{t \in X1 | \A a \in X1 \A b \in red(X1) {a,b} \eq t})";
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII));
  ASSERT_FALSE(analyse.CheckType(parser.AST()));
  EXPECT_EQ(parser.log.Count(ErrorStatus::WARNING), 0);
  EXPECT_EQ(parser.log.Count(ErrorStatus::CRITICAL), 1);
}

TEST_F(UTTypeAuditor, WarningLocalRedeclared) {
  const auto input = R"(D{t \in X1 | \A a \in X1 a \eq t \and \A a \in X1 a \eq t})";
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII));
  ASSERT_TRUE(analyse.CheckType(parser.AST()));
  EXPECT_EQ(parser.log.Count(ErrorStatus::WARNING), 1);
  EXPECT_EQ(begin(parser.log.All())->eid, static_cast<uint32_t>(SemanticEID::localDoubleDeclare));
}