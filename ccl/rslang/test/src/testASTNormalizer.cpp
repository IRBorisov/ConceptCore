#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ASTNormalizer.h"
#include "ccl/rslang/Parser.h"
#include "ccl/rslang/Literals.h"

using ccl::operator""_c17;

class UTastNormalize : public ::testing::Test {
protected:
  using SyntaxTree = ccl::rslang::SyntaxTree;
  using Parser = ccl::rslang::Parser;
  using Syntax = ccl::rslang::Syntax;

protected:
  UTastNormalize();

  Parser parser{};
  RSEnvironment env{};

  void ExpectAST(const std::string& input, const std::string& ast);
};

UTastNormalize::UTastNormalize() {
  env.AddAST("F1", R"(F1 \defexpr [a \in B(X1)] { a })");
  env.AddAST("F2", R"(F2 \defexpr [a \in B(X1), b \in B(X2)] (a, b))");
  env.AddAST("F3", R"(F3 \defexpr [a \in B(X1)] { t \in a | t \eq t})");
}

void UTastNormalize::ExpectAST(const std::string& input, const std::string& ast) {
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  auto normalized = parser.ExtractAST();
  normalized->Normalize(env.GetAST());
  EXPECT_EQ(ccl::rslang::AST2String::Apply(*normalized), ast) << input;
}

TEST_F(UTastNormalize, EnumDeclaration) {
  ExpectAST(
    R"(\A a,b \in X1 a \eq b)",
    u8"[\u2200[a][X1][\u2200[b][X1][=[a][b]]]]"_c17
  );
  ExpectAST(
    R"(\E a,b \in X1 a \eq b)",
    u8"[\u2203[a][X1][\u2203[b][X1][=[a][b]]]]"_c17
  );
  ExpectAST(
    R"(\E a,b,c \in X1 a \eq b)",
    u8"[\u2203[a][X1][\u2203[b][X1][\u2203[c][X1][=[a][b]]]]]"_c17
  );
}

TEST_F(UTastNormalize, Quantor) {
  ExpectAST(
    R"(\A (a,b) \in S1 a \eq b)",
    u8"[\u2200[@ab][S1][=[pr1[@ab]][pr2[@ab]]]]"_c17
  );
  ExpectAST(
    R"(\E (a,b) \in S1 a \eq b)",
    u8"[\u2203[@ab][S1][=[pr1[@ab]][pr2[@ab]]]]"_c17
  );
  ExpectAST(
    R"(\E (a,b,c) \in S1 (a \eq b \and b \eq c))", 
    u8"[\u2203[@abc][S1][&[=[pr1[@abc]][pr2[@abc]]][=[pr2[@abc]][pr3[@abc]]]]]"_c17
  );
  ExpectAST(
    R"(\E (a,(b,c)) \in S1 (a \eq b \and b \eq c))", 
    u8"[\u2203[@abc][S1][&[=[pr1[@abc]][pr1[pr2[@abc]]]][=[pr1[pr2[@abc]]][pr2[pr2[@abc]]]]]]"_c17
  );
}

TEST_F(UTastNormalize,TermTupleDeclaration) {
  ExpectAST(
    R"(D{(a,b) \in S1 | a \eq b})",
    "[DECLARATIVE[@ab][S1][=[pr1[@ab]][pr2[@ab]]]]"
  );
  ExpectAST(
    R"(D{(a,b,c) \in S1 | a \eq b \and b \eq c})", 
    "[DECLARATIVE[@abc][S1][&[=[pr1[@abc]][pr2[@abc]]][=[pr2[@abc]][pr3[@abc]]]]]"
  );
  ExpectAST(
    R"(D{(a,(b,c)) \in S1 | a \eq b \and b \eq c})", 
    "[DECLARATIVE[@abc][S1][&[=[pr1[@abc]][pr1[pr2[@abc]]]][=[pr1[pr2[@abc]]][pr2[pr2[@abc]]]]]]"
  );
}

TEST_F(UTastNormalize, Imperative) {
  ExpectAST(
    R"(I{(b,a) | (a,b) \from S1})",
    u8"[IMPERATIVE[TUPLE[pr2[@ab]][pr1[@ab]]][:\u2208[@ab][S1]]]"_c17
  );
  ExpectAST(
    R"(I{b | (a,b) \from S1; a \eq a})",
    u8"[IMPERATIVE[pr2[@ab]][:\u2208[@ab][S1]][=[pr1[@ab]][pr1[@ab]]]]"_c17
  );
  ExpectAST(
    R"(I{(c,b) | a \from {S1}; (b,c) \assign a})",
    u8"[IMPERATIVE[TUPLE[pr2[@bc]][pr1[@bc]]][:\u2208[a][SET[S1]]][:=[@bc][a]]]"_c17
  );
}

TEST_F(UTastNormalize, RecrusionTupleDeclaration) {
  ExpectAST(
    R"(R{(a,b) \assign S1 | (b,a)})",
    "[REC_SHORT[@ab][S1][TUPLE[pr2[@ab]][pr1[@ab]]]]"
  );
  ExpectAST(
    R"(R{(a,b) \assign S1 | 1 \eq 1 | (b,a)})",
    "[REC_FULL[@ab][S1][=[1][1]][TUPLE[pr2[@ab]][pr1[@ab]]]]"
  );
}

TEST_F(UTastNormalize, Functions) {
  ExpectAST(R"(F1[X1])", "[SET[X1]]");
  ExpectAST(R"(F1[X1 \setminus X1])", "[SET[\\[X1][X1]]]");
  ExpectAST(R"(F1[F1[X1]])", "[SET[SET[X1]]]");
  ExpectAST(R"(F2[X1, X2])", "[TUPLE[X1][X2]]");
  ExpectAST(
    R"(F3[X1])",
    "[DECLARATIVE[__var1][X1][=[__var1][__var1]]]"
  );
  ExpectAST(
    R"(F3[F3[X1]])",
    "[DECLARATIVE[__var1][DECLARATIVE[__var2][X1][=[__var2][__var2]]][=[__var1][__var1]]]"
  );
  ExpectAST(
    R"(\A t \in X1 F3[{t}] \eq t)",
    u8"[\u2200[t][X1][=[DECLARATIVE[__var1][SET[t]][=[__var1][__var1]]][t]]]"_c17
  );
}