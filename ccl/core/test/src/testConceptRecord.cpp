#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/ConceptRecord.h"

class UTConceptRecord : public ::testing::Test {
protected:
  using ConceptRecord = ccl::semantic::ConceptRecord;
  using TextConcept = ccl::semantic::TextConcept;
  using RSConcept = ccl::semantic::RSConcept;
  using CstType = ccl::semantic::CstType;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ManagedText = ccl::lang::ManagedText;
  using EntityUID = ccl::EntityUID;
};

TEST_F(UTConceptRecord, Construction) {
  {
    ConceptRecord cst1{};
    EXPECT_EQ(cst1.uid, EntityUID{ 0 });
    EXPECT_EQ(cst1.alias, "");
    EXPECT_EQ(cst1.type, CstType::base);
    EXPECT_EQ(cst1.rs, "");
    EXPECT_EQ(cst1.convention, "");
    EXPECT_EQ(cst1.term.Nominal(), "");
    EXPECT_EQ(cst1.definition.Raw(), "");
  }
  {
    RSConcept rsCst{ EntityUID{ 13 }, "X1", CstType::axiom, "definition", "convention" };
    TextConcept txtCst{ EntityUID{ 42 }, "OtherName", LexicalTerm{ "Term" }, ManagedText{ "Def" } };
    ConceptRecord cst2{ rsCst, txtCst };
    EXPECT_EQ(cst2.uid, EntityUID{ 13 });
    EXPECT_EQ(cst2.alias, "X1");
    EXPECT_EQ(cst2.type, CstType::axiom);
    EXPECT_EQ(cst2.rs, "definition");
    EXPECT_EQ(cst2.convention, "convention");
    EXPECT_EQ(cst2.term.Nominal(), "Term");
    EXPECT_EQ(cst2.definition.Raw(), "Def");
  }
}

TEST_F(UTConceptRecord, SpawnRS) {
  ConceptRecord cst{};
  cst.uid = EntityUID{ 13 };
  cst.alias = "X1";
  cst.type = CstType::axiom;
  cst.rs = "definition";
  cst.convention = "convention";
  cst.term = LexicalTerm{ "Term" };
  cst.definition = ManagedText{ "Def" };

  const auto rsCst = cst.SpawnRS();
  EXPECT_EQ(rsCst.uid, cst.uid);
  EXPECT_EQ(rsCst.alias, cst.alias);
  EXPECT_EQ(rsCst.type, cst.type);
  EXPECT_EQ(rsCst.definition, cst.rs);
}

TEST_F(UTConceptRecord, SpawnText) {
  ConceptRecord cst{};
  cst.uid = EntityUID{ 13 };
  cst.alias = "X1";
  cst.type = CstType::axiom;
  cst.rs = "definition";
  cst.convention = "convention";
  cst.term = LexicalTerm{ "Term" };
  cst.definition = ManagedText{ "Def" };

  const auto textCst = cst.SpawnText();
  EXPECT_EQ(textCst.uid, cst.uid);
  EXPECT_EQ(textCst.alias, cst.alias);
  EXPECT_EQ(textCst.term, cst.term);
  EXPECT_EQ(textCst.definition, cst.definition);
}