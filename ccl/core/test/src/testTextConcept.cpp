#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"
#include "FakeTermContext.hpp"

#include "ccl/semantic/TextConcept.h"

class UTTextConcept : public ::testing::Test {
protected:
  using TextConcept = ccl::semantic::TextConcept;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ManagedText = ccl::lang::ManagedText;
  using EntityUID = ccl::EntityUID;
};

TEST_F(UTTextConcept, Construction) {
  {
    TextConcept cst1{};
    EXPECT_EQ(cst1.uid, EntityUID{ 0 });
    EXPECT_EQ(cst1.alias, "");
    EXPECT_EQ(cst1.term.Nominal(), "");
    EXPECT_EQ(cst1.definition.Raw(), "");
  }
  {
    TextConcept cst2{ EntityUID{ 42 }, "X1" };
    EXPECT_EQ(cst2.uid, EntityUID{ 42 });
    EXPECT_EQ(cst2.alias, "X1");
    EXPECT_EQ(cst2.term.Nominal(), "");
    EXPECT_EQ(cst2.definition.Raw(), "");
  }
  {
    TextConcept cst3{ EntityUID{ 42 }, "X1", LexicalTerm{ "Ref" } };
    EXPECT_EQ(cst3.uid, EntityUID{ 42 });
    EXPECT_EQ(cst3.alias, "X1");
    EXPECT_EQ(cst3.term.Nominal(), "Ref");
    EXPECT_EQ(cst3.definition.Raw(), "");
  }
  {
    TextConcept cst4{ EntityUID{ 42 }, "X1", LexicalTerm{ "Term" }, ManagedText{ "Def" } };
    EXPECT_EQ(cst4.uid, EntityUID{ 42 });
    EXPECT_EQ(cst4.alias, "X1");
    EXPECT_EQ(cst4.term.Nominal(), "Term");
    EXPECT_EQ(cst4.definition.Raw(), "Def");
  }
}

TEST_F(UTTextConcept, IsEmpty) {
  {
    EXPECT_TRUE(TextConcept{}.IsEmpty());
  }
  {
    TextConcept cst{ EntityUID{ 42 }, "aName" };
    EXPECT_TRUE(cst.IsEmpty());
  }
  {
    TextConcept cst1{};
    cst1.term = LexicalTerm{ "Ref" };
    EXPECT_FALSE(cst1.IsEmpty());
  }
  {
    TextConcept cst2{};
    cst2.definition = ManagedText{ "Def" };
    EXPECT_FALSE(cst2.IsEmpty());
  }
}

TEST_F(UTTextConcept, MatchString) {
  {
    TextConcept cst1{};
    cst1.term = LexicalTerm{ "abc 42 cba" };
    EXPECT_TRUE(cst1.MatchStr("42"));
    EXPECT_FALSE(cst1.MatchStr("43"));
  }
  {
    TextConcept cst2{};
    cst2.definition.SetRaw("abc 42 cba");
    EXPECT_TRUE(cst2.MatchStr("42"));
    EXPECT_FALSE(cst2.MatchStr("43"));
  }
  {
    TextConcept cst3{};
    cst3.alias = "X2";
    EXPECT_FALSE(cst3.MatchStr("X2"));
    EXPECT_FALSE(cst3.MatchStr("X3"));
  }
}

TEST_F(UTTextConcept, Compare) {
  {
    EXPECT_EQ(TextConcept{}, TextConcept{});
  }

  TextConcept cst1{};
  TextConcept cst2{};
  {
    cst1.alias = "X1";
    cst2.alias = "X2";
    EXPECT_EQ(cst1, cst2);
  }
  {
    cst1.term = LexicalTerm{ "42" };
    EXPECT_NE(cst1, cst2);
    cst2.term = LexicalTerm{ "42" };
    EXPECT_EQ(cst1, cst2);
  }
  {
    cst1.definition = ManagedText{ "42" };
    EXPECT_NE(cst1, cst2);
    cst2.definition = ManagedText{ "42" };
    EXPECT_EQ(cst1, cst2);
  }
}

TEST_F(UTTextConcept, TranslateRaw) {
  FakeContext cntxt{};
  TextConcept cst{};
  cst.alias = "X2";
  cst.definition.SetRaw(tccl::EntityRef("X2"));
  cst.term.SetText(tccl::EntityRef("X2"), cntxt);
  cst.TranslateRaw(ccl::CreateTranslator({ { "X2", "X1" } }));
  EXPECT_EQ(cst.alias, "X2");
  EXPECT_EQ(cst.definition.Raw(), tccl::EntityRef("X1"));
  EXPECT_EQ(cst.term.Text().Raw(), tccl::EntityRef("X1"));
}

TEST_F(UTTextConcept, TranslateRefs) {
  FakeContext cntxt{};
  TextConcept cst{};
  cst.alias = "X2";
  cst.definition.SetRaw(tccl::EntityRef("X2"));
  cst.term.SetText(tccl::EntityRef("X2"), cntxt);
  cst.Translate(ccl::CreateTranslator({ { "X2", "X1" } }), cntxt);
  EXPECT_EQ(cst.alias, "X2");
  EXPECT_EQ(cst.definition.Raw(), tccl::EntityRef("X1"));
  EXPECT_EQ(cst.term.Text().Raw(), tccl::EntityRef("X1"));
}