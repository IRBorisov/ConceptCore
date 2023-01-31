#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/lang/EntityTermContext.hpp"
#include "ccl/lang/LexicalTerm.h"

using ccl::lang::EntityTermContext;

struct DummyContext : EntityTermContext {};

TEST(UTEntityTermContext, DefaultMethods) {
	DummyContext context{};

	EXPECT_EQ(nullptr, context.At(std::string{}));
	EXPECT_FALSE(context.Contains(std::string{}));
	EXPECT_TRUE(std::empty(context.MatchingTerms(std::string{})));
}