#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ErrorLogger.h"

using ccl::operator""_c17;

class UTRSParser: public ::testing::Test {
protected:
	using ParseEID = ccl::rslang::ParseEID;
	using LexerEID = ccl::rslang::LexerEID;
	using RSParser = ccl::rslang::RSParser;
	using AsciiLexer = ccl::rslang::AsciiLexer;
	using ErrorLogger = ccl::rslang::ErrorLogger;

protected:
	ErrorLogger log{};
	AsciiLexer lexer{ log.SendReporter() };
	RSParser parser{ log.SendReporter() };

	void ExpectNoWarnings(const std::string& input);
	void ExpectError(const std::string& input);
	void ExpectError(const std::string& input, ParseEID errorCode);
	void ExpectError(const std::string& input, ParseEID errorCode, ccl::StrPos pos);
	void ExpectAST(const std::string& input, const std::string& ast);
	void TestAllBinaryCombos(const std::string& op);
};

void UTRSParser::ExpectError(const std::string& input) {
	log.Clear();
	ASSERT_FALSE(parser.Parse(lexer(input).Stream())) << input;
}

void UTRSParser::ExpectError(const std::string& input, const ParseEID errorCode, const ccl::StrPos pos) {
	ExpectError(input, errorCode);
	const auto errorPos = log.FirstErrorPos();
	EXPECT_EQ(errorPos, pos) << input;
}

void UTRSParser::ExpectError(const std::string& input, const ParseEID errorCode) {
	ExpectError(input);
	EXPECT_EQ(std::begin(log.All())->eid, static_cast<uint32_t>(errorCode)) << input;
}

void UTRSParser::ExpectNoWarnings(const std::string& input) {
	log.Clear();
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
	EXPECT_EQ(ssize(log.All()), 0) << input;
}

void UTRSParser::ExpectAST(const std::string& input, const std::string& ast) {
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
	EXPECT_EQ(ccl::rslang::AST2String::Apply(parser.AST()), ast) << input;
}

void UTRSParser::TestAllBinaryCombos(const std::string& op) {
	const static std::vector<std::string_view> operands = { "a", "1", "X1", "{}" };
	for (const auto& el1 : operands) {
		for (const auto& el2 : operands) {
			std::string test{ el1 };
			test += op;
			test += el2;
			ExpectNoWarnings(test);
		}
	}
}

TEST_F(UTRSParser, EmptyStr) {
	EXPECT_FALSE(parser.Parse(lexer({}).Stream()));
}

TEST_F(UTRSParser, GlobalDeclCorrect) {
	ExpectNoWarnings(R"(X1:==)");
	ExpectNoWarnings(R"(C1:==)");
	ExpectNoWarnings(R"(S1::=B(X1))");
	ExpectNoWarnings(R"(S1::=1)");
	ExpectNoWarnings(R"(S1::=Z)");
	ExpectNoWarnings(R"(S1:==X1)");
	ExpectNoWarnings(R"(S1:=={})");
	ExpectNoWarnings(R"(S1:==X1*X1)");
	ExpectNoWarnings(R"(S1:==B(C1))");
	ExpectNoWarnings(R"(S1:==B(D1))");
	ExpectNoWarnings(R"(S1:==B(X1*(X1*X1)))");
	ExpectNoWarnings(R"(D1:==X1 \setminus X1)");
	ExpectNoWarnings(R"(A1:==1=1)");
	ExpectNoWarnings(R"(T1:==1=1)");
	ExpectNoWarnings(R"(F1:==[a \in X1] X1 \setminus a)");
	ExpectNoWarnings(R"(F1:==[a \in R1] {a})");
	ExpectNoWarnings(R"(F1:==[a \in B(D1)] X1 \setminus a)");
	ExpectNoWarnings(R"(F1:==[a \in D1 \setminus D1] X1 \setminus a)");
	ExpectNoWarnings(R"(P1:==[a \in D1 \setminus D1] 1=1)");
	ExpectNoWarnings(R"(F1:==[a \in B(X1)] card(a))");
}

TEST_F(UTRSParser, GlobalDeclAST) {
	ExpectAST(R"(X1:==)", u8"[:==[X1]]"_c17);
	ExpectAST(R"(S1::=B(X1))", u8"[::=[S1][\u212C[X1]]]"_c17);
	ExpectAST(R"(D1:==X1 \setminus X2)", u8"[:==[D1][\\[X1][X2]]]"_c17);
	ExpectAST(R"(A1:==1=1)", u8"[:==[A1][=[1][1]]]"_c17);
	ExpectAST(R"(F1:==[a \in X1] X1 \setminus a)", 
						u8"[:==[F1][ARGS[ARG[a][X1]]][\\[X1][a]]]"_c17);
	ExpectAST(R"(P1:==[a \in X1] 1=1)", 
						u8"[:==[P1][ARGS[ARG[a][X1]]][=[1][1]]]"_c17);
	ExpectAST(R"(F1:==[a \in X1, b \in X1] {b,a})",
						u8"[:==[F1][ARGS[ARG[a][X1]][ARG[b][X1]]][SET[b][a]]]"_c17);
	ExpectAST(R"(F1:==[a \in R1, b \in B(X1*R1)] {a} \setminus Pr2(b))",
						u8"[:==[F1][ARGS[ARG[a][R1]][ARG[b][\u212C[\u00D7[X1][R1]]]]][\\[SET[a]][Pr2[b]]]]"_c17);
}

TEST_F(UTRSParser, GlobalDeclErrors) {
	ExpectError(R"(F1:==[] X1 \setminus X1)", ParseEID::expectedDeclaration, 6);
	ExpectError(R"(F1:==[a] X1 \setminus a)", ParseEID::expectedDeclaration, 7);
	ExpectError(R"(F1:==[a \subseteq X1] X1 \setminus a)", ParseEID::expectedDeclaration, 8);
	ExpectError(R"(S1::=1=1)", ParseEID::syntax);
	ExpectError(R"(F1:==)", ParseEID::syntax);
	ExpectError(R"(F1:==1=1)", ParseEID::syntax);
	ExpectError(R"(F1:==X1)", ParseEID::syntax);
}

TEST_F(UTRSParser, LogicPredicatesCorrect) {
	TestAllBinaryCombos(R"( = )");
	TestAllBinaryCombos(R"( != )");
	TestAllBinaryCombos(R"( > )");
	TestAllBinaryCombos(R"( >= )");
	TestAllBinaryCombos(R"( <= )");
	TestAllBinaryCombos(R"( < )");

	TestAllBinaryCombos(R"( \in )");
	TestAllBinaryCombos(R"( \notin )");
	TestAllBinaryCombos(R"( \subset )");
	TestAllBinaryCombos(R"( \subseteq )");
	TestAllBinaryCombos(R"( \notsubset )");

	ExpectNoWarnings(R"(P1[X1])");
}

TEST_F(UTRSParser, LogicPredicatesAST) {
	ExpectAST(R"(1 = 2)", u8"[=[1][2]]"_c17);
	ExpectAST(R"(1 < X1)", u8"[<[1][X1]]"_c17);
	ExpectAST(R"(1 > 2)", u8"[>[1][2]]"_c17);
	ExpectAST(R"(1 < 2)", u8"[<[1][2]]"_c17);
	ExpectAST(R"(X1 != a)", u8"[\u2260[X1][a]]"_c17);
	ExpectAST(R"(P1[a,b])", u8"[CALL[P1][a][b]]"_c17);
}

TEST_F(UTRSParser, LogicPredicatesErrors) {
	ExpectError(R"(1=1=1)", ParseEID::syntax, 3);
	ExpectError(R"(1>1=1)", ParseEID::syntax, 3);
	ExpectError(R"(P1[])", ParseEID::syntax, 3);
}

TEST_F(UTRSParser, LogicOperatorsCorrect) {
	ExpectNoWarnings(R"(\neg 1=1)");
	ExpectNoWarnings(R"(\neg (1=1))");
	ExpectNoWarnings(R"(1=1 && 1=1)");
	ExpectNoWarnings(R"(1=1 || 1=1)");
	ExpectNoWarnings(R"(1=1 => 1=1)");
	ExpectNoWarnings(R"(1=1 <=> 1=1)");

	ExpectNoWarnings(R"(1=1 && 1=1)");
	ExpectNoWarnings(R"(1=1 && 1=1 && 1=1)");
	ExpectNoWarnings(R"(1=1 || 1=1 && 1=1)");
	ExpectNoWarnings(R"((1=1 || 1=1) && 1=1)");
	ExpectNoWarnings(R"(1=1 || (1=1 && 1=1))");
}

TEST_F(UTRSParser, LogicOperatorsAST) {
	ExpectAST(R"(\neg 1=2)", u8"[\u00AC[=[1][2]]]"_c17);

	ExpectAST(R"(1=1 && 2=2 && 3=3)", "[&[&[=[1][1]][=[2][2]]][=[3][3]]]");
	ExpectAST(R"(1=1 || 2=2 || 3=3)", u8"[\u2228[\u2228[=[1][1]][=[2][2]]][=[3][3]]]"_c17);
	ExpectAST(R"(1=1 => 2=2 => 3=3)", u8"[\u21D2[\u21D2[=[1][1]][=[2][2]]][=[3][3]]]"_c17);
	ExpectAST(R"(1=1 <=> 2=2 <=> 3=3)", u8"[\u21D4[\u21D4[=[1][1]][=[2][2]]][=[3][3]]]"_c17);

	ExpectAST(R"((1=1 && 2=2) && 3=3)", "[&[&[=[1][1]][=[2][2]]][=[3][3]]]");
	ExpectAST(R"(1=1 && (2=2 && 3=3))", "[&[=[1][1]][&[=[2][2]][=[3][3]]]]");
	ExpectAST(R"(1=1 || 2=2 && 3=3)", u8"[\u2228[=[1][1]][&[=[2][2]][=[3][3]]]]"_c17);
	ExpectAST(R"(1=1 && 2=2 || 3=3 => 4=4 <=> 5=5)",
						u8"[\u21D4[\u21D2[\u2228[&[=[1][1]][=[2][2]]][=[3][3]]][=[4][4]]][=[5][5]]]"_c17);
}

TEST_F(UTRSParser, LogicOperatorsErrors) {
	ExpectError(R"(1=1 && 1)", ParseEID::syntax, 8);
	ExpectError(R"(1=1 = 1=1)", ParseEID::syntax, 4);
	ExpectError(R"(1=1 == 1=1)", ParseEID::syntax, 4);
	ExpectError(R"(1=1 && X1)", ParseEID::syntax, 9);
	ExpectError(R"(1=1 && \neg X1)", ParseEID::syntax, 14);
	ExpectError(R"(1=1 || X1)", ParseEID::syntax, 9);
	ExpectError(R"(1=1 => X1)", ParseEID::syntax, 9);
	ExpectError(R"(1=1 <=> X1)", ParseEID::syntax, 10);
}

TEST_F(UTRSParser, LogicQuantifiedCorrect) {
	ExpectNoWarnings(R"(\A a \in X1 P1[a])");
	ExpectNoWarnings(R"(\A a \in X1 1=1)");
	ExpectNoWarnings(R"(\E a \in X1 1=1)");
	ExpectNoWarnings(R"(\A a \in X1 (1=1))");

	ExpectNoWarnings(R"(\A a \in X1 1=1 || 1=1 && 1=1)");
	ExpectNoWarnings(R"(\A a \in X1 (1=1 || 1=1 && 1=1))");

	ExpectNoWarnings(R"(\A a \in X1 \A b \in X1 1=1)");
	ExpectNoWarnings(R"(\A a,b,c \in X1 1=1)");
	ExpectNoWarnings(R"(\A (a,b,c) \in X1 1=1)");
}

TEST_F(UTRSParser, LogicQuantifiedAST) {
	ExpectAST(R"(\A a \in X1 1=2)", u8"[\u2200[a][X1][=[1][2]]]"_c17);
	ExpectAST(R"(\A a,b \in X1 1=2)", u8"[\u2200[ENUM_DECLARATION[a][b]][X1][=[1][2]]]"_c17);
	ExpectAST(R"(\A a \in X1 1=2 && 3=4)", u8"[&[\u2200[a][X1][=[1][2]]][=[3][4]]]"_c17);
	ExpectAST(R"(\A a \in X1 (1=2 && 3=4))", u8"[\u2200[a][X1][&[=[1][2]][=[3][4]]]]"_c17);
}

TEST_F(UTRSParser, LogicQuantifiedErrors) {
	ExpectError(R"(\A a \in X1, \A b \in X1 1=1)", ParseEID::invalidQuantifier, 11);
	ExpectError(R"(\A ()", ParseEID::invalidQuantifier, 4);
	ExpectError(R"(\A (a,X1) \in X1 1=1)", ParseEID::expectedLocal, 6);
	ExpectError(R"(\A a \notsubset X1 1=1)", ParseEID::invalidQuantifier, 5);
	ExpectError(R"(\A a \notin X1 1=1)", ParseEID::invalidQuantifier, 5);
	ExpectError(R"(\A a \subset X1 1=2)", ParseEID::invalidQuantifier, 5);
	ExpectError(R"(\A a \subseteq X1 1=2)", ParseEID::invalidQuantifier, 5);
	ExpectError(R"(\A a,1 \in X1 1=1)", ParseEID::expectedLocal, 5);
	ExpectError(R"(\A a,{} \in X1 1=1)", ParseEID::expectedLocal, 5);
	ExpectError(R"(\A (a,1) \in X1 1=1)", ParseEID::expectedLocal, 6);
}

TEST_F(UTRSParser, Literals) {
	ExpectNoWarnings(R"(0)");
	ExpectNoWarnings(R"(1)");
	ExpectNoWarnings(R"(16785)");
	ExpectNoWarnings(R"(Z)");
	ExpectNoWarnings(R"({})");

	ExpectError(R"(-1)");
	ExpectError(R"(1.1234)");
	ExpectError(R"(1,1234)");
	ExpectError(R"("abc")");
}

TEST_F(UTRSParser, Identifiers) {
	ExpectNoWarnings(R"(a)");
	ExpectNoWarnings(R"(X1)");
	ExpectNoWarnings(R"(S1)");
	ExpectNoWarnings(R"(D1)");
	ExpectNoWarnings(R"(A1)");
	ExpectNoWarnings(R"(hello_world)");
}

TEST_F(UTRSParser, TermOperatorsCorrect) {
	TestAllBinaryCombos(R"( + )");
	TestAllBinaryCombos(R"( - )");
	TestAllBinaryCombos(R"( \multiply )");

	ExpectNoWarnings(R"(1+2+3)");
	ExpectNoWarnings(R"((1+2)+3)");
	ExpectNoWarnings(R"(1+(2+3))");
	ExpectNoWarnings(R"(1 \multiply 2+3)");
	ExpectNoWarnings(R"(1 \multiply (2+3))");
	ExpectNoWarnings(R"(1 \multiply (2-3))");
	ExpectNoWarnings(R"((2-3) \multiply 1)");
	ExpectNoWarnings(R"(2-(3 \multiply 1))");

	TestAllBinaryCombos(R"( \setminus )");
	TestAllBinaryCombos(R"( \union )");
	TestAllBinaryCombos(R"( \intersect )");
	TestAllBinaryCombos(R"( \symmdiff )");
	TestAllBinaryCombos(R"( * )");
}

TEST_F(UTRSParser, TermOperatorsAST) {
	ExpectAST(R"(1+2+3)", "[+[+[1][2]][3]]");
	ExpectAST(R"((1+2)+3)", "[+[+[1][2]][3]]");
	ExpectAST(R"(1+(2+3))", "[+[1][+[2][3]]]");
	ExpectAST(R"(1+2-3)", "[-[+[1][2]][3]]");
	ExpectAST(R"(1-2-3)", "[-[-[1][2]][3]]");
	ExpectAST(R"(1+(2-3))", "[+[1][-[2][3]]]");
	ExpectAST(R"(1 \multiply 2+3)", "[+[*[1][2]][3]]");
	ExpectAST(R"(1+2 \multiply 3)", "[+[1][*[2][3]]]");
	ExpectAST(R"(1 \multiply 2 \multiply 3)", "[*[*[1][2]][3]]");

	ExpectAST(R"(a \setminus b \setminus c)", "[\\[\\[a][b]][c]]");
	ExpectAST(R"((a \setminus b) \setminus c)", "[\\[\\[a][b]][c]]");
	ExpectAST(R"(a \setminus (b \setminus c))", "[\\[a][\\[b][c]]]");
	ExpectAST(R"(((a \union b) \intersect (c \setminus d)) \symmdiff (e * f))",
						u8"[\u2206[\u2229[\u222A[a][b]][\\[c][d]]][\u00D7[e][f]]]"_c17);
}

TEST_F(UTRSParser, TermOperatorsErrors) {
	ExpectError(R"(1++)", ParseEID::syntax, 2);
	ExpectError(R"(1--)", ParseEID::syntax, 2);

	ExpectError(R"((-2))", ParseEID::syntax, 1);
	ExpectError(R"(1+-2)", ParseEID::syntax, 2);
	ExpectError(R"(1+(-2))", ParseEID::syntax, 3);

	ExpectError(R"(a \setminus (X1))", ParseEID::syntax, 15);
	ExpectError(R"(X1 \setminus (X1;a))", ParseEID::syntax, 16);
	ExpectError(R"(X1 \setminus (1))", ParseEID::syntax, 15);
}

TEST_F(UTRSParser, TermTextOperationsCorrect) {
	ExpectNoWarnings(R"(card(X1))");
	ExpectNoWarnings(R"(card(1))");
	ExpectNoWarnings(R"(card(a))");
	ExpectNoWarnings(R"(card({}))");
	ExpectNoWarnings(R"(card(Z))");

	ExpectNoWarnings(R"(Pr1(a))");
	ExpectNoWarnings(R"(pr1(a))");
	ExpectNoWarnings(R"(Fi1[b](a))");
	ExpectNoWarnings(R"(Fi1,2[b,c](d))");
	ExpectNoWarnings(R"(bool(a))");
	ExpectNoWarnings(R"(debool(a))");
	ExpectNoWarnings(R"(red(a))");
}

TEST_F(UTRSParser, TermTextOperationsAST) {
	ExpectAST(R"(card(X1))", u8"[card[X1]]"_c17);
	ExpectAST(R"(Pr2(a))", u8"[Pr2[a]]"_c17);
	ExpectAST(R"(Fi1,2[b](a))", u8"[Fi1,2[b][a]]"_c17);
}

TEST_F(UTRSParser, TermTextOperationsErrors) {
	ExpectError(R"(Pr1,b(a))", ParseEID::syntax, 3);
	ExpectError(R"(Fi1,b[c](a))", ParseEID::syntax, 3);
}

TEST_F(UTRSParser, TermConstructorsCorrect) {
	ExpectNoWarnings(R"(B(a))");
	ExpectNoWarnings(R"(B(a*b*(c*d)))");
	ExpectNoWarnings(R"(BB(a))");
	ExpectNoWarnings(R"(B(B(a)))");

	ExpectNoWarnings(R"((a, b, c))");
	ExpectNoWarnings(R"({a, b, c})");
	ExpectNoWarnings(R"({a})");
	ExpectNoWarnings(R"({(a,b)})");
	ExpectNoWarnings(R"({(a,b),(b,c)})");

	ExpectNoWarnings(R"(D{a \in X1 | 1=1})");
	ExpectNoWarnings(R"(D{(a,b) \in X1 | 1=1})");
	ExpectNoWarnings(R"({a \in X1 | 1=1})");

	ExpectNoWarnings(R"(R{a := S1 | card(a)<10 | a \setminus a})");
	ExpectNoWarnings(R"(R{a := S1 | a \setminus a})");
	ExpectNoWarnings(R"(R{(a,b) := S1 | (b,a)})");

	ExpectNoWarnings(R"(I{(a, b) | a \from X1; b := a})");
	ExpectNoWarnings(R"(I{(b, a) | a \from X1; b := a})");
	ExpectNoWarnings(R"(I{ red(b) | a \from X1; b := a})");
	ExpectNoWarnings(R"(I{(a, b) | a \from X1; b := a; (a,b) \in S1})");
}

TEST_F(UTRSParser, TermConstructorsAST) {
	ExpectAST(R"(B(a))", u8"[\u212C[a]]"_c17);

	ExpectAST(R"((a,b,c))", u8"[TUPLE[a][b][c]]"_c17);
	ExpectAST(R"({a,b,c})", u8"[SET[a][b][c]]"_c17);
	ExpectAST(R"({{a, b}, {c, d}})", u8"[SET[SET[a][b]][SET[c][d]]]"_c17);
	ExpectAST(R"({a})", u8"[SET[a]]"_c17);

	ExpectAST(R"(D{a \in X1 | 1=2})",
						u8"[DECLARATIVE[a][X1][=[1][2]]]"_c17);
	ExpectAST(R"(D{(a,b) \in X1 | 1=2})",
						u8"[DECLARATIVE[TUPLE_DECLARATION[a][b]][X1][=[1][2]]]"_c17);

	ExpectAST(R"(R{a := S1 | card(a)<10 | a \setminus a})",
						u8"[REC_FULL[a][S1][<[card[a]][10]][\\[a][a]]]"_c17);
	ExpectAST(R"(R{a := S1 | a \setminus a})",
						u8"[REC_SHORT[a][S1][\\[a][a]]]"_c17);
	ExpectAST(R"(R{(a,b) := S1 | (a \setminus a, b)})",
						u8"[REC_SHORT[TUPLE_DECLARATION[a][b]][S1][TUPLE[\\[a][a]][b]]]"_c17);

	ExpectAST(R"(I{(a, b) | a \from X1; b := a})",
						u8"[IMPERATIVE[TUPLE[a][b]][IDECLARE[a][X1]][IASSIGN[b][a]]]"_c17);
	ExpectAST(R"(I{(a, b) | a \from X1; b := a; (a,b) \in S1})",
						u8"[IMPERATIVE[TUPLE[a][b]][IDECLARE[a][X1]][IASSIGN[b][a]][ICHECK[\u2208[TUPLE[a][b]][S1]]]]"_c17);
}

TEST_F(UTRSParser, TermConstructorsErrors) {
	ExpectError(R"({(a;b) \in X1 | 1=1})", ParseEID::syntax, 3);

	ExpectError(R"(D{a \in X1 | 1=1 a)", ParseEID::missingCurlyBrace, 17);
	ExpectError(R"(D{a \subset X1 | 1=1})", ParseEID::syntax, 4);
	ExpectError(R"(D{a \subseteq X1 | 1=1})", ParseEID::syntax, 4);
	ExpectError(R"(D{a \notsubset X1 | 1=1})", ParseEID::syntax, 4);
	ExpectError(R"(D{a \notin X1 | 1=1})", ParseEID::syntax, 4);

	ExpectError(R"(D{(a,1) \in X1 | 1=1})", ParseEID::expectedLocal, 5);
	ExpectError(R"({(a,b) \in X1 | 1=1})", ParseEID::syntax, 7);
	ExpectError(R"(D{a})", ParseEID::syntax, 3);

	ExpectError(R"(R{a := S1})", ParseEID::syntax, 9);

	ExpectError(R"(I{(a, b) | 1})", ParseEID::syntax, 12);
	ExpectError(R"(I{(a, b)})", ParseEID::syntax, 8);
}

TEST_F(UTRSParser, TermFunctionCorrect) {
	ExpectNoWarnings(R"(F1[a])");
	ExpectNoWarnings(R"(F1[1])");
	ExpectNoWarnings(R"(F1[{}])");
	ExpectNoWarnings(R"(F1[Z])");
	ExpectNoWarnings(R"(F1[a,b,c])");
}

TEST_F(UTRSParser, TermFunctionAST) {
	ExpectAST(R"(F1[a])", u8"[CALL[F1][a]]"_c17);
	ExpectAST(R"(F1[a,b])", u8"[CALL[F1][a][b]]"_c17);
}

TEST_F(UTRSParser, TermFunctionErrors) {
	ExpectError(R"(F1())", ParseEID::syntax, 2);
	ExpectError(R"(F1{})", ParseEID::syntax, 2);
	ExpectError(R"(F1[)", ParseEID::syntax, 3);
	ExpectError(R"(F1[])", ParseEID::syntax, 3);
	ExpectError(R"(X1[a])", ParseEID::syntax, 2);
}

TEST_F(UTRSParser, ExcessiveBrackets) {
	ExpectError(R"(1+(1))", ParseEID::syntax);
	ExpectError(R"(1+((1)))", ParseEID::syntax);
	ExpectError(R"(1+(1))", ParseEID::syntax);
	ExpectError(R"(1+(card(X1)))", ParseEID::syntax);

	ExpectError(R"(1=1 && ((1=1)))", ParseEID::syntax, 13);
	ExpectError(R"(1=1 && \A a \in X1(\A b \in X1 1=1))", ParseEID::invalidQuantifier, 34);
	ExpectError(R"(1=1 && \neg(\A a \in X1 1=1))", ParseEID::syntax, 27);

	ExpectError(R"(a \setminus (red(X1)))", ParseEID::syntax, 20);
}

TEST_F(UTRSParser, ComplexExpressions) {
	ExpectNoWarnings(R"(1+2 = 1 \multiply 3)");
	ExpectNoWarnings(R"(X1 \setminus X1 = a \setminus b)");
	ExpectNoWarnings(R"(card(X1)+1)");
	ExpectNoWarnings(R"(a \setminus (X1, 1))");
	ExpectNoWarnings(R"(F1[{(a,b)}])");
}