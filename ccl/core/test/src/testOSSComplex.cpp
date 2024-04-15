#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeSourceManager.hpp"

#include "ccl/env/cclEnvironment.h"
#include "ccl/oss/OSSchema.h"
#include "ccl/ops/RSOperations.h"

class UTossComplex: public ::testing::Test {
protected:
  using Environment = ccl::Environment;
  using RSForm = ccl::semantic::RSForm;
  using CstType = ccl::semantic::CstType;
  using EquationOptions = ccl::ops::EquationOptions;
  using BinarySynthes = ccl::ops::BinarySynthes;
  using PictID = ccl::oss::PictID;
  using Usage = ccl::change::Usage;
  using OSSchema = ccl::oss::OSSchema;
  using ossSourceFacet = ccl::oss::ossSourceFacet;
  using ossOperationsFacet = ccl::oss::ossOperationsFacet;

protected:
  OSSchema oss{};
  ossSourceFacet& srcs{ oss.Src() };
  ossOperationsFacet& ops{ oss.Ops() };

  PictID base1, base2, base3, base4;
  PictID level1_1, level1_2, level2_1, level2_2;

  void SetupLevel1();
  void SetupLevel2Synth();
  void SetupLevel2Rhomb();

  void SetUp() override {
    Environment::Instance().SetSourceManager(std::make_unique<FakeSourceManager>());
  }
  void TearDown() override {
    Environment::Instance().SetSourceManager(std::make_unique<ccl::SourceManager>());
  }

  FakeTRS& Source(const PictID pid) {
    return SrcManager().DummyCast(*srcs(pid)->src);
  }
  RSForm& Schema(const PictID pid) {
    return SrcManager().DummyCast(*srcs(pid)->src).schema;
  }
  FakeSourceManager& SrcManager() {
    return dynamic_cast<FakeSourceManager&>(Environment::Sources());
  }
};

void UTossComplex::SetupLevel1() {
  base1 = oss.InsertBase()->uid;
  base2 = oss.InsertBase()->uid;

  auto& src1 = SrcManager().CreateNewRS();
  auto& src2 = SrcManager().CreateNewRS();
  const auto b1x1 = src1.schema.Emplace(CstType::base);
  src1.schema.Emplace(CstType::term);
  const auto b2x1 = src2.schema.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(b1x1, b2x1);

  srcs.ConnectPict2Src(base1, src1);
  srcs.ConnectPict2Src(base2, src2);

  level1_1 = oss.InsertOperation(base1, base2)->uid;
  ops.InitFor(level1_1, ccl::ops::Type::rsSynt, std::move(equations));

  ASSERT_TRUE(ops.Execute(level1_1));
  EXPECT_FALSE(std::empty(*oss.Src()(level1_1)));
  EXPECT_FALSE(ops(level1_1)->broken);
  EXPECT_FALSE(ops(level1_1)->outdated);
}

void UTossComplex::SetupLevel2Rhomb() {
  base1 = oss.InsertBase()->uid;
  base2 = oss.InsertBase()->uid;
  base3 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base3, SrcManager().CreateNewRS());
  level1_1 = oss.InsertOperation(base1, base2)->uid;
  level1_2 = oss.InsertOperation(base2, base3)->uid;
  level2_1 = oss.InsertOperation(level1_1, level1_2)->uid;
  ops.InitFor(level1_1, ccl::ops::Type::rsMerge, nullptr);
  ops.InitFor(level1_2, ccl::ops::Type::rsMerge, nullptr);
  ops.InitFor(level2_1, ccl::ops::Type::rsMerge, nullptr);

  ASSERT_TRUE(ops.Execute(level1_1));
  ASSERT_TRUE(ops.Execute(level1_2));
  ASSERT_TRUE(ops.Execute(level2_1));
}

void UTossComplex::SetupLevel2Synth() {
  base1 = oss.InsertBase()->uid;
  base2 = oss.InsertBase()->uid;
  base3 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base3, SrcManager().CreateNewRS());
  level1_1 = oss.InsertOperation(base1, base2)->uid;
  level2_1 = oss.InsertOperation(level1_1, base3)->uid;
  level2_2 = oss.InsertOperation(base3, level1_1)->uid;
  const auto x1_b1 = Schema(base1).Emplace(CstType::base);
  Schema(base1).SetTermFor(x1_b1, "X1B1");
  const auto x2_b1 = Schema(base1).Emplace(CstType::base);
  Schema(base1).SetTermFor(x2_b1, "X2B1");
  const auto x1_b2 = Schema(base2).Emplace(CstType::base);
  Schema(base2).SetTermFor(x1_b2, "X1B2");
  const auto x1_b3 = Schema(base3).Emplace(CstType::base);
  Schema(base3).SetTermFor(x1_b3, "X1B3");
  Schema(base1).Emplace(CstType::structured, "\212C(X1)");

  ASSERT_TRUE(ops.InitFor(level1_1, ccl::ops::Type::rsSynt, 
                          { std::make_unique<EquationOptions>(x1_b1, x1_b2) }));
  ASSERT_TRUE(ops.Execute(level1_1));

  const auto& trans1 = *ops(level1_1)->translations;
  const auto x1_lvl1 = trans1[0](x1_b1);
  ASSERT_TRUE(ops.InitFor(level2_1, ccl::ops::Type::rsSynt, 
                          { std::make_unique<EquationOptions>(x1_lvl1, x1_b3) }));
  ASSERT_TRUE(ops.InitFor(level2_2, ccl::ops::Type::rsSynt, 
                          { std::make_unique<EquationOptions>(x1_b3, x1_lvl1) }));
  ASSERT_TRUE(ops.Execute(level2_1));
  ASSERT_TRUE(ops.Execute(level2_2));
}

TEST_F(UTossComplex, CallbackQueryEntity) {
  base1 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  EXPECT_EQ(ccl::Environment::Sources().QueryEntityUse(Source(base1), ccl::EntityUID{ 42 }), Usage::notUsed);

  const auto x1 = Schema(base1).Emplace(CstType::base);
  Source(base1).TriggerSave();
  EXPECT_EQ(ccl::Environment::Sources().QueryEntityUse(Source(base1), x1), Usage::notUsed);

  base2 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
  level1_1 = oss.InsertOperation(base1, base2)->uid;
  ASSERT_TRUE(ops.InitFor(level1_1, ccl::ops::Type::rsMerge));
  ASSERT_TRUE(ops.Execute(level1_1));
  EXPECT_EQ(ccl::Environment::Sources().QueryEntityUse(Source(base1), x1), Usage::asElement);
}

TEST_F(UTossComplex, CallbackQuerySrc) {
  base1 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  EXPECT_TRUE(ccl::Environment::Sources().QuerySrcUse(Source(base1)));
}

TEST_F(UTossComplex, ExecuteBrokenParent) {
  base1 = oss.InsertBase()->uid;
  base2 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
  level1_1 = oss.InsertOperation(base1, base2)->uid;
  level2_1 = oss.InsertOperation(level1_1, base2)->uid;
  const auto x1_b1 = Schema(base1).Emplace(CstType::base);
  const auto x1_b2 = Schema(base2).Emplace(CstType::base);

  ASSERT_TRUE(ops.InitFor(level1_1,
                          ccl::ops::Type::rsSynt,
                          std::make_unique<EquationOptions>(x1_b1, x1_b2)));
  ASSERT_TRUE(ops.Execute(level1_1));

  ASSERT_TRUE(ops.InitFor(level2_1,
                          ccl::ops::Type::rsSynt, 
                          std::make_unique<EquationOptions>(Schema(level1_1).Core().FindAlias("X1").value(), x1_b2)));
  ASSERT_TRUE(ops.Execute(level2_1));

  Schema(base1).Erase(x1_b1);
  Source(base1).TriggerSave();
  EXPECT_FALSE(ops.Execute(level2_1));
}

TEST_F(UTossComplex, OnExecutionCreateAliasTaken) {
  base1 = oss.InsertBase()->uid;
  base2 = oss.InsertBase()->uid;
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
  level1_1 = oss.InsertOperation(base1, base2)->uid;
  level1_2 = oss.InsertOperation(base1, base2)->uid;
  ops.InitFor(level1_1, ccl::ops::Type::rsMerge, nullptr);
  ops.InitFor(level1_2, ccl::ops::Type::rsMerge, nullptr);

  oss.SetPictAlias(level1_1, "42");
  oss.SetPictAlias(level1_2, "42");
  ASSERT_TRUE(ops.Execute(level1_1));
  ASSERT_FALSE(ops.Execute(level1_2));
}

TEST_F(UTossComplex, OnConnectBreakOperation) {
  SetupLevel1();

  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  EXPECT_TRUE(ops(level1_1)->broken);
  EXPECT_TRUE(ops(level1_1)->outdated);
}

TEST_F(UTossComplex, OnConnectDoNotUpdateOperation) {
  SetupLevel1();

  const auto oldTranslations = *ops(level1_1)->translations;
  const auto oldOptions = dynamic_cast<const EquationOptions&>(*ops(level1_1)->options);

  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());

  const auto* options = dynamic_cast<const EquationOptions*>(&*ops(level1_1)->options);
  ASSERT_TRUE(ops(level1_1)->translations != nullptr);
  ASSERT_TRUE(options != nullptr);
  EXPECT_EQ(*ops(level1_1)->translations, oldTranslations);
  EXPECT_TRUE(oldOptions.IsEqualTo(*options));
}

TEST_F(UTossComplex, RomboidMergeDuplicates) {
  SetupLevel2Rhomb();

  auto& basicKS = Schema(base2);

  basicKS.Emplace(CstType::base);
  basicKS.SetTermFor(basicKS.Emplace(CstType::base), "42");
  basicKS.Emplace(CstType::base, "42");

  basicKS.Emplace(CstType::term);
  basicKS.SetTermFor(basicKS.Emplace(CstType::term), "43");
  basicKS.Emplace(CstType::term, "43");

  Schema(level2_1).Emplace(CstType::base);

  ASSERT_NO_THROW(ops.ExecuteAll());
  EXPECT_EQ(std::ssize(Schema(level1_1).Core()), 6);
  EXPECT_EQ(std::ssize(Schema(level1_2).Core()), 6);
  EXPECT_EQ(Schema(level2_1).RSLang().Count(CstType::base), 5U);
  EXPECT_EQ(Schema(level2_1).RSLang().Count(CstType::term), 4U);
}

TEST_F(UTossComplex, UpdateLevel2Options) {
  SetupLevel2Synth();

  const auto oldCount = std::size(Schema(level2_1).Core());
  const auto x3_b1 = Schema(base1).Emplace(CstType::base);
  Schema(base1).MoveBefore(x3_b1, Schema(base1).List().begin());

  const auto s2_b1 = Schema(base1).Emplace(CstType::structured);
  Schema(base1).MoveBefore(s2_b1, Schema(base1).List().Find(Schema(base1).Core().FindAlias("S1").value()));

  const auto x2_b2 = Schema(base2).Emplace(CstType::base);
  Schema(base2).MoveBefore(x2_b2, Schema(base2).List().begin());

  Source(base1).TriggerSave();
  Source(base2).TriggerSave();

  EXPECT_TRUE(ops.Execute(level1_1));
  EXPECT_TRUE(ops.IsExecutable(level2_1));
  EXPECT_TRUE(ops.IsExecutable(level2_2));
  EXPECT_FALSE(ops(level2_1)->broken);
  EXPECT_FALSE(ops(level2_2)->broken);
  EXPECT_TRUE(ops.Execute(level2_1));
  EXPECT_TRUE(ops.Execute(level2_2));
  EXPECT_EQ(std::size(Schema(level2_1).Core()), oldCount + 3);
  EXPECT_EQ(std::size(Schema(level2_2).Core()), oldCount + 3);
}

TEST_F(UTossComplex, ExecuteRedoFailedParent) {
  SetupLevel2Synth();

  Schema(base1).Emplace(CstType::base);
  Source(base1).TriggerSave();
  SrcManager().ReplaceSourceData(Source(level1_1), RSForm{});
  EXPECT_FALSE(ops.Execute(level2_1));
  EXPECT_FALSE(ops.Execute(level2_2));
}