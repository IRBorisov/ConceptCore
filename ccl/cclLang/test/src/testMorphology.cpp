#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"
#include "ccl/lang/Morphology.h"
#include "ccl/lang/Literals.h"

class UTMorphology : public ::testing::Test {
protected:
  using Grammem = ccl::lang::Grammem;
  using Morphology = ccl::lang::Morphology;
};

TEST_F(UTMorphology, Grammemes) {
  EXPECT_EQ(ccl::lang::Str2Grammem(""), Grammem::invalid);
  EXPECT_EQ(ccl::lang::Str2Grammem("invalid"), Grammem::invalid);
  EXPECT_EQ(ccl::lang::Str2Grammem("PLUR"), Grammem::invalid);
  
  EXPECT_EQ(ccl::lang::Str2Grammem("plur"), Grammem::plur);
  EXPECT_EQ(ccl::lang::Str2Grammem("NOUN"), Grammem::NOUN);

  EXPECT_EQ(ccl::lang::Grammem2Str(Grammem::invalid), "UNKN");
  EXPECT_EQ(ccl::lang::Grammem2Str(Grammem::sing), "sing");
  EXPECT_EQ(ccl::lang::Grammem2Str(Grammem::datv), "datv");
}

TEST_F(UTMorphology, Construction) {
  using ccl::lang::operator""_form;

  EXPECT_EQ(Morphology{}.ToString(), "");
  EXPECT_EQ(ssize(Morphology{}.tags), 0);

  const Morphology morpho{ Grammem::sing, Grammem::datv };
  EXPECT_EQ(Morphology(morpho.ToString()), morpho);

  EXPECT_EQ(Morphology("sing,datv"), morpho);
  EXPECT_EQ(Morphology(" sing , datv "), morpho);
  EXPECT_EQ(Morphology("datv,sing"), morpho);
  EXPECT_EQ(Morphology("datv,sing,invalid_tag"), morpho);
  EXPECT_EQ(Morphology("datv, sing, invalid_tag"), morpho);

  EXPECT_EQ(""_form, Morphology{});
  EXPECT_EQ("datv, sing, invalid_tag"_form, morpho);
}

TEST_F(UTMorphology, Comparison) {
  EXPECT_EQ(Morphology(""), Morphology(""));
  EXPECT_EQ(Morphology(" "), Morphology(""));
  EXPECT_EQ(Morphology("sing,datv"), Morphology("datv,sing"));
  EXPECT_EQ(Morphology("sing,datv"), Morphology("datv,sing"));
  EXPECT_EQ(Morphology("sing"), Morphology("sing"));
  EXPECT_NE(Morphology("sing,datv"), Morphology("sing"));
}

TEST_F(UTMorphology, GrammemAccess) {
  const Morphology morpho0{};
  EXPECT_EQ(morpho0.GetPOS(), Grammem::invalid);
  EXPECT_EQ(morpho0.GetCase(), Grammem::invalid);
  EXPECT_EQ(morpho0.GetNumber(), Grammem::invalid);
  EXPECT_EQ(morpho0.GetGender(), Grammem::invalid);
  EXPECT_EQ(morpho0.GetPerson(), Grammem::invalid);
  EXPECT_EQ(morpho0.GetTense(), Grammem::invalid);

  const Morphology morpho1{ Grammem::datv, Grammem::sing };
  EXPECT_EQ(morpho1.GetPOS(), Grammem::invalid);
  EXPECT_EQ(morpho1.GetCase(), Grammem::datv);
  EXPECT_EQ(morpho1.GetNumber(), Grammem::sing);
  EXPECT_EQ(morpho1.GetGender(), Grammem::invalid);
  EXPECT_EQ(morpho1.GetPerson(), Grammem::invalid);
  EXPECT_EQ(morpho1.GetTense(), Grammem::invalid);

  const Morphology morpho2{ Grammem::NOUN, Grammem::femn, Grammem::sing, Grammem::datv };
  EXPECT_EQ(morpho2.GetPOS(), Grammem::NOUN);
  EXPECT_EQ(morpho2.GetCase(), Grammem::datv);
  EXPECT_EQ(morpho2.GetNumber(), Grammem::sing);
  EXPECT_EQ(morpho2.GetGender(), Grammem::femn);
  EXPECT_EQ(morpho2.GetPerson(), Grammem::invalid);
  EXPECT_EQ(morpho2.GetTense(), Grammem::invalid);

  const Morphology morpho3{ Grammem::VERB, Grammem::sing, Grammem::per1, Grammem::pres };
  EXPECT_EQ(morpho3.GetPOS(), Grammem::VERB);
  EXPECT_EQ(morpho3.GetCase(), Grammem::invalid);
  EXPECT_EQ(morpho3.GetNumber(), Grammem::sing);
  EXPECT_EQ(morpho3.GetGender(), Grammem::invalid);
  EXPECT_EQ(morpho3.GetPerson(), Grammem::per1);
  EXPECT_EQ(morpho3.GetTense(), Grammem::pres);
}

TEST_F(UTMorphology, ContainerAPI) {
  EXPECT_EQ(Morphology("").size(), 0U);
  EXPECT_EQ(std::size(Morphology("")), 0U);
  EXPECT_EQ(std::size(Morphology("datv,sing")), 2U);

  EXPECT_TRUE(Morphology("").empty());
  EXPECT_TRUE(std::empty(Morphology("")));
  EXPECT_FALSE(std::empty(Morphology("NOUN")));

  EXPECT_FALSE(Morphology("").Contains(Grammem::sing));
  EXPECT_FALSE(Morphology("NOUN,datv").Contains(Grammem::sing));
  EXPECT_TRUE(Morphology("sing,datv").Contains(Grammem::sing));
}

TEST_F(UTMorphology, ToString) {
  EXPECT_EQ(Morphology("").ToString(), "");
  EXPECT_EQ(Morphology(" datv ").ToString(), "datv");
  const auto tags = Morphology("sing, datv").ToString();
  EXPECT_TRUE(tags == "sing,datv" || tags == "datv,sing");
}