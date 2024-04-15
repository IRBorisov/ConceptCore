#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RefHelper.hpp"
#include "FakeTextProcessor.hpp"
#include "FakeTermContext.hpp"

#include "ccl/Substitutes.hpp"
#include "ccl/lang/EntityTermContext.hpp"
#include "ccl/lang/Literals.h"

using ccl::lang::operator""_form;

class UTLexcicalTerm : public ::testing::Test {
protected:
  FakeContext cntxt{};

protected:
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ManagedText = ccl::lang::ManagedText;
  using Morphology = ccl::lang::Morphology;
  using TextEnvironment = ccl::lang::TextEnvironment;

  void SetUp() override {
    cntxt.Insert("X1", LexicalTerm{ "Test" });
  }
  void TearDown() override {
    TextEnvironment::SetProcessor(std::make_unique<ccl::lang::TextProcessor>());
  }
};

TEST_F(UTLexcicalTerm, DefaultParams) {
  const LexicalTerm term{};
  EXPECT_EQ(term, LexicalTerm());
  EXPECT_EQ(term.Text(), ManagedText());
  EXPECT_TRUE(term.MatchStr(""));
}

TEST_F(UTLexcicalTerm, Access) {
  const LexicalTerm term{"raw", "cached"};
  EXPECT_EQ(term.Nominal(), "cached");
  EXPECT_EQ(term.Text().Str(), "cached");
  EXPECT_EQ(term.Text().Raw(), "raw");
}

TEST_F(UTLexcicalTerm, SetText) {
  LexicalTerm term{};
  term.SetText("generic", cntxt);
  EXPECT_EQ(term.Nominal(), "generic");

  term.SetText("generic " + FakeTextProcessor::x1Ref, cntxt);
  EXPECT_EQ(term.Nominal(), "generic Test");
}

TEST_F(UTLexcicalTerm, UpdateContext) {
  auto term = LexicalTerm{ FakeTextProcessor::x1Ref };
  EXPECT_EQ(FakeTextProcessor::x1Ref, term.Nominal());
  term.UpdateFrom(cntxt);
  EXPECT_EQ(term.Nominal(), "Test");
}

TEST_F(UTLexcicalTerm, SetForm) {
  LexicalTerm term{"test", "test"};
  EXPECT_EQ(term.Nominal(), "test");
  EXPECT_EQ(term.GetForm(Morphology{}), "test");

  term.SetForm(Morphology{}, "42");
  EXPECT_EQ(term.Nominal(), "test");
  EXPECT_EQ(term.GetForm(Morphology{}), "42");

  term.SetForm("sing,ablt"_form, "43");
  EXPECT_EQ(term.Nominal(), "test");
  EXPECT_EQ(term.GetForm(Morphology{}), "42");
  EXPECT_EQ(term.GetForm("sing,ablt"_form), "43");
}

TEST_F(UTLexcicalTerm, GetForm) {
  const auto nominalForm = "sing,nomn"_form;
  const auto manualForm = "sing,ablt"_form;
  const auto cachedForm1 = "sing,datv"_form;
  const auto cachedForm2 = "plur,datv"_form;
  LexicalTerm term{ "42", "42" };
  term.SetForm(manualForm, "43");
  EXPECT_EQ(term.GetForm(nominalForm), "42");
  EXPECT_EQ(term.GetForm(manualForm), "43");
  EXPECT_EQ(term.GetForm(cachedForm1), "42");
  EXPECT_EQ(term.GetForm(cachedForm1), "42");

  term.ClearForms();
  term.SetForm(nominalForm, "44");
  EXPECT_EQ(term.GetForm(cachedForm1), "42");
  EXPECT_EQ(term.GetForm(cachedForm2), "42");
}

TEST_F(UTLexcicalTerm, GetNominalForm) {
  const auto nominalForm = "sing,nomn"_form;
  LexicalTerm term{ "42", "42" };
  term.SetForm(nominalForm, "43");
  EXPECT_EQ(term.GetNominalForm(), "43");
}

TEST_F(UTLexcicalTerm, IsManualForm) {
  LexicalTerm term{ "test", "test" };
  EXPECT_FALSE(term.IsFormManual(Morphology{}));

  term.SetForm({}, "42");
  EXPECT_TRUE(term.IsFormManual(Morphology{}));

  term.SetForm({}, "test");
  EXPECT_TRUE(term.IsFormManual(Morphology{}));
}

TEST_F(UTLexcicalTerm, TranslateRefs) {
  const auto manualForm = "sing,ablt"_form;
  LexicalTerm term{ tccl::EntityRef("X2"), "cache" };
  term.SetForm(manualForm, "man");
  term.TranslateRefs(ccl::CreateTranslator({ { "X2", "X1" } }), cntxt);
  EXPECT_EQ("man", term.GetForm(manualForm));
  EXPECT_EQ("Test", term.Nominal());
  EXPECT_EQ(tccl::EntityRef("X1"), term.Text().Raw());
}

TEST_F(UTLexcicalTerm, TranslateRaw) {
  const auto manualForm = "sing,ablt"_form;
  LexicalTerm term{ tccl::EntityRef("X2"), "cache" };
  term.SetForm(manualForm, "man");
  term.TranslateRaw(ccl::CreateTranslator({ { "X2", "X1" } }));
  EXPECT_EQ("man", term.GetForm(manualForm));
  EXPECT_EQ("cache", term.Nominal());
  EXPECT_EQ(tccl::EntityRef("X1"), term.Text().Raw());
}