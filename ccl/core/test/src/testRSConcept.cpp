#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSConcept.h"

class UTRSConcept : public ::testing::Test {
protected:
  using RSConcept = ccl::semantic::RSConcept;
  using CstType = ccl::semantic::CstType;
  using EntityUID = ccl::EntityUID;
};

TEST_F(UTRSConcept, Construction) {
  {
    RSConcept cst{};
    EXPECT_EQ(cst.uid, EntityUID{ 0 });
    EXPECT_EQ(cst.alias, "");
    EXPECT_EQ(cst.type, CstType::base);
    EXPECT_EQ(cst.definition, "");
    EXPECT_EQ(cst.convention, "");
  }
  {
    RSConcept cst{ EntityUID{ 42 }, "X1", CstType::axiom, "definition", "convention" };
    EXPECT_EQ(cst.uid, EntityUID{ 42 });
    EXPECT_EQ(cst.alias, "X1");
    EXPECT_EQ(cst.type, CstType::axiom);
    EXPECT_EQ(cst.definition, "definition");
    EXPECT_EQ(cst.convention, "convention");
  }
  {
    RSConcept cst{ EntityUID{ 42 }, "aName" };
    EXPECT_EQ(cst.uid, EntityUID{ 42 });
    EXPECT_EQ(cst.alias, "aName");
    EXPECT_EQ(cst.type, CstType::base);
    EXPECT_EQ(cst.definition, "");
    EXPECT_EQ(cst.convention, "");
  }
}

TEST_F(UTRSConcept, IsEmpty) {
  {
    EXPECT_TRUE(RSConcept{}.IsEmpty());
  }
  {
    RSConcept cst{ EntityUID{ 42 }, "aName" };
    EXPECT_TRUE(cst.IsEmpty());
  }
  {
    RSConcept cst1{};
    cst1.definition = "42";
    EXPECT_FALSE(cst1.IsEmpty());
  }
  {
    RSConcept cst2{};
    cst2.alias = "42";
    EXPECT_TRUE(cst2.IsEmpty());
  }
  {
    RSConcept cst3{};
    cst3.convention = "42";
    EXPECT_FALSE(cst3.IsEmpty());
  }
}

TEST_F(UTRSConcept, MatchString) {
  {
    RSConcept cst1{};
    cst1.convention = "abc 42 cba";
    EXPECT_TRUE(cst1.MatchStr("42"));
    EXPECT_FALSE(cst1.MatchStr("43"));
  }
  {
    RSConcept cst2{};
    cst2.definition = "abc 42 cba";
    EXPECT_FALSE(cst2.MatchStr("42"));
    EXPECT_FALSE(cst2.MatchStr("43"));
  }
  {
    RSConcept cst3{};
    cst3.alias = "X2";
    EXPECT_FALSE(cst3.MatchStr("X2"));
    EXPECT_FALSE(cst3.MatchStr("X3"));
  }
}

TEST_F(UTRSConcept, Compare) {
  {
    EXPECT_EQ(RSConcept{}, RSConcept{});
  }

  RSConcept cst1{ EntityUID{0}, "X1" };
  RSConcept cst2{ EntityUID{42}, "X2" };
  
  {
    EXPECT_EQ(cst1, cst2);
  }
  {
    cst1.definition = "42";
    EXPECT_NE(cst1, cst2);
    cst2.definition = "42";
    EXPECT_EQ(cst1, cst2);
  }
  {
    cst1.convention = "42";
    EXPECT_NE(cst1, cst2);
    cst2.convention = "42";
    EXPECT_EQ(cst1, cst2);
  }
  {
    RSConcept a1{};
    a1.type = CstType::axiom;
    RSConcept s1{};
    s1.type = CstType::structured;
    EXPECT_NE(a1, s1);
  }
}

TEST_F(UTRSConcept, TranslateRS) {
  RSConcept cst{};
  cst.alias = "X2";
  cst.definition = R"(X2\X21)";
  cst.convention = "X2";

  ccl::StrSubstitutes substitutes{};
  substitutes.insert({ "X2", "X1" });
  cst.Translate(ccl::CreateTranslator(substitutes));

  EXPECT_EQ(cst.alias, "X2");
  EXPECT_EQ(cst.definition, R"(X1\X21)");
  EXPECT_EQ(cst.convention, "X1");
}