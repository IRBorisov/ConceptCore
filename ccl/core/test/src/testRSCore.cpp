#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RandomGenerator.hpp"
#include "RSLHelper.hpp"

#include "ccl/semantic/RSCore.h"

class UTRSCore : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using RSCore = ccl::semantic::RSCore;
  using Constituenta = ccl::semantic::ConceptRecord;
  using CstType = ccl::semantic::CstType;
  using CstTypeIter = ccl::semantic::CstTypeIter;
  using ConceptRecord = ccl::semantic::ConceptRecord;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ParsingStatus = ccl::semantic::ParsingStatus;
  using ManagedText = ccl::lang::ManagedText;
  using Grammem = ccl::lang::Grammem;

protected:
  static constexpr EntityUID invalid{ 13 };

  RSCore core{};
};

TEST_F(UTRSCore, STLapi) {
  {
    EXPECT_TRUE(std::empty(core));
    EXPECT_EQ(std::size(core), 0U);
    EXPECT_EQ(std::begin(core), std::end(core));
  }

  const auto x1 = core.Emplace(CstType::base);
  const auto x2 = core.Emplace(CstType::base);
  {
    ASSERT_EQ(std::size(core), 2U);
    EXPECT_FALSE(std::empty(core));
    EXPECT_NE(std::begin(core), std::end(core));
  }

  {
    std::unordered_set<EntityUID> expected{ x1, x2 };
    std::unordered_set<EntityUID>  result{};
    for (const auto uid : core) {
      result.emplace(uid);
    }
    EXPECT_EQ(result, expected);
  }
}

TEST_F(UTRSCore, Contains) {
  EXPECT_FALSE(core.Contains(invalid));
  EXPECT_FALSE(core.RSLang().Contains(invalid));
  EXPECT_FALSE(core.Texts().Contains(invalid));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_TRUE(core.Contains(x1));
  EXPECT_TRUE(core.RSLang().Contains(x1));
  EXPECT_TRUE(core.Texts().Contains(x1));
  EXPECT_FALSE(core.Contains(EntityUID{ x1 + 1 }));
}

TEST_F(UTRSCore, FindAlias) {
  EXPECT_FALSE(core.FindAlias("X1").has_value());

  const auto x1 = core.Emplace(CstType::base);
  {
    EXPECT_FALSE(core.FindAlias("X2").has_value());
    ASSERT_TRUE(core.FindAlias("X1").has_value());
    EXPECT_EQ(core.FindAlias("X1").value(), x1);
    EXPECT_EQ(core.RSLang().FindAlias("X1").value(), x1);
    EXPECT_EQ(core.Texts().FindAlias("X1").value(), x1);
  }
}

TEST_F(UTRSCore, AccessCst) {
  const auto x1 = core.Emplace(CstType::base);
  EXPECT_EQ(&core.GetRS(x1), &core.RSLang().At(x1));
  EXPECT_EQ(&core.GetParse(x1), &core.RSLang().InfoFor(x1));
  EXPECT_EQ(&core.GetText(x1), &core.Texts().At(x1));
}

TEST_F(UTRSCore, AsRecord) {
  ConceptRecord cst1{};
  cst1.uid = core.Emplace(CstType::base, "X1=X1");
  cst1.type = CstType::base;
  cst1.alias = "X1";
  cst1.rs = "X1=X1";
  cst1.definition = ManagedText{ "def" };

  core.SetDefinitionFor(cst1.uid, "def");
  const auto generated = core.AsRecord(cst1.uid);
  EXPECT_EQ(cst1.uid, generated.uid);
  EXPECT_EQ(cst1.alias, generated.alias);
  EXPECT_EQ(cst1.type, generated.type);
  EXPECT_EQ(cst1.rs, generated.rs);
  EXPECT_EQ(cst1.convention, generated.convention);
  EXPECT_EQ(cst1.definition, generated.definition);
  EXPECT_EQ(cst1.term, generated.term);
}

TEST_F(UTRSCore, Emplace) {
  const auto x1 = core.Emplace(CstType::base);
  EXPECT_EQ(core.GetRS(x1).alias, "X1");
  EXPECT_EQ(core.GetText(x1).alias, "X1");

  const auto x2 = core.Emplace(CstType::base, "expr");
  EXPECT_EQ(core.GetRS(x2).alias, "X2");
  EXPECT_EQ(core.GetText(x2).alias, "X2");
  EXPECT_EQ(core.GetRS(x2).definition, "expr");
  EXPECT_EQ(core.GetText(x2).definition.Raw(), "");
}

TEST_F(UTRSCore, Insert) {
  const auto d1 = core.Emplace(CstType::term);
  core.SetExpressionFor(d1, "{X2}");
  core.SetDefinitionFor(d1, tccl::EntityRef("X2"));

  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X2";
  cst.term = LexicalTerm{ "term" };

  const auto x1 = core.Insert(ConceptRecord{ cst });
  EXPECT_EQ(core.GetRS(x1).alias, cst.alias);
  EXPECT_EQ(core.GetRS(x1).type, cst.type);
  EXPECT_EQ(core.GetText(x1).term, cst.term);
  EXPECT_EQ(core.GetParse(d1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.GetText(d1).definition.Str(), "term");
}

TEST_F(UTRSCore, InsertCopyID) {
  RSCore source{};
  const auto x1 = source.Emplace(CstType::base, "test");
  const auto copyX1 = core.InsertCopy(x1, source);
  EXPECT_EQ(core.GetRS(copyX1).definition, "test");
  EXPECT_EQ(core.GetRS(copyX1).alias, "X1");

  const auto copyBase = core.InsertCopy({ x1, x1 }, source);
  ASSERT_EQ(std::size(copyBase), 2U);
  EXPECT_EQ(core.GetRS(copyBase.at(0)).definition, "test");
  EXPECT_EQ(core.GetRS(copyBase.at(1)).definition, "test");
  EXPECT_EQ(core.GetRS(copyBase.at(0)).alias, "X2");
  EXPECT_EQ(core.GetRS(copyBase.at(1)).alias, "X3");
}

TEST_F(UTRSCore, InsertCopyRecord) {
  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X1";
  cst.rs = "test";

  const auto copyX1 = core.InsertCopy(cst);
  EXPECT_EQ(core.GetRS(copyX1).definition, "test");
  EXPECT_EQ(core.GetRS(copyX1).alias, "X1");

  const auto copyBase = core.InsertCopy({ cst, cst });
  ASSERT_EQ(std::size(copyBase), 2U);
  EXPECT_EQ(core.GetRS(copyBase.at(0)).definition, "test");
  EXPECT_EQ(core.GetRS(copyBase.at(1)).definition, "test");
  EXPECT_EQ(core.GetRS(copyBase.at(0)).alias, "X2");
  EXPECT_EQ(core.GetRS(copyBase.at(1)).alias, "X3");
}

TEST_F(UTRSCore, InsertRename) {
  core.Emplace(CstType::base);
  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X1";
  cst.term = LexicalTerm{ "term" };
  cst.definition = ManagedText{ tccl::EntityRef("X1") };
  cst.rs = "X1=X1";

  const auto x2 = core.Insert(ConceptRecord{ cst });
  EXPECT_EQ(core.GetRS(x2).alias, "X2");
  EXPECT_EQ(core.GetRS(x2).type, cst.type);
  EXPECT_EQ(core.GetRS(x2).definition, "X2=X2");
  EXPECT_EQ(core.GetText(x2).term, cst.term);
  EXPECT_EQ(core.GetText(x2).definition.Str(), "term");
  EXPECT_EQ(core.GetText(x2).definition.Raw(), tccl::EntityRef("X2"));
}

TEST_F(UTRSCore, InsertCopyRename) {
  core.Emplace(CstType::base);

  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X1";
  cst.rs = "X1=X1";
  cst.definition = ManagedText{ tccl::EntityRef("X1") };
  const auto x2 = core.InsertCopy(cst);
  EXPECT_EQ(core.GetRS(x2).alias, "X2");
  EXPECT_EQ(core.GetRS(x2).type, cst.type);
  EXPECT_EQ(core.GetRS(x2).definition, "X2=X2");
  EXPECT_EQ(core.GetText(x2).definition.Raw(), tccl::EntityRef("X2"));

  const auto x3 = core.InsertCopy(x2, core);
  EXPECT_EQ(core.GetRS(x3).alias, "X3");
  EXPECT_EQ(core.GetRS(x3).definition, "X3=X3");
  EXPECT_EQ(core.GetText(x3).definition.Raw(), tccl::EntityRef("X3"));
}

TEST_F(UTRSCore, InsertReorder) {
  const auto d1 = core.Emplace(CstType::term);
  const auto s1 = core.Emplace(CstType::structured);
  const auto x1 = core.Emplace(CstType::base);
  const auto order = core.List().SortSubset({ d1, s1, x1 });
  ccl::VectorOfEntities test{ x1, s1, d1 };
  EXPECT_EQ(order, test);
}

TEST_F(UTRSCore, Load) {
  const auto d1 = core.Emplace(CstType::term);
  core.SetExpressionFor(d1, "{X2}");
  core.SetDefinitionFor(d1, tccl::EntityRef("X2"));

  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X2";
  cst.term = LexicalTerm{ "term" };

  const auto x1 = core.Load(ConceptRecord{ cst });
  EXPECT_EQ(core.GetRS(x1).alias, cst.alias);
  EXPECT_EQ(core.GetRS(x1).type, cst.type);
  EXPECT_EQ(core.GetText(x1).term, cst.term);
  EXPECT_EQ(core.GetParse(d1).status, ParsingStatus::UNKNOWN);
  EXPECT_NE(core.GetText(d1).definition.Str(), "term");
}

TEST_F(UTRSCore, MoveBefore) {
  const auto x1 = core.Emplace(CstType::base);
  const auto x2 = core.Emplace(CstType::base);
  EXPECT_EQ(*std::begin(core.List()), x1);
  EXPECT_TRUE(core.MoveBefore(x2, core.List().Find(x1)));
  EXPECT_EQ(*std::begin(core.List()), x2);

  const auto d1 = core.Emplace(CstType::term);
  EXPECT_FALSE(core.MoveBefore(d1, core.List().Find(x1)));
  EXPECT_FALSE(core.MoveBefore(x1, std::end(core.List())));
}

TEST_F(UTRSCore, Erase) {
  EXPECT_FALSE(core.Erase(invalid));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_FALSE(core.Erase(EntityUID{ x1 + 1 }));

  const auto x2 = core.Emplace(CstType::base);
  EXPECT_EQ(core.GetRS(x2).alias, "X2");
  EXPECT_TRUE(core.Erase(x1));
  EXPECT_EQ(core.GetRS(x2).alias, "X2");
  EXPECT_TRUE(core.Contains(x2));
  EXPECT_FALSE(core.Contains(x1));

  const auto d1 = core.Emplace(CstType::term, "{X2}");
  EXPECT_EQ(core.GetParse(d1).status, ParsingStatus::VERIFIED);
  EXPECT_TRUE(core.Erase(x2));
  EXPECT_EQ(core.GetParse(d1).status, ParsingStatus::INCORRECT);
}

TEST_F(UTRSCore, SetName) {
  EXPECT_FALSE(core.SetAliasFor(invalid, "X1"));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_FALSE(core.SetAliasFor(x1, core.GetRS(x1).alias));

  const auto d1 = core.Emplace(CstType::term, "{X2}");
  core.SetDefinitionFor(d1, tccl::EntityRef("X2"));
  core.SetTermFor(x1, "term");
  EXPECT_TRUE(core.SetAliasFor(x1, "X2"));
  EXPECT_EQ(core.GetRS(x1).alias, "X2");
  EXPECT_EQ(core.GetText(x1).alias, "X2");
  EXPECT_EQ(core.GetParse(d1).status, ParsingStatus::VERIFIED);
  EXPECT_EQ(core.GetText(d1).definition.Str(), "term");
}

TEST_F(UTRSCore, SetExpression) {
  EXPECT_FALSE(core.SetExpressionFor(invalid, ""));
  EXPECT_FALSE(core.SetExpressionFor(invalid, "expr"));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_TRUE(core.SetExpressionFor(x1, "expr"));
  EXPECT_EQ(core.GetRS(x1).definition, "expr");
  EXPECT_FALSE(core.SetExpressionFor(x1, "expr"));
  EXPECT_TRUE(core.SetExpressionFor(x1, ""));
  EXPECT_FALSE(core.SetExpressionFor(x1, ""));
}

TEST_F(UTRSCore, SetConvention) {
  EXPECT_FALSE(core.SetConventionFor(invalid, ""));
  EXPECT_FALSE(core.SetConventionFor(invalid, "convention"));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_TRUE(core.SetConventionFor(x1, "convention"));
  EXPECT_EQ(core.GetRS(x1).convention, "convention");
  EXPECT_FALSE(core.SetConventionFor(x1, "convention"));
  EXPECT_TRUE(core.SetConventionFor(x1, ""));
  EXPECT_FALSE(core.SetConventionFor(x1, ""));
}

TEST_F(UTRSCore, SetTerm) {
  EXPECT_FALSE(core.SetTermFor(invalid, ""));
  EXPECT_FALSE(core.SetTermFor(invalid, "term"));

  const auto x1 = core.Emplace(CstType::base);
  const auto d1 = core.Emplace(CstType::term);
  EXPECT_TRUE(core.SetTermFor(x1, "term"));
  EXPECT_EQ(core.GetText(x1).term.Nominal(), "term");
  EXPECT_FALSE(core.SetTermFor(x1, "term"));
  EXPECT_TRUE(core.SetTermFor(d1, tccl::EntityRef("X1")));
  EXPECT_EQ(core.GetText(d1).term.Nominal(), "term");
}

TEST_F(UTRSCore, SetTermForm) {
  using namespace ccl::lang;
  const Morphology testForm{ Grammem::plur, Grammem::datv };
  EXPECT_FALSE(core.SetTermFormFor(invalid, "term2", testForm));
  EXPECT_FALSE(core.SetTermFormFor(invalid, "", testForm));

  const auto x1 = core.Emplace(CstType::base);
  const auto d1 = core.Emplace(CstType::term);
  core.SetTermFor(d1, tccl::EntityRef("X1", testForm));
  core.SetTermFor(x1, "term2");
  EXPECT_TRUE(core.SetTermFormFor(x1, "terma", testForm));
  EXPECT_FALSE(core.SetTermFormFor(x1, "terma", testForm));
  EXPECT_EQ(core.GetText(x1).term.Nominal(), "term2");
  EXPECT_EQ(core.GetText(x1).term.GetForm(testForm), "terma");
  EXPECT_EQ(core.GetText(d1).term.Nominal(), "terma");
}

TEST_F(UTRSCore, SetDefinition) {
  EXPECT_FALSE(core.SetDefinitionFor(invalid, ""));
  EXPECT_FALSE(core.SetDefinitionFor(invalid, "def"));

  const auto x1 = core.Emplace(CstType::base);
  EXPECT_TRUE(core.SetDefinitionFor(x1, "def"));
  EXPECT_EQ(core.GetText(x1).definition.Str(), "def");
  EXPECT_FALSE(core.SetDefinitionFor(x1, "def"));
  EXPECT_TRUE(core.SetDefinitionFor(x1, ""));
  EXPECT_FALSE(core.SetDefinitionFor(x1, ""));
}

TEST_F(UTRSCore, ResetAliases) {
  const auto x1 = core.Emplace(CstType::base);
  const std::string oldName = core.GetRS(x1).alias;
  core.SetAliasFor(x1, "X3");
  core.ResetAliases();
  EXPECT_EQ(core.GetRS(x1).alias, oldName);
}

TEST_F(UTRSCore, ResetAllNamesReorder) {
  const auto x1 = core.Emplace(CstType::base);
  const auto x2 = core.Emplace(CstType::base);
  core.MoveBefore(x2, core.List().Find(x1));
  core.ResetAliases();
  EXPECT_EQ(core.GetRS(x2).alias, "X1");
  EXPECT_EQ(core.GetRS(x1).alias, "X2");
}

TEST_F(UTRSCore, ResetAllNamesNextName) {
  const auto x1 = core.Emplace(CstType::base);
  const auto x2 = core.Emplace(CstType::base);
  core.MoveBefore(x2, core.List().Find(x1));
  core.ResetAliases();
  EXPECT_EQ(core.GetRS(core.Emplace(CstType::base)).alias, "X3");
}

TEST_F(UTRSCore, UpdateState) {
  const auto x1 = core.Emplace(CstType::base);
  core.SetDefinitionFor(x1, tccl::EntityRef("X2"));
  ConceptRecord cst{};
  cst.type = CstType::base;
  cst.alias = "X2";
  cst.term = LexicalTerm{ "term" };

  core.Load(std::move(cst));
  EXPECT_NE(core.GetText(x1).definition.Str(), "term");

  core.UpdateState();
  EXPECT_EQ(core.GetText(x1).definition.Str(), "term");
}

TEST_F(UTRSCore, Translations) {
  const auto x1 = core.Emplace(CstType::base);
  core.SetExpressionFor(x1, "X2=X2");
  core.SetTermFor(x1, "X1 term");
  core.SetDefinitionFor(x1, tccl::EntityRef("X3"));

  const auto x2 = core.Emplace(CstType::base);
  core.SetExpressionFor(x2, "X4=X4");
  core.SetTermFor(x2, tccl::EntityRef("X1"));
  core.SetDefinitionFor(x2, tccl::EntityRef("X2"));

  const auto translator = ccl::CreateTranslator({ {"X1", "X2"}, {"X2", "X3"}, {"X3", "X4"} });
  {
    RSCore test = core;
    test.TranslateAll(translator);
    EXPECT_EQ(test.GetRS(x1).alias, "X1");
    EXPECT_EQ(test.GetRS(x1).definition, "X3=X3");
    EXPECT_EQ(test.GetText(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.GetText(x1).definition.Raw(), tccl::EntityRef("X4"));

    EXPECT_EQ(test.GetRS(x2).alias, "X2");
    EXPECT_EQ(test.GetRS(x2).definition, "X4=X4");
    EXPECT_EQ(test.GetText(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.GetText(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
  {
    RSCore test = core;
    test.Translate(x2, translator);
    EXPECT_EQ(test.GetRS(x1).alias, "X1");
    EXPECT_EQ(test.GetRS(x1).definition, "X2=X2");
    EXPECT_EQ(test.GetText(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.GetText(x1).definition.Raw(), tccl::EntityRef("X3"));

    EXPECT_EQ(test.GetRS(x2).alias, "X2");
    EXPECT_EQ(test.GetRS(x2).definition, "X4=X4");
    EXPECT_EQ(test.GetText(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.GetText(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
  {
    RSCore test = core;
    test.TranslateTexts(x2, translator);
    EXPECT_EQ(test.GetRS(x1).alias, "X1");
    EXPECT_EQ(test.GetRS(x1).definition, "X2=X2");
    EXPECT_EQ(test.GetText(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.GetText(x1).definition.Raw(), tccl::EntityRef("X3"));

    EXPECT_EQ(test.GetRS(x2).alias, "X2");
    EXPECT_EQ(test.GetRS(x2).definition, "X4=X4");
    EXPECT_EQ(test.GetText(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.GetText(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
  {
    RSCore test = core;
    test.TranslateTerm(x2, translator);
    EXPECT_EQ(test.GetRS(x1).alias, "X1");
    EXPECT_EQ(test.GetRS(x1).definition, "X2=X2");
    EXPECT_EQ(test.GetText(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.GetText(x1).definition.Raw(), tccl::EntityRef("X3"));

    EXPECT_EQ(test.GetRS(x2).alias, "X2");
    EXPECT_EQ(test.GetRS(x2).definition, "X4=X4");
    EXPECT_EQ(test.GetText(x2).term.Text().Raw(), tccl::EntityRef("X2"));
    EXPECT_EQ(test.GetText(x2).definition.Raw(), tccl::EntityRef("X2"));
  }
  {
    RSCore test = core;
    test.TranslateDef(x2, translator);
    EXPECT_EQ(test.GetRS(x1).alias, "X1");
    EXPECT_EQ(test.GetRS(x1).definition, "X2=X2");
    EXPECT_EQ(test.GetText(x1).term.Text().Raw(), "X1 term");
    EXPECT_EQ(test.GetText(x1).definition.Raw(), tccl::EntityRef("X3"));

    EXPECT_EQ(test.GetRS(x2).alias, "X2");
    EXPECT_EQ(test.GetRS(x2).definition, "X4=X4");
    EXPECT_EQ(test.GetText(x2).term.Text().Raw(), tccl::EntityRef("X1"));
    EXPECT_EQ(test.GetText(x2).definition.Raw(), tccl::EntityRef("X3"));
  }
}