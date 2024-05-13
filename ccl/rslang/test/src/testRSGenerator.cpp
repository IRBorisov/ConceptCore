#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/RSGenerator.h"
#include "ccl/rslang/Parser.h"
#include "ccl/rslang/Literals.h"

using ccl::operator""_c17;

class UTRSGenerator: public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using Generator = ccl::rslang::Generator;
  using TypedID = ccl::rslang::TypedID;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using AST2String = ccl::rslang::AST2String;
  using Parser = ccl::rslang::Parser;
  using Syntax = ccl::rslang::Syntax;

protected:
  RSEnvironment env{};

  void SetupEnvironment();
  void ExpectASTGeneration(const std::string& input, const std::string& output) const;
};

void UTRSGenerator::SetupEnvironment() {
  env.InsertBase("X1");
  env.data["S1"].type = "B(X1)"_t;
}

void UTRSGenerator::ExpectASTGeneration(const std::string& input, const std::string& output) const {
  Parser parser{};
  ASSERT_TRUE(parser.Parse(input, Syntax::MATH)) << input;
  const auto ast = parser.ExtractAST();
  const auto generated = Generator::FromTree(*ast);
  EXPECT_EQ(generated, output) << input;

  ASSERT_TRUE(parser.Parse(generated, Syntax::MATH)) << generated << " from " << input;
  EXPECT_EQ(AST2String::Apply(*ast), AST2String::Apply(parser.AST())) << input << R"( != )" << generated;
}

TEST_F(UTRSGenerator, GlobalDefinition) {
  EXPECT_EQ(Generator::GlobalDefinition("A1", "1=1", false), "A1:==1=1");
  EXPECT_EQ(Generator::GlobalDefinition("A1", "1=1", true), "A1::=1=1");
  EXPECT_EQ(Generator::GlobalDefinition("abc", "1=1", false), "abc:==1=1");
  EXPECT_EQ(Generator::GlobalDefinition("X1", "", false), "X1:==");
  EXPECT_EQ(Generator::GlobalDefinition("", "1=1", false), ":==1=1");
}

TEST_F(UTRSGenerator, CreatePrefix) {
  EXPECT_EQ(Generator::CreatePrefix({}), "[]");
  EXPECT_EQ(Generator::CreatePrefix(FunctionArguments{ TypedID{ "", "X1"_t } }), u8"[\u2208X1]"_c17);
  EXPECT_EQ(Generator::CreatePrefix(FunctionArguments{ TypedID{ "a", "X1"_t } }), u8"[a\u2208X1]"_c17);
  EXPECT_EQ(Generator::CreatePrefix(FunctionArguments{ TypedID{ "a", "X1"_t }, TypedID{ "b", "X2"_t } }),
            u8"[a\u2208X1, b\u2208X2]"_c17);
}

TEST_F(UTRSGenerator, ExtractPrefix) {
  EXPECT_EQ(Generator::ExtractPrefix(""), "");
  EXPECT_EQ(Generator::ExtractPrefix("a"), "");
  EXPECT_EQ(Generator::ExtractPrefix("[]"), "[]");
  EXPECT_EQ(Generator::ExtractPrefix(" []"), " []");
  EXPECT_EQ(Generator::ExtractPrefix("[] 1=1"), "[]");
  EXPECT_EQ(Generator::ExtractPrefix("[a] 1=1"), "[a]");
  EXPECT_EQ(Generator::ExtractPrefix("[a]] 1=1"), "[a]");
  EXPECT_EQ(Generator::ExtractPrefix("[[a] 1=1"), "");
  EXPECT_EQ(Generator::ExtractPrefix("[a] F1[a]"), "[a]");
}

TEST_F(UTRSGenerator, FromASTGlobal) {
  ExpectASTGeneration(
    "X1:== ",
    "X1:=="
  );
  ExpectASTGeneration(
    "D1:==X1\\X2",
    "D1:==X1\\X2"
  );
  ExpectASTGeneration(
    u8"F1:==[a\u2208X1]a \\ a"_c17,
    u8"F1:==[a\u2208X1] a\\a"_c17
  );
}

TEST_F(UTRSGenerator, FromASTInteger) {
  ExpectASTGeneration("42", "42");
  ExpectASTGeneration("Z", "Z");

  ExpectASTGeneration("(1+2)+3", "1+2+3");
  ExpectASTGeneration("1 + (2+3)", "1+(2+3)");
  ExpectASTGeneration("1 - (2-3)", "1-(2-3)");
  ExpectASTGeneration("1 +2*3", "1+2*3");
  ExpectASTGeneration("(1+2) * 3", "(1+2)*3");

  ExpectASTGeneration(
    "1 = 2",
    "1=2"
  );
  ExpectASTGeneration(
    u8"1 \u2260 2"_c17,
    u8"1\u22602"_c17
  );
  ExpectASTGeneration(
    "1 > 2",
    "1>2"
  );
  ExpectASTGeneration(
    "1 < 2",
    "1<2"
  );
  ExpectASTGeneration(
    u8"1 \u2264 2"_c17,
    u8"1\u22642"_c17
  );
  ExpectASTGeneration(
    u8"1 \u2265 2"_c17,
    u8"1\u22652"_c17
  );

  ExpectASTGeneration("1+2=3", "1+2=3");
  ExpectASTGeneration("1-2=3", "1-2=3");
  ExpectASTGeneration("1*2=3", "1*2=3");

  ExpectASTGeneration("card( X1 )", "card(X1)");
  ExpectASTGeneration("card(X1)", "card(X1)");
}

TEST_F(UTRSGenerator, FromASTLogic) {
  ExpectASTGeneration(
    u8"\u00AC (1=2 & 3=4)"_c17,
    u8"\u00AC(1=2 & 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u00AC (1=2 \u2228 3=4)"_c17,
    u8"\u00AC(1=2 \u2228 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u00AC (1=2 \u21D2 3=4)"_c17,
    u8"\u00AC(1=2 \u21D2 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u00AC (1=2 \u21D4 3=4)"_c17,
    u8"\u00AC(1=2 \u21D4 3=4)"_c17
  );

  ExpectASTGeneration(
    "1=2 & (3=4 & 5=6)",
    "1=2 & (3=4 & 5=6)"
  );
  ExpectASTGeneration(
    u8"1=2 \u2228 (3=4 \u2228 5=6)"_c17,
    u8"1=2 \u2228 (3=4 \u2228 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 \u21D2 (3=4 \u21D2 5=6)"_c17,
    u8"1=2 \u21D2 (3=4 \u21D2 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 \u21D4 (3=4 \u21D4 5=6)"_c17,
    u8"1=2 \u21D4 (3=4 \u21D4 5=6)"_c17
  );

  ExpectASTGeneration(
    u8"1=2 & (3=4 \u2228 5=6)"_c17,
    u8"1=2 & (3=4 \u2228 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 & (3=4 \u21D2 5=6)"_c17,
    u8"1=2 & (3=4 \u21D2 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 & (3=4 \u21D4 5=6)"_c17,
    u8"1=2 & (3=4 \u21D4 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 \u2228 (3=4 \u21D2 5=6)"_c17,
    u8"1=2 \u2228 (3=4 \u21D2 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 \u2228 (3=4 \u21D4 5=6)"_c17,
    u8"1=2 \u2228 (3=4 \u21D4 5=6)"_c17
  );
  ExpectASTGeneration(
    u8"1=2 \u21D2 (3=4 \u21D4 5=6)"_c17,
    u8"1=2 \u21D2 (3=4 \u21D4 5=6)"_c17
  );
}

TEST_F(UTRSGenerator, FromASTQuantifier) {
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 (1=2)"_c17,
    u8"\u2200a\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2203a\u2208X1 (1=2)"_c17,
    u8"\u2203a\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a,b\u2208X1 (1=2)"_c17,
    u8"\u2200a, b\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 1=2 & 3=4"_c17,
    u8"\u2200a\u2208X1 1=2 & 3=4"_c17
  );
  ExpectASTGeneration(
    u8"\u2200b\u2208X1\u2200a\u2208X1 1=2"_c17,
    u8"\u2200b\u2208X1 \u2200a\u2208X1 1=2"_c17
  );

  ExpectASTGeneration(
    u8"\u00AC\u2200a\u2208X1 1=2"_c17,
    u8"\u00AC\u2200a\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 \u00AC1=2"_c17,
    u8"\u2200a\u2208X1 \u00AC1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 (1=2 & 3=4)"_c17,
    u8"\u2200a\u2208X1 (1=2 & 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u2203a\u2208X1 (1=2 & 3=4)"_c17,
    u8"\u2203a\u2208X1 (1=2 & 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 (1=2 \u2228 3=4)"_c17,
    u8"\u2200a\u2208X1 (1=2 \u2228 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 (1=2 \u21D2 3=4)"_c17,
    u8"\u2200a\u2208X1 (1=2 \u21D2 3=4)"_c17
  );
  ExpectASTGeneration(
    u8"\u2200a\u2208X1 (1=2 \u21D4 3=4)"_c17,
    u8"\u2200a\u2208X1 (1=2 \u21D4 3=4)"_c17
  );

  ExpectASTGeneration(
    u8"\u2200(a,b)\u2208X1 1=2"_c17,
    u8"\u2200(a, b)\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200(a,b,c)\u2208X1 1=2"_c17,
    u8"\u2200(a, b, c)\u2208X1 1=2"_c17
  );
  ExpectASTGeneration(
    u8"\u2200(a,(b,c))\u2208X1 1=2"_c17,
    u8"\u2200(a, (b, c))\u2208X1 1=2"_c17
  );
}

TEST_F(UTRSGenerator, FromASTTyped) {
  ExpectASTGeneration("X1", "X1");
  ExpectASTGeneration("a", "a");
  ExpectASTGeneration("F1[a]", "F1[a]");
  ExpectASTGeneration("F1[S1 , S2 , S3]", "F1[S1, S2, S3]");

  ExpectASTGeneration(
    u8"X1 \u2208 X2"_c17,
    u8"X1\u2208X2"_c17
  );
  ExpectASTGeneration(
    u8"X1 \u2209 X2"_c17,
    u8"X1\u2209X2"_c17
  );
  ExpectASTGeneration(
    u8"X1 \u2282 X2"_c17,
    u8"X1\u2282X2"_c17
  );
  ExpectASTGeneration(
    u8"X1 \u2286 X2"_c17,
    u8"X1\u2286X2"_c17
  );
  ExpectASTGeneration(
    u8"X1 \u2284 X2"_c17,
    u8"X1\u2284X2"_c17
  );

  ExpectASTGeneration(
    u8"\u212C\u212C( X1 )"_c17,
    u8"\u212C\u212C(X1)"_c17
  );
  ExpectASTGeneration(
    u8"\u212C(\u212C( X1 ))"_c17,
    u8"\u212C\u212C(X1)"_c17
  );
  ExpectASTGeneration(
    "Pr1( S1 )",
    "Pr1(S1)"
  );
  ExpectASTGeneration(
    "Pr42( S1 )",
    "Pr42(S1)"
  );
  ExpectASTGeneration(
    "pr1( a )",
    "pr1(a)"
  );
  ExpectASTGeneration(
    "Fi1,2[ b, c]( a )",
    "Fi1,2[b, c](a)"
  );
  ExpectASTGeneration(
    "pr42( a )",
    "pr42(a)"
  );
  ExpectASTGeneration(
    "bool( a )",
    "bool(a)"
  );
  ExpectASTGeneration(
    "debool( a )",
    "debool(a)"
  );
  ExpectASTGeneration(
    "red( a )",
    "red(a)"
  );

  ExpectASTGeneration(
    "(a,b)",
    "(a, b)"
  );
  ExpectASTGeneration(
    "(a,b,c)",
    "(a, b, c)"
  );
  ExpectASTGeneration(
    "{a}",
    "{a}"
  );
  ExpectASTGeneration(
    "{a, b}",
    "{a, b}"
  );
  ExpectASTGeneration(
    "{a,b,c}",
    "{a, b, c}"
  );
  ExpectASTGeneration(
    "{(a,b),c}",
    "{(a, b), c}"
  );

  ExpectASTGeneration(
    u8"{a \u2208 X1|1=2}"_c17,
    u8"D{a\u2208X1 | 1=2}"_c17
  );
  ExpectASTGeneration(
    u8"D{(a,b) \u2208 X1|1=2}"_c17,
    u8"D{(a, b)\u2208X1 | 1=2}"_c17
  );
  ExpectASTGeneration(
    u8"D{(a,b,c) \u2208 X1|1=2}"_c17,
    u8"D{(a, b, c)\u2208X1 | 1=2}"_c17
  );
  ExpectASTGeneration(
    u8"D{(a,(b,c)) \u2208 X1|1=2}"_c17
    , u8"D{(a, (b, c))\u2208X1 | 1=2}"_c17
  );
  ExpectASTGeneration(
    u8"D{((a,b),c) \u2208 X1|1=2}"_c17,
    u8"D{((a, b), c)\u2208X1 | 1=2}"_c17
  );
  ExpectASTGeneration(
    "R{a:=X1 | 1=1| a \\ a}",
    "R{a:=X1 | 1=1 | a\\a}"
  );
  ExpectASTGeneration(
    u8"I{ (a,b) | b:\u2208X1; a:= b; 1=1}"_c17,
    u8"I{(a, b) | b:\u2208X1; a:=b; 1=1}"_c17
  );
}

TEST_F(UTRSGenerator, FromASTTypedOperators) {
  ExpectASTGeneration(
    "X1\\(X2\\X3)",
    "X1\\(X2\\X3)"
  );
  ExpectASTGeneration(
    "(X1\\X2)\\X3",
    "X1\\X2\\X3"
  );
  ExpectASTGeneration(
    "X1\\X2\\X3",
    "X1\\X2\\X3"
  );
  ExpectASTGeneration(
    u8"X1\u222AX2\u222AX3"_c17,
    u8"X1\u222AX2\u222AX3"_c17
  );
  ExpectASTGeneration(
    u8"X1\u2229X2\u2229X3"_c17,
    u8"X1\u2229X2\u2229X3"_c17
  );
  ExpectASTGeneration(
    u8"X1\u2206X2\u2206X3"_c17,
    u8"X1\u2206X2\u2206X3"_c17
  );

  ExpectASTGeneration(
    u8"X1\\(X2\u222AX3)"_c17,
    u8"X1\\(X2\u222AX3)"_c17
  );

  ExpectASTGeneration(
    u8"X1\u00D7X2\u00D7X3"_c17,
    u8"X1\u00D7X2\u00D7X3"_c17
  );
  ExpectASTGeneration(
    u8"(X1\u00D7X2)\u00D7X3"_c17,
    u8"(X1\u00D7X2)\u00D7X3"_c17
  );
  ExpectASTGeneration(
    u8"X1\u00D7(X2\u00D7X3)"_c17,
    u8"X1\u00D7(X2\u00D7X3)"_c17
  );
  ExpectASTGeneration(
    u8"X1\u00D7Pr1( S1 )"_c17,
    u8"X1\u00D7Pr1(S1)"_c17
  );
  ExpectASTGeneration(
    u8"Pr1( S1 )\u00D7X1"_c17,
    u8"Pr1(S1)\u00D7X1"_c17
  );
  ExpectASTGeneration(
    u8"{ S1 }\u00D7X1"_c17,
    u8"{S1}\u00D7X1"_c17
  );
  ExpectASTGeneration(
    u8"X1\u00D7{ S1 }"_c17,
    u8"X1\u00D7{S1}"_c17
  );
}

TEST_F(UTRSGenerator, CreateCall) {
  SetupEnvironment();
  EXPECT_EQ(Generator::CreateCall("F1", {}), "F1[]");
  EXPECT_EQ(Generator::CreateCall("F1", { "X1" }), "F1[X1]");
  EXPECT_EQ(Generator::CreateCall("", { "X1" }), "[X1]");
  EXPECT_EQ(Generator::CreateCall("F1", { "X1", "a" }), "F1[X1, a]");
  EXPECT_EQ(Generator::CreateCall("F1", { "", "" }), "F1[, ]");
}

TEST_F(UTRSGenerator, TermFromFunction) {
  static const std::string func1{ u8"[a\u2208\u212C(X1)] a\\X1"_c17 };
  static const std::string func2{ u8"[a\u2208\u212C(X1), b\u2208\u212C(X1)] a\\b"_c17 };

  Generator gen{ env };
  SetupEnvironment();

  env.data["F1"].arguments = FunctionArguments{ TypedID{ "a", "B(X1)"_t } };
  EXPECT_EQ(gen.TermFromFunction("F1", "[]", {"S1"}), "");
  EXPECT_EQ(gen.TermFromFunction("F1", "[] 1=1", {"S1"}), "1=1");
  EXPECT_EQ(gen.TermFromFunction("F1", func1, { "S1" }), "S1\\X1");

  env.data["F1"].arguments = FunctionArguments{ 
    TypedID{ "a", "B(X1)"_t },
    TypedID{ "b", "B(X1)"_t } 
  };
  EXPECT_EQ(gen.TermFromFunction("F1", func2, {"S1", "X1"}), "S1\\X1");
  EXPECT_EQ(gen.TermFromFunction("F1", func2, {"S1"}), "");
  EXPECT_EQ(gen.TermFromFunction("F1", func2, {"S1", "X1", "X1"}), "");
}

TEST_F(UTRSGenerator, FunctionFromExpression) {
  Generator gen{ env };
  SetupEnvironment();

  EXPECT_EQ(gen.FunctionFromExpr({}, ""), "");
  EXPECT_EQ(gen.FunctionFromExpr({ "S1" }, ""), "");
  EXPECT_EQ(
    gen.FunctionFromExpr({ "S1" }, "X1\\X1"),
    u8"[arg1\u2208\u212C(X1)] X1\\X1"_c17
  );
  EXPECT_EQ(
    gen.FunctionFromExpr({ "S1" }, "S1\\X1"),
    u8"[arg1\u2208\u212C(X1)] arg1\\X1"_c17
  );
  EXPECT_EQ(
    gen.FunctionFromExpr({ "S1", "X1" }, "S1\\X1"), 
    u8"[arg1\u2208\u212C(X1), arg2\u2208\u212C(X1)] arg1\\arg2"_c17
  );
  EXPECT_EQ(
    gen.FunctionFromExpr({ "S1" }, u8"{arg1 \u2208 S1 | 1=1}"_c17),
    u8"[arg2\u2208\u212C(X1)] {arg1 \u2208 arg2 | 1=1}"_c17
  );
}

TEST_F(UTRSGenerator, GenerateStructure) {
  Generator gen{ env };
  EXPECT_TRUE(empty(gen.StructureFor("X1", "X1"_t)));
  EXPECT_TRUE(empty(gen.StructureFor("X1", "B(X1)"_t)));
  EXPECT_TRUE(empty(gen.StructureFor("S1", "B(X1)"_t)));

  const auto structure = gen.StructureFor("S1", "BB(B(X1)*X1)"_t);
  EXPECT_EQ(ssize(structure), 4);
  EXPECT_EQ(structure.at(0).first, "red(S1)");
  EXPECT_EQ(structure.at(1).first, "Pr1(red(S1))");
  EXPECT_EQ(structure.at(2).first, "red(Pr1(red(S1)))");
  EXPECT_EQ(structure.at(3).first, "Pr2(red(S1))");
}

TEST_F(UTRSGenerator, Ascii2RS) {
  using ccl::rslang::ConvertTo;

  EXPECT_EQ(
    ConvertTo(R"(S1 \in B(X1))", Syntax::MATH),
    u8"S1\u2208\u212C(X1)"_c17
  );
  EXPECT_EQ(
    ConvertTo("B(Y*(X1*X2)*BB(X3))", Syntax::MATH),
    u8"\u212C(Y\u00D7(X1\u00D7X2)\u00D7\u212C\u212C(X3))"_c17
  );

  EXPECT_EQ(ConvertTo("  X1  ", Syntax::MATH), "X1");
  EXPECT_EQ(ConvertTo("a,?b", Syntax::MATH), "a,?b");
}

TEST_F(UTRSGenerator, RS2Ascii) {
  EXPECT_EQ(
    ConvertTo(u8"S1\u2208\u212C(X1)"_c17, Syntax::ASCII),
    R"(S1 \in B(X1))"
  );
  EXPECT_EQ(
    ConvertTo(u8"\u212C(Y\u00D7(X1\u00D7X2)\u00D7\u212C\u212C(X3))"_c17, Syntax::ASCII),
    R"(B(Y*(X1*X2)*BB(X3)))"
  );

  EXPECT_EQ(ConvertTo("  X1  ", Syntax::ASCII), R"(X1)");
  EXPECT_EQ(ConvertTo("\xE2\x88\x85", Syntax::ASCII), R"({})");
  EXPECT_EQ(ConvertTo("a,?b", Syntax::ASCII), R"(a,?b)");
}