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
	using RSParser = ccl::rslang::RSParser;
	using TypeAuditor = ccl::rslang::TypeAuditor;
	using AsciiLexer = ccl::rslang::AsciiLexer;
	using ErrorLogger = ccl::rslang::ErrorLogger;
	using FunctionArguments = ccl::rslang::FunctionArguments;
	using TypedID = ccl::rslang::TypedID;
	using LogicT = ccl::rslang::LogicT;

protected:
	UTTypeAuditor();

	RSEnvironment env{};
	ErrorLogger log{};
	AsciiLexer lexer{ log.SendReporter() };
	RSParser parser{ log.SendReporter() };
	TypeAuditor analyse{ env, log.SendReporter() };

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
	EXPECT_EQ(log.FirstErrorPos(), pos) << input;
}

void UTTypeAuditor::ExpectError(const std::string& input, const SemanticEID errorCode) {
	log.Clear();
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
	ASSERT_FALSE(analyse.CheckType(parser.AST())) << input;
	EXPECT_EQ(begin(log.All())->eid, static_cast<uint32_t>(errorCode)) << input;
}

void UTTypeAuditor::ExpectNoWarnings(const std::string& input) {
	log.Clear();
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
	ASSERT_TRUE(analyse.CheckType(parser.AST())) << input;
	EXPECT_EQ(ssize(log.All()), 0) << input;
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
	ExpectTypification(R"(X1:==)", "B(X1)"_t);
	ExpectTypification(R"(D1:==X1 \union X1)", "B(X1)"_t);
	ExpectLogic(R"(A1:==1=1)");

	ExpectTypification(R"(S1::=X1)", "X1"_t);
	ExpectTypification(R"(S1::=Z)", "Z"_t);
	ExpectTypification(R"(S1::=X1*X1)", "X1*X1"_t);

	ExpectTypification(R"(F42:==[a \in X1] {a})", "B(X1)"_t);
	const auto& args = analyse.GetDeclarationArgs();
	ASSERT_EQ(size(args), 1U);
	EXPECT_EQ(args[0].name, "a");
	EXPECT_EQ(args[0].type, "X1"_t);

	ExpectTypification(R"(F42:==[a \in R1] {a})", Typification("R1").Bool());
	const auto& args2 = analyse.GetDeclarationArgs();
	ASSERT_EQ(size(args2), 1U);
	EXPECT_EQ(args2[0].name, "a");
	EXPECT_EQ(args2[0].type, Typification("R1"));

	ExpectTypification(R"(F42:==[a \in D{ b \in X1 | b=b}] {a})", "B(X1)"_t);
	const auto& args3 = analyse.GetDeclarationArgs();
	ASSERT_EQ(size(args3), 1U);
	EXPECT_EQ(args3[0].name, "a");
	EXPECT_EQ(args3[0].type, "X1"_t);
}

TEST_F(UTTypeAuditor, DefinitionsErrors) {
	ExpectError(R"(S1::=R1)", SemanticEID::globalStructure);
	ExpectError(R"(S1::=1)", SemanticEID::globalStructure);
	ExpectError(R"(S1::=X1 \union X1)", SemanticEID::globalStructure);
	ExpectError(R"(S1::=B(X1 \union X1))", SemanticEID::globalStructure);
	ExpectError(R"(S1::=X1*(X1 \union X1))", SemanticEID::globalStructure);

	ExpectError(R"(F42:==[a \in R1] {a} \union R1)", SemanticEID::radicalUsage, 28);

	ExpectError(R"(a)", SemanticEID::localUndeclared, 0);
}

TEST_F(UTTypeAuditor, NumericCorrect) {
	SetupConstants();

	ExpectTypification(R"(1)", Typification::Integer());
	ExpectTypification(R"(Z)", Typification::Integer().Bool());
	ExpectTypification(R"(card(X1))", "Z"_t);
	ExpectTypification(R"(card(C1))", "Z"_t);

	ExpectTypification(R"(1 + 1)", "Z"_t);
	ExpectTypification(R"(1 - 1)", "Z"_t);
	ExpectTypification(R"(1 \multiply 1)", "Z"_t);

	ExpectTypification(R"(S4 + S4)", "C1"_t);
	ExpectTypification(R"(S4 + 1)", "C1"_t);
	ExpectTypification(R"(1 + S4)", "C1"_t);
	ExpectTypification(R"(S6 + S6)", "C3"_t);

	ExpectLogic(R"(1 > 1)");
	ExpectLogic(R"(1 >= 1)");
	ExpectLogic(R"(1 < 1)");
	ExpectLogic(R"(1 <= 1)");

	ExpectLogic(R"(S4 < S4)");
	ExpectLogic(R"(S4 < 1)");
	ExpectLogic(R"(1 < S4)");
	ExpectLogic(R"(S5 < S5)");
	ExpectLogic(R"(S6 < S6)");

	ExpectLogic(R"(1 = 1)");
	ExpectLogic(R"(X1 = X1)");
	ExpectLogic(R"(S4 = S4)");
	ExpectLogic(R"(S4 = 1)");
	ExpectLogic(R"(1 = S4)");
	ExpectLogic(R"(S5 = S5)");
	ExpectLogic(R"(S6 = S6)");
}

TEST_F(UTTypeAuditor, NumericErrors) {
	SetupConstants();

	ExpectError(R"(card(debool(X1)))", SemanticEID::invalidCard, 5);
	ExpectError(R"(card(S4))", SemanticEID::invalidCard, 5);
	ExpectError(R"(card(1))", SemanticEID::invalidCard, 5);
	ExpectError(R"(card((1,2)))", SemanticEID::invalidCard, 5);

	ExpectError(R"(debool(X1) + 1)", SemanticEID::arithmeticNotSupported, 0);
	ExpectError(R"(1 + debool(X1))", SemanticEID::arithmeticNotSupported, 4);
	ExpectError(R"(S5 + S5)", SemanticEID::arithmeticNotSupported, 0);
	ExpectError(R"(S5 + 1)", SemanticEID::arithmeticNotSupported, 0);
	ExpectError(R"(1 + S5)", SemanticEID::arithmeticNotSupported, 4);
	ExpectError(R"(S6 + S4)", SemanticEID::typesNotCompatible, 5);
	ExpectError(R"(S6 + 1)", SemanticEID::typesNotCompatible, 5);

	ExpectError(R"(debool(X1) < 1)", SemanticEID::orderingNotSupported, 0);
	ExpectError(R"(1 < debool(X1))", SemanticEID::orderingNotSupported, 4);

	ExpectError(R"(S1=S2)", SemanticEID::typesNotCompatible, 3);
	ExpectError(R"(debool(X1) = 1)", SemanticEID::typesNotCompatible, 13);
	ExpectError(R"(1 = debool(X1))", SemanticEID::typesNotCompatible, 4);
}

TEST_F(UTTypeAuditor, LogicCorrect) {
	ExpectLogic(R"(\neg 1 = 1)");
	ExpectLogic(R"(1!=1)");
	ExpectLogic(R"(1=1 && 1=1)");

	ExpectLogic(R"(\A a \in X1 a=a)");
	ExpectLogic(R"(\A a,b \in X1 a=b)");
	ExpectLogic(R"(\A (a,b) \in S1 a=b)");
	ExpectLogic(R"(\A a \in X1*X1 a=a)");

	ExpectLogic(R"(\A a \in S1 (R{(a1, a2) := a | (a1, a2)} = a))");
	ExpectLogic(R"(\A a \in X1 R{b := a | b}=a)");
}

TEST_F(UTTypeAuditor, LogicErrors) {
	ExpectError(R"(\A a \in X1 a={})", SemanticEID::invalidEqualsEmpty, 12);
	ExpectError(R"(\A a \in X1*X1 a={})", SemanticEID::invalidEqualsEmpty, 15);

	ExpectError(R"(\A (a,a) \in S1 pr2(a) \in X1)", SemanticEID::localShadowing, 6);
	ExpectError(R"(\A (a,b) \in X1 1=1)", SemanticEID::ivalidBinding, 4);
	ExpectError(R"(\A (a,b,c) \in S1 1=1)", SemanticEID::ivalidBinding, 4);

	ExpectError(R"(\A t \in X1 {t \in X1 | t=t} = X1)", SemanticEID::localShadowing, 13);
	ExpectError(R"(\A t \in X1 t=t && t=X1)", SemanticEID::localOutOfScope, 19);
}

TEST_F(UTTypeAuditor, ConstructorsCorrect) {
	SetupConstants();

	ExpectTypification(R"({X1, X1})", "BB(X1)"_t);
	ExpectTypification(R"({1})", "B(Z)"_t);
	ExpectTypification(R"({1, card(X1)})", "B(Z)"_t);
	ExpectTypification(R"({S4, 1})", "B(C1)"_t);
	ExpectTypification(R"({1, S4})", "B(C1)"_t);

	ExpectTypification(R"((X1, X1))", "B(X1)*B(X1)"_t);
	ExpectTypification(R"((1, 2))", "Z*Z"_t);
	ExpectTypification(R"({(1, S4), (S4, 1)})", "B(C1*C1)"_t);
	
	ExpectTypification(R"(D{t \in B(X1) | t=t})", "BB(X1)"_t);
	ExpectTypification(R"(D{t \in Z | t>2})", "B(Z)"_t);
	ExpectTypification(R"(D{t \in S1 | pr2(t)=pr1(t)})", "B(X1*X1)"_t);

	ExpectTypification(R"(R{a := S1 | card(a)<10 | a \union a})", "B(X1*X1)"_t);
	ExpectTypification(R"(R{a := S1 | a \union a})", "B(X1*X1)"_t);
	ExpectTypification(R"(R{a := 1 | a < 10 | a + 1})", "Z"_t);
	ExpectTypification(R"(R{a := 1 | a < S4 | a + S4})", "C1"_t);

	ExpectTypification(R"(I{(a, b) | a \from X1; b := a})", "B(X1*X1)"_t);
	ExpectTypification(R"(I{(a, b) | a \from X1; b := a; 1=1})", "B(X1*X1)"_t);
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

	ExpectError(R"(R{a := S1 | {a}})", SemanticEID::typesNotEqual, 12);
	ExpectError(R"(\A a \in S1 R{(a1, a2) := a | a1} = a)", SemanticEID::typesNotEqual, 30);

	ExpectError(R"(I{(a, b) | a \from X1; b := {a}; a != b})", SemanticEID::typesNotCompatible, 38);
	ExpectError(R"(I{(a, b) | a \from X1; g \in a \setminus X2; b := {a}})", SemanticEID::invalidTypeOperation, 29);
}

TEST_F(UTTypeAuditor, TypedPredicatesCorrect) {
	SetupConstants();

	ExpectLogic(R"(X1=X1)");
	ExpectLogic(R"(X1={})");
	ExpectLogic(R"(Z={})");

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

	ExpectError(R"(X1 \in X1)", SemanticEID::invalidElementPredicat, 7);
	ExpectError(R"(S4 \in C2)", SemanticEID::invalidElementPredicat, 7);
	ExpectError(R"(S5 \in Z)", SemanticEID::invalidElementPredicat, 7);
	ExpectError(R"(S6 \in Z)", SemanticEID::invalidElementPredicat, 7);
	ExpectError(R"((1,2) \in Z)", SemanticEID::invalidElementPredicat, 10);
	ExpectError(R"({1} \in Z)", SemanticEID::invalidElementPredicat, 8);
	ExpectError(R"((1,2) \in Z)", SemanticEID::invalidElementPredicat, 10);

	ExpectError(R"(X1 \subset S2)", SemanticEID::typesNotEqual, 11);

	ExpectError(R"(X1 \notin X1)", SemanticEID::invalidElementPredicat, 10);
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
	ExpectTypification(R"(Pr1(S1))", "B(X1)"_t);
	ExpectTypification(R"(Fi1[X1](S1))", "B(X1*X1)"_t);
	ExpectTypification(R"(Fi1[{1,2,3}](Z*X1))", "B(Z*X1)"_t);
	ExpectTypification(R"(Fi1[{1,2,3}](C1*X1))", "B(C1*X1)"_t);
	ExpectTypification(R"(Pr1,2(S1))", "B(X1*X1)"_t);
	ExpectTypification(R"(bool(X1))", "BB(X1)"_t);
	ExpectTypification(R"(debool({X1}))", "B(X1)"_t);
	ExpectTypification(R"(red(S2))", "B(X1)"_t);
}

TEST_F(UTTypeAuditor, TypedOperationsErrors) {
	SetupConstants();

	ExpectError(R"(X1 \union {})", SemanticEID::emptySetUsage, 10);
	ExpectError(R"(X1 \union S2)", SemanticEID::typesNotEqual, 10);
	ExpectError(R"(S2 \union X1)", SemanticEID::typesNotEqual, 10);

	ExpectError(R"(Pr1(X1))", SemanticEID::invalidProjectionSet, 4);
	ExpectError(R"(Pr3(S1))", SemanticEID::invalidProjectionSet, 4);
	ExpectError(R"(pr1(debool(X1)))", SemanticEID::invalidProjectionTuple, 4);
	ExpectError(R"(Fi1[X1](B(X1)))", SemanticEID::invalidFilterArgumentType, 8);
	ExpectError(R"(Fi1[1](B(X1)))", SemanticEID::invalidFilterArgumentType, 7);
	ExpectError(R"(Fi1[S4](B(X1)))", SemanticEID::invalidFilterArgumentType, 8);
	ExpectError(R"(Fi1[Z](B(X1)))", SemanticEID::invalidFilterArgumentType, 7);
	ExpectError(R"(Fi3[X1](X1*X1))", SemanticEID::invalidFilterArgumentType, 8);
	ExpectError(R"(Fi1[X1](B(X1)*X1))", SemanticEID::typesNotEqual, 4);
	ExpectError(R"(Fi1,2[X1](X1*X1))", SemanticEID::invalidFilterArity, 0);
	ExpectError(R"(\A a \in X1 Fi1[a](B(X1)*X1)=X1)", SemanticEID::typesNotEqual, 16);
	ExpectError(R"(\A a \in X1*X1 Fi1[a](B(X1)*X1)=X1)", SemanticEID::typesNotEqual, 19);
	ExpectError(R"(red(X1))", SemanticEID::invalidReduce, 5);

	ExpectError(R"(\A a \in X1 B(a)=X1)", SemanticEID::invalidBoolean, 14);
	ExpectError(R"(\A a \in S1 B(a)=X1)", SemanticEID::invalidBoolean, 14);
	ExpectError(R"(\A a \in X1 a*a \in S1)", SemanticEID::invalidDecart, 12);
	ExpectError(R"(\A a \in X1 debool(a)=X1)", SemanticEID::invalidDebool, 19);
	ExpectError(R"(\A a \in S1 pr3(a) \in X1)", SemanticEID::invalidProjectionTuple, 16);
}

TEST_F(UTTypeAuditor, TypedFunctions) {
	ExpectTypification(R"(F1[X1, X1])", "B(X1)"_t);
	ExpectTypification(R"(F1[X1 \union X1, X1])", "B(X1)"_t);
	ExpectTypification(R"(F1[Pr1(S1), Pr2(S1)])", "B(X1)"_t);

	ExpectError(R"(F1)", SemanticEID::globalFuncWithoutArgs, 0);
	ExpectError(R"(F1[X1])", SemanticEID::invalidArgsArtity, 3);
	ExpectError(R"(F1[X1, X1, X1])", SemanticEID::invalidArgsArtity, 3);

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
	ExpectError(R"(P1[X1])", SemanticEID::invalidArgsArtity, 3);
	ExpectError(R"(P1[X1, X1, X1])", SemanticEID::invalidArgsArtity, 3);

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
	ExpectTypification(R"(F2[Z, 1])", "B(Z)"_t);
	ExpectTypification(R"(F2[Z, S4])", "B(C1)"_t);
	ExpectTypification(R"(F2[{S4}, 1])", "B(C1)"_t);
	ExpectError(R"(F2[X1, B(X1)])", SemanticEID::invalidArgumentType, 7);

	env.Insert("F3", Typification::Tuple({ Typification("R1"), Typification("R2") }));
	FunctionArguments f3Args{};
	f3Args.emplace_back("a", Typification("R1").Bool());
	f3Args.emplace_back("b", Typification::Tuple({ Typification("R1"), Typification("R2") }).Bool());
	env.data["F3"].arguments = f3Args;

	ExpectTypification(R"(F3[X1, X1*B(X1)])", "X1*B(X1)"_t);
	ExpectError(R"(F3[X1, X1])", SemanticEID::invalidArgumentType, 7);
	ExpectError(R"(F3[X1, X1*X1*X1])", SemanticEID::invalidArgumentType, 7);
	ExpectError(R"(F3[X1, (X1*X1)*X1])", SemanticEID::invalidArgumentType, 7);
}

TEST_F(UTTypeAuditor, TemplatedFunctionsNesting) {
	env.Insert("F2", Typification("R1").Bool());
	FunctionArguments f2Args{};
	f2Args.emplace_back(TypedID{ "a", Typification("R1") });
	f2Args.emplace_back(TypedID{ "b", Typification::Tuple({ Typification("R1"), Typification("R2") }) });
	env.data["F2"].arguments = f2Args;

	ExpectTypification(R"(F3:==[a \in R2, b \in R2*R1] F2[a, b])", Typification("R2").Bool());
	ExpectTypification(R"(F3:==[a \in R3, b \in R3*R4] F2[a, b])", Typification("R3").Bool());
	ExpectError(R"(F3:==[a \in R2, b \in R1*R2] F2[a, b])", SemanticEID::invalidArgumentType, 35);
	ExpectError(R"(F3:==[a \in R3, b \in R1*R2] F2[a, b])", SemanticEID::invalidArgumentType, 35);
}

TEST_F(UTTypeAuditor, ErrorPopup) {
	ExpectError(R"(1<card(X42))", SemanticEID::globalNotTyped, 7);
	ExpectError(R"(card(X42)<1)", SemanticEID::globalNotTyped, 5);
	ExpectError(R"(1+card(X42))", SemanticEID::globalNotTyped, 7);
	ExpectError(R"(card(X42)+1)", SemanticEID::globalNotTyped, 5);

	ExpectError(R"(X42 \in X1)", SemanticEID::globalNotTyped, 0);
	ExpectError(R"(X1 \in X42)", SemanticEID::globalNotTyped, 7);
	ExpectError(R"(X42=X1)", SemanticEID::globalNotTyped, 0);
	ExpectError(R"(X42={})", SemanticEID::globalNotTyped, 0);
	ExpectError(R"(X1=X42)", SemanticEID::globalNotTyped, 3);
	ExpectError(R"(1=1 && X1=X42)", SemanticEID::globalNotTyped, 10);

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

	ExpectError(R"(S4:==X42)", SemanticEID::globalNotTyped, 5);
	ExpectError(R"(D1:==X42)", SemanticEID::globalNotTyped, 5);
	ExpectError(R"(A1:==X42=X42)", SemanticEID::globalNotTyped, 5);

	ExpectError(R"(D{t \in X42 | t=t})", SemanticEID::globalNotTyped, 8);
	ExpectError(R"(I{(t, X42) | t \from X1})", SemanticEID::globalNotTyped, 6);
	ExpectError(R"(I{t | t \from X42})", SemanticEID::globalNotTyped, 14);
	ExpectError(R"(R{t:=X42 | t})", SemanticEID::globalNotTyped, 5);
	ExpectError(R"(R{t:=X1 | t \union X42})", SemanticEID::globalNotTyped, 19);
	ExpectError(R"(R{t:=X1 | 1=1 | t \union X42})", SemanticEID::globalNotTyped, 25);
	ExpectError(R"(R{t:=X1 | X42=X42 | t \union X1})", SemanticEID::globalNotTyped, 10);

	ExpectError(R"(F42[X1])", SemanticEID::globalNotTyped, 0);
	ExpectError(R"(F42:==[a \in X50] {a})", SemanticEID::globalNotTyped, 13);
	ExpectError(R"(F1:==[a \in X1, a \in X1] {a})", SemanticEID::localShadowing, 16);

	ExpectError(R"(F1[F21[X1], X1])", SemanticEID::globalNotTyped, 3);
	ExpectError(R"(F1[X1, F21[X1]])", SemanticEID::globalNotTyped, 7);
}

TEST_F(UTTypeAuditor, WarningLocalNotUsed) {
	const auto input = R"(D{t \in X1 | \A a \in X1 1=1})";
	ASSERT_TRUE(parser.Parse(lexer(input).Stream()));
	ASSERT_TRUE(analyse.CheckType(parser.AST()));
	EXPECT_EQ(log.Count(ErrorStatus::WARNING), 2); // Note: 1 warning to 'a' and 1 warning for 't'
	EXPECT_EQ(begin(log.All())->eid, static_cast<uint32_t>(SemanticEID::localNotUsed));
}

TEST_F(UTTypeAuditor, NoWarningAfterError) {
	const auto input = R"(D{t \in X1 | \A a \in X1 \A b \in red(X1) {a,b} = t})";
	ASSERT_TRUE(parser.Parse(lexer(input).Stream()));
	ASSERT_FALSE(analyse.CheckType(parser.AST()));
	EXPECT_EQ(log.Count(ErrorStatus::WARNING), 0);
	EXPECT_EQ(log.Count(ErrorStatus::CRITICAL), 1);
}

TEST_F(UTTypeAuditor, WarningLocalRedeclared) {
	const auto input = R"(D{t \in X1 | \A a \in X1 a=t && \A a \in X1 a=t})";
	ASSERT_TRUE(parser.Parse(lexer(input).Stream()));
	ASSERT_TRUE(analyse.CheckType(parser.AST()));
	EXPECT_EQ(log.Count(ErrorStatus::WARNING), 1);
	EXPECT_EQ(begin(log.All())->eid, static_cast<uint32_t>(SemanticEID::localDoubleDeclare));
}