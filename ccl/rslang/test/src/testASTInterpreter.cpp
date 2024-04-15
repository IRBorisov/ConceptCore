#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/ASTInterpreter.h"
#include "ccl/rslang/Parser.h"
#include "ccl/rslang/RSGenerator.h"

#include "FakeRSEnvironment.hpp"

inline std::string CreateFilter(const std::string& domain, const std::string& expression) {
  return R"(D{t \in )" + domain + R"( | )" + expression + '}';
}

class UTASTInterpreter : public ::testing::Test {
protected:
  using StructuredData = ccl::object::StructuredData;
  using Factory = ccl::object::Factory;
  using ASTInterpreter = ccl::rslang::ASTInterpreter;
  using SyntaxTree = ccl::rslang::SyntaxTree;
  using ValueEID = ccl::rslang::ValueEID;
  using ExpressionValue = ccl::rslang::ExpressionValue;
  using DataID = ccl::object::DataID;
  using Syntax = ccl::rslang::Syntax;

protected:
  UTASTInterpreter();

  static StructuredData CreateBaseSet(int32_t count);

  ccl::rslang::Parser parser{};
  RSEnvironment env{};
  ExpressionValue resultVal{};

  std::map<std::string, StructuredData> data {
    { "X1", CreateBaseSet(3) }, 
    { "X2", CreateBaseSet(3) },
    { "S1", CreateBaseSet(2) },
    { "S2", Factory::Set({ CreateBaseSet(3), Factory::EmptySet() }) }, // BB(X1)
    { "S3", Factory::Singleton(Factory::TupleV({ 0, 0 })) }, // B(X1*X2)
  };

  ASTInterpreter eval { 
    [&](const std::string& name) -> std::optional<StructuredData> {
      if (!data.contains(name)) {
        return std::nullopt;
      } else {
        return data.at(name);
      }
    },
    parser.log.SendReporter()
  };

  void ExpectValue(const std::string& input, DataID value);
  void ExpectValue(const std::string& input, bool value);
  void ExpectValue(const std::string& input, StructuredData value);

  void Evaluate(const std::string& input);
  void ExpectError(const std::string& input, ValueEID  id, ccl::StrPos pos);
};

UTASTInterpreter::UTASTInterpreter() {
  env.AddAST("F1", R"(F1 \defexpr [a \in B(X1)] { a })");
}

ccl::object::StructuredData UTASTInterpreter::CreateBaseSet(const int32_t count) {
  auto result = Factory::EmptySet();
  for (int32_t i = 0; i < count; ++i) {
    result.ModifyB().AddElement(Factory::Val(i));
  }
  return result;
}

void UTASTInterpreter::Evaluate(const std::string& input) {
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  SyntaxTree ast = parser.AST();
  ast.Normalize(env.GetAST());
  const auto result = eval.Evaluate(ast);
  ASSERT_TRUE(result.has_value()) << input;
  resultVal = result.value();
}

void UTASTInterpreter::ExpectValue(const std::string& input, const DataID value) {
  Evaluate(input);
  ASSERT_TRUE(std::holds_alternative<StructuredData>(resultVal)) << input;
  const auto& val = std::get<StructuredData>(resultVal);
  ASSERT_TRUE(val.IsElement()) << input;
  EXPECT_EQ(val.E().Value(), value) << input;
}

void UTASTInterpreter::ExpectValue(const std::string& input, const bool value) {
  Evaluate(input);
  ASSERT_TRUE(std::holds_alternative<bool>(resultVal)) << input;
  const auto& val = std::get<bool>(resultVal);
  EXPECT_EQ(val, value) << input;
}

void UTASTInterpreter::ExpectValue(const std::string& input, const StructuredData value) {
  Evaluate(input);
  ASSERT_TRUE(std::holds_alternative<StructuredData>(resultVal)) << input;
  const auto& val = std::get<StructuredData>(resultVal);
  EXPECT_EQ(val, value) << input;
}

void UTASTInterpreter::ExpectError(const std::string& input, const ValueEID id, const ccl::StrPos pos) {
  parser.log.Clear();
  ASSERT_TRUE(parser.Parse(input, Syntax::ASCII)) << input;
  auto ast = parser.ExtractAST();
  ast->Normalize(env.GetAST());
  ASSERT_FALSE(eval.Evaluate(*ast).has_value()) << input;

  EXPECT_EQ(static_cast<uint32_t>(id), begin(parser.log.All())->eid) << input;
  const auto errorPos = parser.log.FirstErrorPos();
  EXPECT_EQ(errorPos, pos) << input;
}

TEST_F(UTASTInterpreter, IterationsCounter) {
  ExpectValue(R"(\A a \in X1 a \eq a)", true);
  EXPECT_EQ(data.at("X1").B().Cardinality(), eval.Iterations());

  ExpectValue(R"(\E a \in X1 a \eq a)", true);
  EXPECT_EQ(eval.Iterations(), 1);

  ExpectValue(R"(I{a | a \from X1} \eq X1)", true);
  EXPECT_EQ(data.at("X1").B().Cardinality(), eval.Iterations());
}

TEST_F(UTASTInterpreter, NumericExpressions) {
  ExpectValue(R"(42)", 42);
  ExpectValue(R"(4 \plus 2)", 6);
  ExpectValue(R"(4 \multiply 2)", 8);
  ExpectValue(R"(4 \minus 2)", 2);
  ExpectValue(R"(2 \minus 4)", -2);
  ExpectValue(R"(card(X1))", 3);
}

TEST_F(UTASTInterpreter, NumericPredicates) {
  ExpectValue(R"(2 \plus 2 \eq 4)", true);
  ExpectValue(R"(2 \multiply 2 \eq 5)", false);
  ExpectValue(R"(1 \noteq 1)", false);
  ExpectValue(R"(1 \noteq 0)", true);
  ExpectValue(R"(1 \eq 1)", true);
  ExpectValue(R"(1 \gr 1)", false);
  ExpectValue(R"(1 \gr 0)", true);
  ExpectValue(R"(1 \ls 1)", false);
  ExpectValue(R"(0 \ls 1)", true);
  ExpectValue(R"(1 \le 1)", true);
  ExpectValue(R"(1 \le 2)", true);
  ExpectValue(R"(1 \ge 1)", true);
  ExpectValue(R"(2 \ge 1)", true);
}

TEST_F(UTASTInterpreter, LogicOperations) {
  ExpectValue(R"(\neg 1 \eq 2)", true);
  ExpectValue(R"(\neg 1 \eq 1)", false);

  ExpectValue(R"(1 \eq 1 \and 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 2 \and 1 \eq 1)", false);
  ExpectValue(R"(1 \eq 1 \and 1 \eq 2)", false);
  ExpectValue(R"(1 \eq 2 \and 1 \eq 2)", false);

  ExpectValue(R"(1 \eq 1 \or 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 2 \or 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 1 \or 1 \eq 2)", true);
  ExpectValue(R"(1 \eq 2 \or 1 \eq 2)", false);

  ExpectValue(R"(1 \eq 1 \impl 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 2 \impl 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 1 \impl 1 \eq 2)", false);
  ExpectValue(R"(1 \eq 2 \impl 1 \eq 2)", true);

  ExpectValue(R"(1 \eq 1 \equiv 1 \eq 1)", true);
  ExpectValue(R"(1 \eq 2 \equiv 1 \eq 1)", false);
  ExpectValue(R"(1 \eq 1 \equiv 1 \eq 2)", false);
  ExpectValue(R"(1 \eq 2 \equiv 1 \eq 2)", true);
}

TEST_F(UTASTInterpreter, Quantifier) {
  ExpectValue(R"(\A a \in X1 a \eq a)", true);
  ExpectValue(R"(\A a \in X1 a \noteq a)", false);
  ExpectValue(R"(\A a \in (X1 \setminus X1) a \eq a)", true);
  ExpectValue(R"(\A a \in (X1 \setminus X1) a \noteq a)", true);
  ExpectValue(R"(\A a \in X1 a \in S1)", false);

  ExpectValue(R"(\E a \in X1 a \eq a)", true);
  ExpectValue(R"(\E a \in X1 a \noteq a)", false);
  ExpectValue(R"(\E a \in (X1 \setminus X1) a \eq a)", false);
  ExpectValue(R"(\E a \in (X1 \setminus X1) a \noteq a)", false);
  ExpectValue(R"(\E a \in X1 a \in S1)", true);

  ExpectValue(R"(\A a,b \in X1 a \eq b)", false);
  ExpectValue(R"(\E a,b \in X1 a \eq b)", true);
  ExpectValue(R"(\A a \in X1 \E b \in X1 a \eq b)", true);
  ExpectValue(R"(\A (a,b) \in S3 (a \in X1 \and b \in X2))", true);

  ExpectValue(R"(debool({X1}) \eq X1)", true);
  ExpectValue(R"(\A a \in X1 debool({a}) \eq a)", true);
  ExpectValue(R"(\A a \in X1*X1 debool({a}) \eq a)", true);
}

TEST_F(UTASTInterpreter, TypedPredicates) {
  ExpectValue(R"(X1 \eq X1)", true);
  ExpectValue(R"(X1 \eq S1)", false);
  ExpectValue(R"(S1 \eq X1)", false);

  ExpectValue(R"(X1 \eq {})", false);

  ExpectValue(R"(X1 \noteq X1)", false);
  ExpectValue(R"(X1 \noteq S1)", true);
  ExpectValue(R"(S1 \noteq X1)", true);

  ExpectValue(R"(X1 \in S2)", true);
  ExpectValue(R"(S1 \in S2)", false);

  ExpectValue(R"(X1 \notin S2)", false);
  ExpectValue(R"(S1 \notin S2)", true);

  ExpectValue(R"(X1 \subset X1)", false);
  ExpectValue(R"(S1 \subset X1)", true);
  ExpectValue(R"(X1 \subset S1)", false);

  ExpectValue(R"(X1 \notsubset X1)", true);
  ExpectValue(R"(S1 \notsubset X1)", false);
  ExpectValue(R"(X1 \notsubset S1)", true);

  ExpectValue(R"(X1 \subseteq X1)", true);
  ExpectValue(R"(S1 \subseteq X1)", true);
  ExpectValue(R"(X1 \subseteq S1)", false);
}

TEST_F(UTASTInterpreter, TypedBasics) {
  ExpectValue(R"(X1)", data.at("X1"));

  ExpectValue(R"(X1 \union X1)", data.at("X1"));
  ExpectValue(R"(X1 \union S1)", data.at("X1"));
  ExpectValue(R"(S1 \union X1)", data.at("X1"));

  ExpectValue(R"(X1 \intersect X1)", data.at("X1"));
  ExpectValue(R"(X1 \intersect S1)", data.at("S1"));
  ExpectValue(R"(S1 \intersect X1)", data.at("S1"));

  ExpectValue(R"(X1 \setminus X1)", Factory::EmptySet());
  ExpectValue(R"(X1 \setminus S1)", Factory::Singleton(Factory::Val(2)));
  ExpectValue(R"(S1 \setminus X1)", Factory::EmptySet());

  ExpectValue(R"(X1 \symmdiff X1)", Factory::EmptySet());
  ExpectValue(R"(X1 \symmdiff S1)", Factory::Singleton(Factory::Val(2)));
  ExpectValue(R"(S1 \symmdiff X1)", Factory::Singleton(Factory::Val(2)));

  ExpectValue(R"(X1*X2)", Factory::Decartian({ data.at("X1"),  data.at("X2") }));
  ExpectValue(R"(X1*X2*X1)", Factory::Decartian({ data.at("X1"),  data.at("X2"), data.at("X1") }));
  ExpectValue(R"(X1*(X2 \setminus X2))", Factory::EmptySet());
  ExpectValue(R"((X1 \setminus X1)*X2)", Factory::EmptySet());

  ExpectValue(R"(B(X1))", Factory::Boolean(data.at("X1")));
  ExpectValue(R"(B(X1 \setminus X1))", Factory::Singleton(Factory::EmptySet()));

  ExpectValue(R"({ X1 })", Factory::Singleton(data.at("X1")));
  ExpectValue(R"({ X1, X1 })", Factory::Singleton(data.at("X1")));
  ExpectValue(R"({ X1, S1 })", Factory::Set({ data.at("X1"), data.at("S1") }));
  ExpectValue(R"({ X1, S1, X1 })", Factory::Set({ data.at("X1"), data.at("S1") }));

  ExpectValue(R"((X1, X1))", Factory::Tuple({ data.at("X1"), data.at("X1") }));
  ExpectValue(R"((X1, X2, X1))", Factory::Tuple({ data.at("X1"), data.at("X2"), data.at("X1") }));
}

TEST_F(UTASTInterpreter, TypedExpressions) {
  ExpectValue(R"(bool(X1))", Factory::Singleton(data.at("X1")));
  ExpectValue(R"(bool(X1 \setminus X1))", Factory::Singleton(Factory::EmptySet()));
  ExpectValue(R"(debool(bool(X1)))", data.at("X1"));
  ExpectValue(R"(red(S2))", data.at("X1"));
  ExpectValue(R"(red(S2 \setminus S2))", Factory::EmptySet());

  ExpectValue(R"(pr1((X1, X2)))", data.at("X1"));
  ExpectValue(R"(pr2((X1, X2)))", data.at("X2"));

  ExpectValue(R"(Pr1(S3))", Factory::Singleton(Factory::Val(0)));
  ExpectValue(R"(Pr2(S3))", Factory::Singleton(Factory::Val(0)));
  ExpectValue(R"(Pr1(S3 \setminus S3))", Factory::EmptySet());
  ExpectValue(R"(Pr1,2(S3 \setminus S3))", Factory::EmptySet());
  ExpectValue(R"(Fi1[X1](S3))", data.at("S3"));
  ExpectValue(R"(Fi1,2[X1,X2](S3))", data.at("S3"));
  ExpectValue(R"(Fi2,1[X2,X1](S3))", data.at("S3"));
  ExpectValue(R"(Fi1[X1 \setminus X1](S3))", Factory::EmptySet());
  ExpectValue(R"(Fi1[X1](S3 \setminus S3))", Factory::EmptySet());

  ExpectValue(CreateFilter(R"(X1)", R"(t \eq t)"), data.at("X1"));
  ExpectValue(CreateFilter(R"(X1 \setminus X1)", R"(t \eq t)"), Factory::EmptySet());
  ExpectValue(CreateFilter(R"(B(X1) \setminus B(X1))", R"(t \eq t)"), Factory::EmptySet());

  ExpectValue(R"(D{a \in X1 | 1 \eq 1})", data.at("X1"));
  ExpectValue(R"(D{a \in X1 | a \noteq a})", Factory::EmptySet());

  ExpectValue(R"(I{(a,b) | a \from X1; b \assign a; b \noteq a})", Factory::EmptySet());
  ExpectValue(R"(I{a | a \from X1})", data.at("X1"));

  ExpectValue(R"(R{a \assign X1 | a \setminus a})", Factory::EmptySet());
  ExpectValue(R"(R{a \assign X1 \setminus X1 | a \union X1})", data.at("X1"));
}

TEST_F(UTASTInterpreter, Function) {
  ExpectValue(R"(F1[X1])", Factory::Singleton(data.at("X1")));
  ExpectValue(R"(F1[F1[X1]])", Factory::Singleton(Factory::Singleton(data.at("X1"))));
  ExpectValue(R"(F1[X1 \setminus X1])", Factory::Singleton(Factory::EmptySet()));
}

TEST_F(UTASTInterpreter, GlobalDeclaration) {
  ExpectValue(R"(D1 \defexpr X1 \setminus X1)", Factory::EmptySet());
  ExpectValue(R"(A1 \defexpr 1 \eq 1)", true);
}

TEST_F(UTASTInterpreter, ErrorsGlobalID) {
  ExpectError(R"(X1 \setminus D1)", ValueEID::globalMissingValue, 13);
  ExpectError(R"(D1 \setminus X1)", ValueEID::globalMissingValue, 0);
  ExpectError(R"(\neg D1 \setminus X1 \eq X1)", ValueEID::globalMissingValue, 5);

  ExpectError(R"(card(D1) \eq 1)", ValueEID::globalMissingValue, 5);
  ExpectError(R"(1 \eq card(D1))", ValueEID::globalMissingValue, 11);
  ExpectError(R"(card(D1) \plus 1)", ValueEID::globalMissingValue, 5);
  ExpectError(R"(1 \plus card(D1))", ValueEID::globalMissingValue, 13);
  ExpectError(R"(card(D1) \gr 1)", ValueEID::globalMissingValue, 5);
  ExpectError(R"(1 \gr card(D1))", ValueEID::globalMissingValue, 11);
  ExpectError(R"(card(D1) \eq 1 \and 1 \eq 1)", ValueEID::globalMissingValue, 5);
  ExpectError(R"(1 \eq 1 \and card(D1) \eq 1)", ValueEID::globalMissingValue, 18);

  ExpectError(R"(\A a \in D1 a  \eq  a)", ValueEID::globalMissingValue, 9);
  ExpectError(R"(\A a \in X1 a \in D1)", ValueEID::globalMissingValue, 18);

  ExpectError(R"(X1*D1)", ValueEID::globalMissingValue, 3);
  ExpectError(R"(D1*X1)", ValueEID::globalMissingValue, 0);
  ExpectError(R"(B(D1))", ValueEID::globalMissingValue, 2);

  ExpectError(R"(pr1(D1))", ValueEID::globalMissingValue, 4);
  ExpectError(R"(Pr1(D1))", ValueEID::globalMissingValue, 4);
  ExpectError(R"(bool(D1))", ValueEID::globalMissingValue, 5);
  ExpectError(R"(debool(D1))", ValueEID::globalMissingValue, 7);
  ExpectError(R"(red(D1))", ValueEID::globalMissingValue, 4);

  ExpectError(R"({ D1 })", ValueEID::globalMissingValue, 2);
  ExpectError(R"((D1, X1))", ValueEID::globalMissingValue, 1);
  ExpectError(CreateFilter("D1", R"(t \eq t)"), ValueEID::globalMissingValue, 8);
  ExpectError(CreateFilter("X1", R"(t \in D1)"), ValueEID::globalMissingValue, 19);

  ExpectError(R"(D{a \in D1 | a \eq X1})", ValueEID::globalMissingValue, 8);
  ExpectError(R"(D{a \in X1 | a \eq D1})", ValueEID::globalMissingValue, 19);

  ExpectError(R"(R{a \assign D1 | a \union X1})", ValueEID::globalMissingValue, 12);
  ExpectError(R"(R{a \assign X1 | a \union D1})", ValueEID::globalMissingValue, 26);
  ExpectError(R"(R{a \assign X1 | 1 \eq 1 | a \union D1})", ValueEID::globalMissingValue, 36);
  ExpectError(R"(R{a \assign X1 | D1 \eq D1 | a \union X1})", ValueEID::globalMissingValue, 17);

  ExpectError(R"(I{(a,b) | a \from D1; b \assign a; b \noteq a})", ValueEID::globalMissingValue, 18);
  ExpectError(R"(I{(a,b) | a \from X1; b \assign a; b \noteq D1})", ValueEID::globalMissingValue, 44);
  ExpectError(R"(I{(a,b) | a \from X1; b \assign D1; b \noteq a})", ValueEID::globalMissingValue, 32);
}

TEST_F(UTASTInterpreter, ErrorsPopup) {
  ExpectError(R"(debool(X1))", ValueEID::invalidDebool, 0);

  ExpectError(R"(card({debool(X1)}))", ValueEID::invalidDebool, 6);
  ExpectError(R"(card({debool(X1)}) \plus 1)", ValueEID::invalidDebool, 6);
  ExpectError(R"(1 \plus card({debool(X1)}))", ValueEID::invalidDebool, 14);

  ExpectError(R"(\A a \in X1 {debool(X1)} \eq X1)", ValueEID::invalidDebool, 13);
  ExpectError(R"(\A a \in {debool(X1)} 1 \eq 1)", ValueEID::invalidDebool, 10);

  ExpectError(R"(\neg {debool(X1)} \eq X1)", ValueEID::invalidDebool, 6);
  ExpectError(R"({debool(X1)} \eq X1 \and 1 \eq 1)", ValueEID::invalidDebool, 1);
  ExpectError(R"(1 \eq 1 \and {debool(X1)} \eq X1)", ValueEID::invalidDebool, 14);

  ExpectError(R"({debool(X1)} \eq X1)", ValueEID::invalidDebool, 1);
  ExpectError(R"(X1 \eq {debool(X1)})", ValueEID::invalidDebool, 8);
  ExpectError(R"(B({debool(X1)}))", ValueEID::invalidDebool, 3);
  ExpectError(R"((X1, debool(X1)))", ValueEID::invalidDebool, 5);
  ExpectError(R"({X1, {debool(X1)}})", ValueEID::invalidDebool, 6);
  ExpectError(R"({debool(X1)}*X1)", ValueEID::invalidDebool, 1);
  ExpectError(R"(X1*{debool(X1)})", ValueEID::invalidDebool, 4);
  ExpectError(R"({debool(X1)} \setminus X1)", ValueEID::invalidDebool, 1);
  ExpectError(R"(X1 \setminus {debool(X1)})", ValueEID::invalidDebool, 14);

  ExpectError(R"(pr1((debool(X1), debool(X1))))", ValueEID::invalidDebool, 5);
  ExpectError(R"(Pr1({debool(X1)}*X1))", ValueEID::invalidDebool, 5);
  ExpectError(R"(debool(debool({X1, X1 \setminus X1})))", ValueEID::invalidDebool, 7);
  ExpectError(R"(red({{debool(X1)}}))", ValueEID::invalidDebool, 6);
  ExpectError(R"(bool(debool(X1)))", ValueEID::invalidDebool, 5);

  ExpectError(R"(D{a \in {debool(X1)} | a \eq X1})", ValueEID::invalidDebool, 9);
  ExpectError(R"(D{a \in X1 | a \eq debool(X1)})", ValueEID::invalidDebool, 19);

  ExpectError(R"(R{a \assign debool(X1) | a})", ValueEID::invalidDebool, 12);
  ExpectError(R"(R{a \assign X1 | a \union {debool(X1)}})", ValueEID::invalidDebool, 27);
  ExpectError(R"(R{a \assign X1 | 1 \eq 1 | a \union {debool(X1)}})", ValueEID::invalidDebool, 37);
  ExpectError(R"(R{a \assign X1 | debool(X1) \eq debool(X1) | a \union X1})", ValueEID::invalidDebool, 17);

  ExpectError(R"(I{(a,b) | a \from {debool(X1)}; b \assign a; b \noteq a})", ValueEID::invalidDebool, 19);
  ExpectError(R"(I{(a,b) | a \from X1; b \assign a; b \noteq debool(X1)})", ValueEID::invalidDebool, 44);
  ExpectError(R"(I{(a,b) | a \from X1; b \assign debool(X1); b \noteq a})", ValueEID::invalidDebool, 32);

  ExpectError(R"(X4 \defexpr )", ValueEID::unknownError, 0);
  ExpectError(R"(S4 \deftype B(X1))", ValueEID::unknownError, 0);
  ExpectError(R"(F1 \defexpr [a \in X1] {a})", ValueEID::unknownError, 0);
}

TEST_F(UTASTInterpreter, ErrorsBoolSetLimit) {
  data.insert({ "X3", CreateBaseSet(StructuredData::BOOL_INFINITY) });
  ExpectError(R"(B(X3))", ValueEID::booleanLimit, 0);
}

TEST_F(UTASTInterpreter, ErrorsIterateIntegers) {
  ExpectError(R"(\A a \in Z a \eq a)", ValueEID::iterateInfinity, 9);
}