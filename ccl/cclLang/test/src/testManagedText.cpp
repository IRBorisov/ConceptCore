#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RefHelper.hpp"
#include "FakeTermContext.hpp"

#include "ccl/lang/ManagedText.h"
#include "ccl/lang/Reference.h"
#include "ccl/lang/TextEnvironment.h"

class UTManagedText : public ::testing::Test {
protected:
  using ManagedText = ccl::lang::ManagedText;
  using LexicalTerm = ccl::lang::LexicalTerm;

protected:
  UTManagedText();

  FakeContext context{};
};

UTManagedText::UTManagedText() {
  context.Insert("X1", LexicalTerm{ "Test" });
}

TEST_F(UTManagedText, EmptyCache) {
  ManagedText text("123", std::string{});
  EXPECT_EQ(text.Str(), "123");
  EXPECT_EQ(text.Raw(), "123");
}

TEST_F(UTManagedText, Comparison) {
  ManagedText t1{};
  ManagedText t2{ "@test", "1test" };
  ManagedText t3{ "@test", "2test" };
  ManagedText t4{ "@@test", "1test" };
  ManagedText t5{ "@@test" };
  EXPECT_NE(t1, t2);
  EXPECT_EQ(t2, t3);
  EXPECT_NE(t2, t4);
  EXPECT_NE(t3, t4);
  EXPECT_EQ(t4, t5);
}

TEST_F(UTManagedText, Str) {
  std::string text{ "test" };
  auto modified = text + "123";
  EXPECT_EQ(ManagedText(text).Str(), text);
  EXPECT_EQ(ManagedText(text, modified).Str(), modified);
  EXPECT_EQ(ManagedText(modified, text).Str(), text);
}

TEST_F(UTManagedText, SetRaw) {
  ManagedText text{ "42", "43" };
  text.SetRaw("44");
  EXPECT_EQ(text.Raw(), "44");
  EXPECT_EQ(text.Str(), "44");
}

TEST_F(UTManagedText, UpdateFrom) {
  ManagedText text{ tccl::EntityRef("X1"), "cache" };
  EXPECT_EQ(text.Str(), "cache");
  text.UpdateFrom(context);
  EXPECT_EQ(text.Str(), "Test");
}

TEST_F(UTManagedText, UpdateFromSkipResolution) {
  ManagedText text{ tccl::EntityRef("X1"), "cache" };
  EXPECT_EQ(text.Str(), "cache");
  ccl::lang::TextEnvironment::Instance().skipResolving = true;
  text.UpdateFrom(context);
  ccl::lang::TextEnvironment::Instance().skipResolving = false;
  EXPECT_EQ(text.Str(), "cache");
}

TEST_F(UTManagedText, TranslateRaw) {
  ManagedText text{ tccl::EntityRef("X2"), "cache" };
  text.TranslateRaw(ccl::CreateTranslator({ {"X2", "X1"} }));
  EXPECT_EQ(text.Raw(), tccl::EntityRef("X1"));
  EXPECT_EQ(text.Str(), "cache");
}

TEST_F(UTManagedText, Referals) {
  const ManagedText input{ tccl::EntityRef("X1") };
  const std::unordered_set<std::string> result{ "X1" };
  EXPECT_EQ(input.Referals(), result);
}

TEST_F(UTManagedText, ReferalsInvalid) {
  EXPECT_EQ(ssize(ManagedText("X1").Referals()), 0);
  EXPECT_EQ(ssize(ManagedText("@{X1}").Referals()), 0);
}

TEST_F(UTManagedText, ReferalsMultiple) {
  const ManagedText text{ "@{X2|nomn,sing} text @{abc|nomn,sing} X4 "
                  "@{-1|testing} @{X1|nomn,sing} @{X2,datv,sing}" };
  EXPECT_EQ(text.Referals(), std::unordered_set<std::string>({ "X2", "abc", "X1" }));
}

TEST_F(UTManagedText, ReferalsRepeat) {
  const ManagedText text{ tccl::EntityRef("X1") + " " + tccl::EntityRef("X1") };
  EXPECT_EQ(ssize(text.Referals()), 1);
}

TEST_F(UTManagedText, TranslateRefs) {
  {
    ManagedText text{ tccl::EntityRef("X2"), "cache" };
    text.TranslateRefs(ccl::CreateTranslator({ {"X2", "X1" } }), context);
    EXPECT_EQ(text.Raw(), tccl::EntityRef("X1"));
    EXPECT_EQ(text.Str(), "Test");
  }
  {
    ManagedText text{ tccl::EntityRef("X1") + " " + tccl::EntityRef("X2") };
    text.TranslateRefs(ccl::CreateTranslator({ { "X1", "X11" } }), context);
    EXPECT_EQ(text.Raw(), tccl::EntityRef("X11") + " " + tccl::EntityRef("X2"));
  }
  {
    ManagedText text{ tccl::EntityRef("X11") + " " + tccl::EntityRef("X2") };
    text.TranslateRefs(ccl::CreateTranslator({ { "X11", "X1" } }), context);
    EXPECT_EQ(text.Raw(), tccl::EntityRef("X1") + " " + tccl::EntityRef("X2"));
  }
  {
    ManagedText text{ tccl::EntityRef("X11") + " \xE2\x84\xAC " + tccl::EntityRef("X21") + " \xE2\x84\xAC " + tccl::EntityRef("X3") };
    auto translator = ccl::CreateTranslator({ { "X11", "X1" }, { "X21", "X2" },  { "X3", "X4" } });
    text.TranslateRefs(translator, context);
    EXPECT_EQ(text.Raw(), tccl::EntityRef("X1") + " \xE2\x84\xAC " + tccl::EntityRef("X2") + " \xE2\x84\xAC " + tccl::EntityRef("X4"));
  }
}