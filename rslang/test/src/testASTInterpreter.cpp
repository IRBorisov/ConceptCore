#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/ASTInterpreter.h"
#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ErrorLogger.h"
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

protected:
	UTASTInterpreter();

	static StructuredData CreateBaseSet(int32_t count);

	ccl::rslang::ErrorLogger log{};
	ccl::rslang::AsciiLexer lexer{ log.SendReporter() };
	ccl::rslang::RSParser parser{ log.SendReporter() };
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
		log.SendReporter()	
	};

	void ExpectValue(const std::string& input, DataID value);
	void ExpectValue(const std::string& input, bool value);
	void ExpectValue(const std::string& input, StructuredData value);

	void Evaluate(const std::string& input);
	void ExpectError(const std::string& input, ValueEID  id, ccl::StrPos pos);
};

UTASTInterpreter::UTASTInterpreter() {
	env.AddAST("F1", R"(F1:==[a \in B(X1)] { a })");
}

ccl::object::StructuredData UTASTInterpreter::CreateBaseSet(const int32_t count) {
	auto result = Factory::EmptySet();
	for (int32_t i = 0; i < count; ++i) {
		result.ModifyB().AddElement(Factory::Val(i));
	}
	return result;
}

void UTASTInterpreter::Evaluate(const std::string& input) {
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
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
	log.Clear();
	ASSERT_TRUE(parser.Parse(lexer(input).Stream())) << input;
	auto ast = parser.ExtractAST();
	ast->Normalize(env.GetAST());
	ASSERT_FALSE(eval.Evaluate(*ast).has_value()) << input;

	EXPECT_EQ(static_cast<uint32_t>(id), begin(log.All())->eid) << input;
	const auto errorPos = log.FirstErrorPos();
	EXPECT_EQ(errorPos, pos) << input;
}

TEST_F(UTASTInterpreter, IterationsCounter) {
	ExpectValue(R"(\A a \in X1 a=a)", true);
	EXPECT_EQ(data.at("X1").B().Cardinality(), eval.Iterations());

	ExpectValue(R"(\E a \in X1 a=a)", true);
	EXPECT_EQ(eval.Iterations(), 1);

	ExpectValue(R"(I{a | a \from X1} = X1)", true);
	EXPECT_EQ(data.at("X1").B().Cardinality(), eval.Iterations());
}

TEST_F(UTASTInterpreter, NumericExpressions) {
	ExpectValue(R"(42)", 42);
	ExpectValue(R"(4+2)", 6);
	ExpectValue(R"(4 \multiply 2)", 8);
	ExpectValue(R"(4-2)", 2);
	ExpectValue(R"(2-4)", -2);
	ExpectValue(R"(card(X1))", 3);
}

TEST_F(UTASTInterpreter, NumericPredicates) {
	ExpectValue(R"(2+2=4)", true);
	ExpectValue(R"(2 \multiply 2=5)", false);
	ExpectValue(R"(1!=1)", false);
	ExpectValue(R"(1!=0)", true);
	ExpectValue(R"(1=1)", true);
	ExpectValue(R"(1>1)", false);
	ExpectValue(R"(1>0)", true);
	ExpectValue(R"(1<1)", false);
	ExpectValue(R"(0<1)", true);
	ExpectValue(R"(1<=1)", true);
	ExpectValue(R"(1>=1)", true);
}

TEST_F(UTASTInterpreter, LogicOperations) {
	ExpectValue(R"(\neg 1=2)", true);
	ExpectValue(R"(\neg 1=1)", false);

	ExpectValue(R"(1=1 && 1=1)", true);
	ExpectValue(R"(1=2 && 1=1)", false);
	ExpectValue(R"(1=1 && 1=2)", false);
	ExpectValue(R"(1=2 && 1=2)", false);

	ExpectValue(R"(1=1 || 1=1)", true);
	ExpectValue(R"(1=2 || 1=1)", true);
	ExpectValue(R"(1=1 || 1=2)", true);
	ExpectValue(R"(1=2 || 1=2)", false);

	ExpectValue(R"(1=1 => 1=1)", true);
	ExpectValue(R"(1=2 => 1=1)", true);
	ExpectValue(R"(1=1 => 1=2)", false);
	ExpectValue(R"(1=2 => 1=2)", true);

	ExpectValue(R"(1=1 <=> 1=1)", true);
	ExpectValue(R"(1=2 <=> 1=1)", false);
	ExpectValue(R"(1=1 <=> 1=2)", false);
	ExpectValue(R"(1=2 <=> 1=2)", true);
}

TEST_F(UTASTInterpreter, Quantifier) {
	ExpectValue(R"(\A a \in X1 a=a)", true);
	ExpectValue(R"(\A a \in X1 a!=a)", false);
	ExpectValue(R"(\A a \in (X1 \setminus X1) a=a)", true);
	ExpectValue(R"(\A a \in (X1 \setminus X1) a!=a)", true);
	ExpectValue(R"(\A a \in X1 a \in S1)", false);

	ExpectValue(R"(\E a \in X1 a=a)", true);
	ExpectValue(R"(\E a \in X1 a!=a)", false);
	ExpectValue(R"(\E a \in (X1 \setminus X1) a=a)", false);
	ExpectValue(R"(\E a \in (X1 \setminus X1) a!=a)", false);
	ExpectValue(R"(\E a \in X1 a \in S1)", true);

	ExpectValue(R"(\A a,b \in X1 a=b)", false);
	ExpectValue(R"(\E a,b \in X1 a=b)", true);
	ExpectValue(R"(\A a \in X1 \E b \in X1 a=b)", true);
	ExpectValue(R"(\A (a,b) \in S3 (a \in X1 && b \in X2))", true);

	ExpectValue(R"(debool({X1})=X1)", true);
	ExpectValue(R"(\A a \in X1 debool({a})=a)", true);
	ExpectValue(R"(\A a \in X1*X1 debool({a})=a)", true);
}

TEST_F(UTASTInterpreter, TypedPredicates) {
	ExpectValue(R"(X1 = X1)", true);
	ExpectValue(R"(X1 = S1)", false);
	ExpectValue(R"(S1 = X1)", false);

	ExpectValue(R"(X1 = {})", false);

	ExpectValue(R"(X1 != X1)", false);
	ExpectValue(R"(X1 != S1)", true);
	ExpectValue(R"(S1 != X1)", true);

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

	ExpectValue(CreateFilter(R"(X1)", "t=t"), data.at("X1"));
	ExpectValue(CreateFilter(R"(X1 \setminus X1)", "t=t"), Factory::EmptySet());
	ExpectValue(CreateFilter(R"(B(X1) \setminus B(X1))", "t=t"), Factory::EmptySet());

	ExpectValue(R"(D{a \in X1 | 1=1})", data.at("X1"));
	ExpectValue(R"(D{a \in X1 | a!=a})", Factory::EmptySet());

	ExpectValue(R"(I{(a,b) | a \from X1; b:=a; b!=a})", Factory::EmptySet());
	ExpectValue(R"(I{a | a \from X1})", data.at("X1"));

	ExpectValue(R"(R{a:=X1 | a \setminus a})", Factory::EmptySet());
	ExpectValue(R"(R{a:=X1 \setminus X1 | a \union X1})", data.at("X1"));
}

TEST_F(UTASTInterpreter, Function) {
	ExpectValue(R"(F1[X1])", Factory::Singleton(data.at("X1")));
	ExpectValue(R"(F1[F1[X1]])", Factory::Singleton(Factory::Singleton(data.at("X1"))));
	ExpectValue(R"(F1[X1 \setminus X1])", Factory::Singleton(Factory::EmptySet()));
}

TEST_F(UTASTInterpreter, GlobalDeclaration) {
	ExpectValue(R"(D1:==X1 \setminus X1)", Factory::EmptySet());
	ExpectValue(R"(A1:==1=1)", true);
}

TEST_F(UTASTInterpreter, ErrorsGlobalID) {
	ExpectError(R"(X1 \setminus D1)", ValueEID::globalMissingValue, 13);
	ExpectError(R"(D1 \setminus X1)", ValueEID::globalMissingValue, 0);
	ExpectError(R"(\neg D1 \setminus X1=X1)", ValueEID::globalMissingValue, 5);

	ExpectError(R"(card(D1)=1)", ValueEID::globalMissingValue, 5);
	ExpectError(R"(1=card(D1))", ValueEID::globalMissingValue, 7);
	ExpectError(R"(card(D1)+1)", ValueEID::globalMissingValue, 5);
	ExpectError(R"(1+card(D1))", ValueEID::globalMissingValue, 7);
	ExpectError(R"(card(D1)>1)", ValueEID::globalMissingValue, 5);
	ExpectError(R"(1>card(D1))", ValueEID::globalMissingValue, 7);
	ExpectError(R"(card(D1)=1 && 1=1)", ValueEID::globalMissingValue, 5);
	ExpectError(R"(1=1 && card(D1)=1)", ValueEID::globalMissingValue, 12);

	ExpectError(R"(\A a \in D1 a = a)", ValueEID::globalMissingValue, 9);
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
	ExpectError(CreateFilter("D1", R"(t=t)"), ValueEID::globalMissingValue, 8);
	ExpectError(CreateFilter("X1", R"(t \in D1)"), ValueEID::globalMissingValue, 19);

	ExpectError(R"(D{a \in D1 | a=X1})", ValueEID::globalMissingValue, 8);
	ExpectError(R"(D{a \in X1 | a=D1})", ValueEID::globalMissingValue, 15);

	ExpectError(R"(R{a:=D1 | a \union X1})", ValueEID::globalMissingValue, 5);
	ExpectError(R"(R{a:=X1 | a \union D1})", ValueEID::globalMissingValue, 19);
	ExpectError(R"(R{a:=X1 | 1=1 | a \union D1})", ValueEID::globalMissingValue, 25);
	ExpectError(R"(R{a:=X1 | D1=D1 | a \union X1})", ValueEID::globalMissingValue, 10);

	ExpectError(R"(I{(a,b) | a \from D1; b:=a; b!=a})", ValueEID::globalMissingValue, 18);
	ExpectError(R"(I{(a,b) | a \from X1; b:=a; b!=D1})", ValueEID::globalMissingValue, 31);
	ExpectError(R"(I{(a,b) | a \from X1; b:=D1; b!=a})", ValueEID::globalMissingValue, 25);
}

TEST_F(UTASTInterpreter, ErrorsPopup) {
	ExpectError(R"(debool(X1))", ValueEID::invalidDebool, 0);

	ExpectError(R"(card({debool(X1)}))", ValueEID::invalidDebool, 6);
	ExpectError(R"(card({debool(X1)})+1)", ValueEID::invalidDebool, 6);
	ExpectError(R"(1+card({debool(X1)}))", ValueEID::invalidDebool, 8);

	ExpectError(R"(\A a \in X1 {debool(X1)}=X1)", ValueEID::invalidDebool, 13);
	ExpectError(R"(\A a \in {debool(X1)} 1=1)", ValueEID::invalidDebool, 10);

	ExpectError(R"(\neg {debool(X1)}=X1)", ValueEID::invalidDebool, 6);
	ExpectError(R"({debool(X1)}=X1 && 1=1)", ValueEID::invalidDebool, 1);
	ExpectError(R"(1=1 && {debool(X1)}=X1)", ValueEID::invalidDebool, 8);

	ExpectError(R"({debool(X1)}=X1)", ValueEID::invalidDebool, 1);
	ExpectError(R"(X1={debool(X1)})", ValueEID::invalidDebool, 4);
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

	ExpectError(R"(D{a \in {debool(X1)} | a=X1})", ValueEID::invalidDebool, 9);
	ExpectError(R"(D{a \in X1 | a=debool(X1)})", ValueEID::invalidDebool, 15);

	ExpectError(R"(R{a:=debool(X1) | a})", ValueEID::invalidDebool, 5);
	ExpectError(R"(R{a:=X1 | a \union {debool(X1)}})", ValueEID::invalidDebool, 20);
	ExpectError(R"(R{a:=X1 | 1=1 | a \union {debool(X1)}})", ValueEID::invalidDebool, 26);
	ExpectError(R"(R{a:=X1 | debool(X1)=debool(X1) | a \union X1})", ValueEID::invalidDebool, 10);

	ExpectError(R"(I{(a,b) | a \from {debool(X1)}; b:=a; b!=a})", ValueEID::invalidDebool, 19);
	ExpectError(R"(I{(a,b) | a \from X1; b:=a; b!=debool(X1)})", ValueEID::invalidDebool, 31);
	ExpectError(R"(I{(a,b) | a \from X1; b:=debool(X1); b!=a})", ValueEID::invalidDebool, 25);

	ExpectError(R"(X4:==)", ValueEID::unknownError, 0);
	ExpectError(R"(S4::=B(X1))", ValueEID::unknownError, 0);
	ExpectError(R"(F1:==[a \in X1] {a})", ValueEID::unknownError, 0);
}

TEST_F(UTASTInterpreter, ErrorsBoolSetLimit) {
	data.insert({ "X3", CreateBaseSet(StructuredData::BOOL_INFINITY) });
	ExpectError(R"(B(X3))", ValueEID::booleanLimit, 0);
}

TEST_F(UTASTInterpreter, ErrorsIterateIntegers) {
	ExpectError(R"(\A a \in Z a=a)", ValueEID::iterateInfinity, 9);
}