#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/Auditor.hpp"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/RSGenerator.h"

class UTAuditor : public ::testing::Test {
protected:
	using Typification = ccl::rslang::Typification;
	using TestAuditor = ccl::rslang::Auditor<ccl::rslang::AsciiLexer>;
	using LogicT = ccl::rslang::LogicT;

protected:
	UTAuditor();

	RSEnvironment env{};
	std::unique_ptr<TestAuditor> checker{ nullptr };
};

UTAuditor::UTAuditor() {
	checker = std::make_unique<TestAuditor>(env, env.GetValueContext(), env.GetAST());
	env.InsertBase("X1");
	env.Insert("S1", "B(X1*X1)"_t);
}

TEST_F(UTAuditor, EmptyInput) {
	EXPECT_FALSE(checker->CheckType({}));
	EXPECT_TRUE(std::holds_alternative<ccl::rslang::LogicT>(checker->GetType()));
}

TEST_F(UTAuditor, CheckCorrect) {
	ASSERT_TRUE(checker->CheckType("1=1"));
	EXPECT_TRUE(std::holds_alternative<LogicT>(checker->GetType()));

	ASSERT_TRUE(checker->CheckType("1+1"));
	EXPECT_EQ(std::get<Typification>(checker->GetType()), "Z"_t);

	ASSERT_TRUE(checker->CheckType(R"(X1 \setminus X1)"));
	EXPECT_EQ(std::get<Typification>(checker->GetType()), "B(X1)"_t);
}