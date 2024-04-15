#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/Interpreter.h"

class UTInterpreter : public ::testing::Test {
protected:
  using StructuredData = ccl::object::StructuredData;
  using Factory = ccl::object::Factory;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using Typification = ccl::rslang::Typification;
  using TypedID = ccl::rslang::TypedID;

  using TestInterpreter = ccl::rslang::Interpreter;

protected:
  UTInterpreter();

  RSEnvironment env{};
  TestInterpreter interpreter;
};

UTInterpreter::UTInterpreter() : interpreter{ env, env.GetAST(), env.GetDataContext() } {
  env.InsertBase("X1");
  env.data["X1"].objects = Factory::SetV({ 1, 2, 3 });
}

TEST_F(UTInterpreter, EvaluateExpression) {
  EXPECT_FALSE(interpreter.Evaluate(std::string{}).has_value());
  EXPECT_FALSE(interpreter.Evaluate(R"(X1 \defexpr)").has_value());
  EXPECT_FALSE(interpreter.Evaluate(R"(X1:==)").has_value());
  EXPECT_FALSE(interpreter.Evaluate(R"(invalid \eq 1)").has_value());
  EXPECT_EQ(std::get<StructuredData>(interpreter.Evaluate(R"(X1 \setminus X1)").value()), Factory::EmptySet());
  EXPECT_EQ(std::get<bool>(interpreter.Evaluate(R"(1 \eq 1)").value()), true);
  EXPECT_EQ(std::get<bool>(interpreter.Evaluate(R"(1=1)").value()), true);
}

TEST_F(UTInterpreter, Errors) {
  EXPECT_FALSE(interpreter.Evaluate(R"(X2 \setminus X2)").has_value());
  EXPECT_EQ(size(interpreter.Errors().All()), 1U);
  EXPECT_EQ(interpreter.Errors().FirstErrorPos(), 0);
  EXPECT_EQ(interpreter.Iterations(), 0);
}

TEST_F(UTInterpreter, Functions) {
  EXPECT_FALSE(interpreter.Evaluate("F1[X1]").has_value());
  
  env.data["F1"] = {};
  EXPECT_FALSE(interpreter.Evaluate("F1[X1]").has_value());

  ASSERT_TRUE(env.AddAST("F1", R"(F1 \defexpr [a \in B(X1)] {a})"));

  env.data["F1"].type = "B(X1)"_t;
  env.data["F1"].arguments = { TypedID{ "a", "B(X1)"_t } };
  EXPECT_TRUE(interpreter.Evaluate("F1[X1]").has_value());
}