#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"
#include "ccl/semantic/Thesaurus.h"

class UTThesaurus : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using Thesaurus = ccl::semantic::Thesaurus;
  using TextConcept = ccl::semantic::TextConcept;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ManagedText = ccl::lang::ManagedText;
  using Morphology = ccl::lang::Morphology;
  using Grammem = ccl::lang::Grammem;

protected:
  Thesaurus core{};

  static constexpr EntityUID x1{ 42 };
  static constexpr EntityUID x2{ 43 };
  static constexpr EntityUID invalid{ 13 };
};

TEST_F(UTThesaurus, Construction) {
  {
    EXPECT_EQ(std::size(core), 0U);
    EXPECT_FALSE(core.Contains(x1));
    EXPECT_FALSE(core.Contains(x2));
  }

  core.Emplace(x1, "X1");
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1") });
  EXPECT_TRUE(core.TermGraph().ConnectionExists(x1, x2));
  {
    Thesaurus copy{ core };
    ASSERT_EQ(std::size(copy), 2U);
    ASSERT_TRUE(copy.Contains(x1));
    EXPECT_EQ(copy.At(x1), core.At(x1));
    ASSERT_TRUE(copy.Contains(x2));
    EXPECT_EQ(copy.At(x2), core.At(x2));
    EXPECT_TRUE(copy.TermGraph().ConnectionExists(x1, x2));
  }
  {
    Thesaurus copyCtor{};
    copyCtor = core;
    ASSERT_EQ(std::size(copyCtor), 2U);
    ASSERT_TRUE(copyCtor.Contains(x1));
    EXPECT_EQ(copyCtor.At(x1), core.At(x1));
    ASSERT_TRUE(copyCtor.Contains(x2));
    EXPECT_EQ(copyCtor.At(x2), core.At(x2));
    EXPECT_TRUE(copyCtor.TermGraph().ConnectionExists(x1, x2));
  }
  {
    Thesaurus temp{ core };
    Thesaurus movedCopy{ std::move(temp) };
    ASSERT_EQ(std::size(movedCopy), 2U);
    ASSERT_TRUE(movedCopy.Contains(x1));
    EXPECT_EQ(movedCopy.At(x1), core.At(x1));
    ASSERT_TRUE(movedCopy.Contains(x2));
    EXPECT_EQ(movedCopy.At(x2), core.At(x2));
    EXPECT_TRUE(movedCopy.TermGraph().ConnectionExists(x1, x2));
  }
  {
    Thesaurus temp{ core };
    Thesaurus moveAss{};
    moveAss = std::move(temp);
    ASSERT_EQ(std::size(moveAss), 2U);
    ASSERT_TRUE(moveAss.Contains(x1));
    EXPECT_EQ(moveAss.At(x1), core.At(x1));
    ASSERT_TRUE(moveAss.Contains(x2));
    EXPECT_EQ(moveAss.At(x2), core.At(x2));
    EXPECT_TRUE(moveAss.TermGraph().ConnectionExists(x1, x2));
  }
}

TEST_F(UTThesaurus, STLapi) {
  {
    EXPECT_TRUE(std::empty(core));
    EXPECT_EQ(std::size(core), 0U);
    EXPECT_EQ(std::begin(core), std::end(core));
  }

  core.Emplace(x1, "X1");
  core.Emplace(x2, "X2");
  {
    ASSERT_EQ(std::size(core), 2U);
    EXPECT_FALSE(std::empty(core));
    EXPECT_NE(std::begin(core), std::end(core));
  }
  {
    std::unordered_map<EntityUID, std::string> expected{};
    expected.emplace(x1, "X1");
    expected.emplace(x2, "X2");

    std::unordered_map<EntityUID, std::string>  result{};
    for (const auto& cst : core) {
      result.emplace(cst.uid, cst.alias);
    }
    EXPECT_EQ(result, expected);
  }
}

TEST_F(UTThesaurus, Contains) {
  EXPECT_FALSE(core.Contains(x1));

  core.Emplace(x1, "X1");
  EXPECT_TRUE(core.Contains(x1));
  EXPECT_FALSE(core.Contains(invalid));
}

TEST_F(UTThesaurus, Access) {
  core.Emplace(x1, "X1");
  ASSERT_TRUE(core.Contains(x1));
  EXPECT_EQ(core.At(x1).alias, "X1");
}

TEST_F(UTThesaurus, FindAlias) {
  EXPECT_FALSE(core.FindAlias("X1").has_value());

  core.Emplace(x1, "X1");
  {
    EXPECT_FALSE(core.FindAlias("X2").has_value());
    ASSERT_TRUE(core.FindAlias("X1").has_value());
    EXPECT_EQ(core.FindAlias("X1").value(), x1);
  }

  core.Emplace(x2, "X1");
  {
    EXPECT_FALSE(core.FindAlias("X2").has_value());
    ASSERT_TRUE(core.FindAlias("X1").has_value());
    EXPECT_TRUE(core.FindAlias("X1").value() == x1 || core.FindAlias("X1").value() == x2);
  }
}

TEST_F(UTThesaurus, Emplace) {
  {
    Thesaurus test{};
    EXPECT_TRUE(test.Emplace(x1, "X1"));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, x1);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).term.Nominal(), "");
    EXPECT_EQ(test.At(x1).definition.Raw(), "");
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1", LexicalTerm{"term"});
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, x1);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).term.Nominal(), "term");
    EXPECT_EQ(test.At(x1).definition.Raw(), "");
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ "definition" });
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, x1);
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x1).term.Nominal(), "term");
    EXPECT_EQ(test.At(x1).definition.Raw(), "definition");
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1");
    EXPECT_FALSE(test.Emplace(x1, "X2"));
    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).alias, "X1");
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1");
    EXPECT_TRUE(test.Emplace(x2, "X1"));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).alias, "X1");
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).alias, "X1");
  }
}

TEST_F(UTThesaurus, InsertCopy) {
  const TextConcept cst{ x1, "X1", LexicalTerm{"term"}, ManagedText{"definition"} };
  {
    Thesaurus test{};
    EXPECT_TRUE(test.InsertCopy(cst));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, cst.uid);
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).term, cst.term);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Thesaurus test{};
    test.InsertCopy(cst);
    TextConcept modified = cst;
    modified.definition = ManagedText("another definition");
    EXPECT_FALSE(test.InsertCopy(modified));
    EXPECT_EQ(1u, std::size(test));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(cst.definition, test.At(x1).definition);
  }
  {
    Thesaurus test{};
    test.InsertCopy(cst);
    TextConcept modified = cst;
    modified.uid = x2;
    modified.definition = ManagedText("another definition");
    EXPECT_TRUE(test.InsertCopy(modified));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).definition, modified.definition);
  }
}

TEST_F(UTThesaurus, Insert) {
  const TextConcept cst{ x1, "X1", LexicalTerm{"term"}, ManagedText{"definition"} };
  {
    Thesaurus test{};
    EXPECT_TRUE(test.Insert(TextConcept{ cst }));
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, cst.uid);
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).term, cst.term);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Thesaurus test{};
    test.Insert(TextConcept{ cst });
    TextConcept modified = cst;
    modified.definition = ManagedText("another definition");
    EXPECT_FALSE(test.Insert(std::move(modified)));
    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Thesaurus test{};
    test.Insert(TextConcept{ cst });
    TextConcept modified = cst;
    modified.uid = x2;
    modified.definition = ManagedText("another definition");
    EXPECT_TRUE(test.Insert(std::move(modified)));
    EXPECT_EQ(std::size(test), 2U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition, cst.definition);
    ASSERT_TRUE(test.Contains(x2));
    EXPECT_EQ(test.At(x2).definition, ManagedText("another definition"));
  }
}

TEST_F(UTThesaurus, Load) {
  const TextConcept cst{ x1, "X1", LexicalTerm{"term"}, ManagedText{"definition"} };
  {
    Thesaurus test{};
    test.Load(TextConcept{ cst });
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).uid, cst.uid);
    EXPECT_EQ(test.At(x1).alias, cst.alias);
    EXPECT_EQ(test.At(x1).term, cst.term);
    EXPECT_EQ(test.At(x1).definition, cst.definition);
  }
  {
    Thesaurus test{};
    test.Insert(TextConcept{ cst });

    TextConcept modified = cst;
    modified.definition = ManagedText("another definition");
    test.Load(std::move(modified));

    EXPECT_EQ(std::size(test), 1U);
    ASSERT_TRUE(test.Contains(x1));
    EXPECT_EQ(test.At(x1).definition.Str(), "another definition");
  }
}

TEST_F(UTThesaurus, Erase) {
  ASSERT_NO_THROW(core.Erase(invalid));
  core.Emplace(x1, "X1");
  core.Emplace(x2, "X2");
  EXPECT_EQ(std::size(core), 2U);
  EXPECT_TRUE(core.Contains(x1));
  EXPECT_TRUE(core.Contains(x2));

  core.Erase(x1);
  EXPECT_EQ(std::size(core), 1U);
  EXPECT_FALSE(core.Contains(x1));
  EXPECT_TRUE(core.Contains(x2));
}

TEST_F(UTThesaurus, ContextContains) {
  core.Emplace(x1, "X1");
  EXPECT_EQ(core.Context().Contains("X1"), core.Contains(x1));
  EXPECT_EQ(core.Context().Contains("invalid"), core.Contains(invalid));
}

TEST_F(UTThesaurus, ContextAt) {
  core.Emplace(x1, "X1", LexicalTerm{"term"});
  EXPECT_TRUE(core.Context().At("invalid") == nullptr);
  ASSERT_TRUE(core.Context().Contains("X1"));
  EXPECT_EQ(*core.Context().At("X1"), LexicalTerm{ "term" });
}

TEST_F(UTThesaurus, ContextMatching) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" });
  core.Emplace(x2, "X2", LexicalTerm{ "abc" }, ManagedText{"using term"});
  core.Emplace(invalid, "X3");
  std::unordered_map<std::string, std::string> expected{ {"X1", "term"} };
  EXPECT_EQ(core.Context().MatchingTerms("term"), expected);
}

TEST_F(UTThesaurus, ContextMatchingNameDuplicates) {
  core.Emplace(x1, "X1", LexicalTerm{ "simple term" });
  core.Emplace(x2, "X1", LexicalTerm{ "complex term" });
  std::unordered_map<std::string, std::string> expected{ {"X1", "simple term"}, {"X1", "complex term"} };
  const auto result = core.Context().MatchingTerms("term");
  EXPECT_TRUE(std::is_permutation(begin(expected), end(expected), begin(result), end(result)));
}

TEST_F(UTThesaurus, TermGraph) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" });
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1") });
  EXPECT_TRUE(core.TermGraph().IsReachableFrom(x2, x1));
  EXPECT_FALSE(core.TermGraph().IsReachableFrom(x1, x2));
  EXPECT_FALSE(core.TermGraph().IsReachableFrom(invalid, x1));
  EXPECT_FALSE(core.TermGraph().IsReachableFrom(x1, invalid));
}

TEST_F(UTThesaurus, DefGraph) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" });
  core.Emplace(x2, "X2", LexicalTerm{ "another" }, ManagedText{ tccl::EntityRef("X1") });
  EXPECT_TRUE(core.DefGraph().IsReachableFrom(x2, x1));
  EXPECT_FALSE(core.DefGraph().IsReachableFrom(x1, x2));
  EXPECT_FALSE(core.DefGraph().IsReachableFrom(invalid, x1));
  EXPECT_FALSE(core.DefGraph().IsReachableFrom(x1, invalid));
}

TEST_F(UTThesaurus, SetName) {
  {
    EXPECT_FALSE(core.SetAliasFor(invalid, "X1"));
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1");
    EXPECT_FALSE(test.SetAliasFor(x1, "X1"));
    EXPECT_TRUE(test.SetAliasFor(x1, "X2"));
    EXPECT_EQ(test.At(x1).alias, "X2");
  }
  {
    Thesaurus test{};
    test.Emplace(x1, "X1");
    test.Emplace(x2, "X2");
    EXPECT_TRUE(test.SetAliasFor(x1, "X2"));
    EXPECT_EQ(test.At(x1).alias, "X2");
    EXPECT_EQ(test.At(x2).alias, "X2");
  }
}

TEST_F(UTThesaurus, SetNameSubstitute) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ tccl::EntityRef("X1") });
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1") }, ManagedText{ tccl::EntityRef("X1") });
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x1, x1));
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x1, x2));
  EXPECT_TRUE(core.TermGraph().ConnectionExists(x1, x2));

  {
    Thesaurus test{ core };
    EXPECT_TRUE(test.SetAliasFor(x1, "X3", true));
    EXPECT_EQ(test.At(x1).alias, "X3");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X3"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X3"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X3"));
    EXPECT_EQ(test.At(x1).definition.Str(), "term");
    EXPECT_EQ(test.At(x2).definition.Str(), "term");
    EXPECT_EQ(test.At(x2).term.Nominal(), "term");
    EXPECT_TRUE(test.DefGraph().ConnectionExists(x1, x1));
    EXPECT_TRUE(test.DefGraph().ConnectionExists(x1, x2));
    EXPECT_TRUE(test.TermGraph().ConnectionExists(x1, x2));
  }
  {
    Thesaurus test{ core };
    EXPECT_TRUE(test.SetAliasFor(x1, "X3", false));
    EXPECT_EQ(test.At(x1).alias, "X3");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X1"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X1"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X1"));
    EXPECT_NE(test.At(x1).definition.Str(), "term");
    EXPECT_NE(test.At(x2).definition.Str(), "term");
    EXPECT_NE(test.At(x2).term.Nominal(), "term");
    EXPECT_FALSE(test.DefGraph().ConnectionExists(x1, x1));
    EXPECT_FALSE(test.DefGraph().ConnectionExists(x1, x2));
    EXPECT_FALSE(test.TermGraph().ConnectionExists(x1, x2));
  }
}

TEST_F(UTThesaurus, SetTerm) {
  EXPECT_FALSE(core.SetTermFor(invalid, "term2"));
  EXPECT_FALSE(core.SetTermFor(invalid, ""));

  core.Emplace(x1, "X1", LexicalTerm{ "term" });
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1") });
  EXPECT_FALSE(core.SetTermFor(x1, "term"));
  EXPECT_TRUE(core.SetTermFor(x1, "term2"));
  EXPECT_EQ(core.At(x1).term.Nominal(), "term2");
  EXPECT_EQ(core.At(x2).term.Nominal(), "term2");
}

TEST_F(UTThesaurus, SetTermForm) {
  using namespace ccl::lang;
  const Morphology testForm{ Grammem::plur, Grammem::datv };
  EXPECT_FALSE(core.SetTermFormFor(invalid, "term2", testForm));
  EXPECT_FALSE(core.SetTermFormFor(invalid, "", testForm));

  core.Emplace(x1, "X1", LexicalTerm{ "term" });
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1", testForm) });
  EXPECT_TRUE(core.SetTermFormFor(x1, "terma", testForm));
  EXPECT_FALSE(core.SetTermFormFor(x1, "terma", testForm));
  EXPECT_EQ(core.At(x1).term.Nominal(), "term");
  EXPECT_EQ(core.At(x1).term.GetForm(testForm), "terma");
  EXPECT_EQ(core.At(x2).term.Nominal(), "terma");
}

TEST_F(UTThesaurus, SetDefinition) {
  EXPECT_FALSE(core.SetDefinitionFor(invalid, "def2"));
  EXPECT_FALSE(core.SetDefinitionFor(invalid, ""));

  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ "def" });
  EXPECT_FALSE(core.SetDefinitionFor(x1, "def"));
  EXPECT_TRUE(core.SetDefinitionFor(x1, "def2"));
  EXPECT_EQ(core.At(x1).definition.Str(), "def2");
}

TEST_F(UTThesaurus, UpdateOnInsert) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ tccl::EntityRef("X2") });
  core.Emplace(x2, "X2", LexicalTerm{ "newTerm" }, ManagedText{ tccl::EntityRef("X1") });
  EXPECT_EQ(core.At(x1).definition.Str(), "newTerm");
  EXPECT_EQ(core.At(x2).definition.Str(), "term");
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x1, x2));
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x2, x1));
}

TEST_F(UTThesaurus, UpdateOnLoad) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ tccl::EntityRef("X2") });
  core.Load(TextConcept{ x2, "X2", LexicalTerm{"newTerm"}, ManagedText{ tccl::EntityRef("X1") } });
  EXPECT_NE(core.At(x1).definition.Str(), "newTerm");
  EXPECT_NE(core.At(x2).definition.Str(), "term");
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x1, x2));
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x2, x1));
}

TEST_F(UTThesaurus, UpdateOnErase) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ tccl::EntityRef("X2") });
  core.Emplace(x2, "X2", LexicalTerm{ "newTerm" });
  EXPECT_EQ(core.At(x1).definition.Str(), "newTerm");
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x2, x1));

  core.Erase(x2);
  EXPECT_NE(core.At(x1).definition.Str(), "newTerm");
  EXPECT_FALSE(core.DefGraph().ConnectionExists(x2, x1));
}

TEST_F(UTThesaurus, UpdateState) {
  core.Emplace(x1, "X1", LexicalTerm{ "term" }, ManagedText{ tccl::EntityRef("X2") });
  core.Load(TextConcept{ x2, "X2", LexicalTerm{"newTerm"}, ManagedText{ tccl::EntityRef("X1") } });
  EXPECT_NE(core.At(x1).definition.Str(), "newTerm");
  EXPECT_NE(core.At(x2).definition.Str(), "term");

  core.UpdateState();
  EXPECT_EQ(core.At(x1).definition.Str(), "newTerm");
  EXPECT_EQ(core.At(x2).definition.Str(), "term");
}

TEST_F(UTThesaurus, Translations) {
  core.Emplace(x1, "X1", LexicalTerm{ "X1 term" }, ManagedText{ tccl::EntityRef("X3") });
  core.Emplace(x2, "X2", LexicalTerm{ tccl::EntityRef("X1") }, ManagedText{ tccl::EntityRef("X2") });
  EXPECT_TRUE(core.TermGraph().ConnectionExists(x1, x2));
  EXPECT_TRUE(core.DefGraph().ConnectionExists(x2, x2));

  const auto translator = ccl::CreateTranslator({ {"X1", "X2"}, {"X2", "X3"}, {"X3", "X4"} });
  {
    Thesaurus test = core;
    test.SubstitueAliases(translator);
    EXPECT_TRUE(test.TermGraph().ConnectionExists(x1, x2));
    EXPECT_EQ(test.At(x1).alias, "X2");
    EXPECT_EQ(test.At(x2).alias, "X3");
    EXPECT_EQ(test.At(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X4"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
  {
    Thesaurus test = core;
    test.TranslateAll(translator);
    EXPECT_FALSE(test.TermGraph().ConnectionExists(x1, x2));
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x2).alias, "X2");
    EXPECT_EQ(test.At(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X4"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
  {
    Thesaurus test = core;
    test.Translate(x2, translator);
    EXPECT_FALSE(test.TermGraph().ConnectionExists(x1, x2));
    EXPECT_EQ("X1", test.At(x1).alias);
    EXPECT_EQ("X2", test.At(x2).alias);
    EXPECT_EQ("X1 term", test.At(x1).term.Text().Raw());
    EXPECT_EQ(tccl::EntityRef("X3"), test.At(x1).definition.Raw());
    EXPECT_EQ(tccl::EntityRef("X2"), test.At(x2).term.Text().Raw());
    EXPECT_EQ(tccl::EntityRef("X3"), test.At(x2).definition.Raw());
  }
  {
    Thesaurus test = core;
    test.TranslateTerm(x2, translator);
    EXPECT_FALSE(test.TermGraph().ConnectionExists(x1, x2));
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x2).alias, "X2");
    EXPECT_EQ(test.At(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X3"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X2"));
  }
  {
    Thesaurus test = core;
    test.TranslateDef(x2, translator);
    EXPECT_TRUE(test.TermGraph().ConnectionExists(x1, x2));
    EXPECT_EQ(test.At(x1).alias, "X1");
    EXPECT_EQ(test.At(x2).alias, "X2");
    EXPECT_EQ(test.At(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.At(x1).definition.Raw(), tccl::EntityRef("X3"));
    EXPECT_EQ(test.At(x2).term.Text().Raw(), tccl::EntityRef("X1"));
    EXPECT_EQ(test.At(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
}

TEST_F(UTThesaurus, Homonyms) {
  {
    EXPECT_FALSE(core.HasHomonyms());
    EXPECT_TRUE(std::empty(core.GetHomonyms()));
  }
  {
    core.Emplace(x1, "X1");
    core.Emplace(x2, "X2");
    EXPECT_FALSE(core.HasHomonyms());
    EXPECT_TRUE(std::empty(core.GetHomonyms()));
  }
  {
    core.SetTermFor(x1, "term");
    core.SetTermFor(x2, "term");
    EXPECT_TRUE(core.HasHomonyms());
    const auto substitutes = core.GetHomonyms();
    ASSERT_EQ(std::size(substitutes), 1U);
    EXPECT_TRUE((begin(substitutes)->first == "X1" && begin(substitutes)->second == "X2")
          || (begin(substitutes)->first == "X2" && begin(substitutes)->second == "X1"));
  }
}