#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeSourceManager.hpp"

#include "ccl/oss/OSSchema.h"
#include "ccl/ops/RSOperations.h"
#include "RSLHelper.hpp"

class UTossOps: public ::testing::Test {
protected:
  using BinarySynthes = ccl::ops::BinarySynthes;
  using Equation = ccl::ops::Equation;
  using EquationOptions = ccl::ops::EquationOptions;
  using CstType = ccl::semantic::CstType;
  using RSForm = ccl::semantic::RSForm;
  using Usage = ccl::change::Usage;
  using PictID = ccl::oss::PictID;
  using OSSchema = ccl::oss::OSSchema;
  using ossSourceFacet = ccl::oss::ossSourceFacet;
  using ossOperationsFacet = ccl::oss::ossOperationsFacet;
  using Status = ccl::ops::Status;
  using Type = ccl::ops::Type;

protected:
  OSSchema oss{};
  ossSourceFacet& srcs{ oss.Src() };
  ossOperationsFacet& ops{ oss.Ops() };
  const PictID base1{ oss.InsertBase()->uid };
  const PictID base2{ oss.InsertBase()->uid };
  const PictID level1{ oss.InsertOperation(base1, base2)->uid };

  void SetupBaseSources();
  bool ExecuteEmptyMerge();

  void SetUp() override {
    ccl::Environment::Instance().SetSourceManager(std::make_unique<FakeSourceManager>());
  }
  void TearDown() override {
    ccl::Environment::Instance().SetSourceManager(std::make_unique<ccl::SourceManager>());
  }

  FakeTRS& Source(const PictID pid) {
    return SrcManager().DummyCast(*srcs(pid)->src);
  }
  RSForm& Schema(const PictID pid) {
    return Source(pid).schema;
  }
  FakeSourceManager& SrcManager() {
    return dynamic_cast<FakeSourceManager&>(ccl::Environment::Sources());
  }
};

void UTossOps::SetupBaseSources() {
  srcs.ConnectPict2Src(base1, SrcManager().CreateNewRS());
  srcs.ConnectPict2Src(base2, SrcManager().CreateNewRS());
}

bool UTossOps::ExecuteEmptyMerge() {
  SetupBaseSources();
  return ops.InitFor(level1, Type::rsMerge, nullptr)
      && ops.Execute(level1);
}

TEST_F(UTossOps, InvalidID) {
  auto someID = PictID{ 42U };
  while (oss.Contains(someID)) {
    ++someID;
  }
  EXPECT_TRUE(empty(ops.VariantsFor(someID)));
  EXPECT_TRUE(ops(someID) == nullptr);
  EXPECT_FALSE(ops.HasOperation(someID));
  EXPECT_FALSE(ops.IsExecutable(someID));
  EXPECT_FALSE(ops.IsTranslatable(someID));
  EXPECT_FALSE(ops.Execute(someID));
  EXPECT_NO_THROW(ops.ExecuteAll());
}

TEST_F(UTossOps, BasicNoOp) {
  const auto pid = oss.InsertBase()->uid;
  ASSERT_TRUE(ops(pid) == nullptr);
  EXPECT_TRUE(empty(ops.VariantsFor(pid)));
  EXPECT_FALSE(ops.HasOperation(pid));
  EXPECT_FALSE(ops.IsExecutable(pid));
  EXPECT_FALSE(ops.IsTranslatable(pid));
  EXPECT_FALSE(ops.Execute(pid));
  EXPECT_NO_THROW(ops.ExecuteAll());
}

TEST_F(UTossOps, BasicOps) {
  ASSERT_TRUE(ops(level1) != nullptr);
  EXPECT_EQ(ops.StatusOf(base1), Status::undefined);
  EXPECT_EQ(ops.StatusOf(level1), Status::undefined);
  EXPECT_EQ(ops(level1)->type, Type::tba);
  EXPECT_TRUE(ops(level1)->options == nullptr);
  EXPECT_TRUE(ops(level1)->translations == nullptr);

  EXPECT_TRUE(ops.HasOperation(level1));
  EXPECT_FALSE(empty(ops.VariantsFor(level1)));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops.Execute(level1));
}

TEST_F(UTossOps, ConnectingToSources) {
  SetupBaseSources();
  EXPECT_EQ(ops.StatusOf(level1), Status::undefined);
  EXPECT_EQ(ops(level1)->type, Type::tba);
  EXPECT_TRUE(ops(level1)->options == nullptr);
  EXPECT_TRUE(ops(level1)->translations == nullptr);

  EXPECT_FALSE(empty(ops.VariantsFor(level1)));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops.Execute(level1));
}

TEST_F(UTossOps, VariantsForBinaryRS) {
  SetupBaseSources();
  std::unordered_set<Type> variants
    { Type::tba, Type::rsMerge, Type::rsSynt };
  EXPECT_EQ(ops.VariantsFor(level1), variants);
}

TEST_F(UTossOps, InitMerge) {
  SetupBaseSources();

  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, nullptr));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops.IsTranslatable(level1));
  EXPECT_FALSE(ops(level1)->broken);
  EXPECT_EQ(ops.StatusOf(level1), Status::defined);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
}

TEST_F(UTossOps, InitForInvalid) {
  EXPECT_FALSE(ops.InitFor(base1, Type::rsMerge, nullptr));
  SetupBaseSources();
  EXPECT_FALSE(ops.InitFor(base1, Type::rsMerge, nullptr));
}

TEST_F(UTossOps, InitMergeEmptyOptions) {
  SetupBaseSources();
  auto equations = std::make_unique<EquationOptions>();
  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, std::move(equations)));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops(level1)->broken);
  EXPECT_EQ(ops.StatusOf(level1), Status::defined);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
}

TEST_F(UTossOps, InitMergeNotemptyOptions) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_x1);

  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, std::move(equations)));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_TRUE(ops(level1)->broken);
  EXPECT_EQ(ops.StatusOf(level1), Status::broken);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
}

TEST_F(UTossOps, InitMergeEmptyReinitNullptr) {
  SetupBaseSources();
  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, std::make_unique<EquationOptions>()));
  EXPECT_TRUE(ops(level1)->options != nullptr);

  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, nullptr));
  EXPECT_TRUE(ops(level1)->options == nullptr);

  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, std::make_unique<EquationOptions>()));
  EXPECT_TRUE(ops(level1)->options != nullptr);
}

TEST_F(UTossOps, InitSynt) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_x1);
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops(level1)->broken);
  EXPECT_EQ(ops.StatusOf(level1), Status::defined);
  EXPECT_EQ(ops(level1)->type, Type::rsSynt);
}

TEST_F(UTossOps, InitSyntEmptyEquations) {
  SetupBaseSources();
  auto equations = std::make_unique<EquationOptions>();
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::defined);
  EXPECT_EQ(ops(level1)->type, Type::rsSynt);
}

TEST_F(UTossOps, InitSyntDouble) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = EquationOptions{ ks1_x1, ks2_x1 };
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::make_unique<EquationOptions>(equations)));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::make_unique<EquationOptions>(equations)));
}

TEST_F(UTossOps, InitNullOptions) {
  SetupBaseSources();
  ASSERT_FALSE(ops.InitFor(level1, Type::rsSynt, nullptr));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::undefined);
  EXPECT_EQ(ops(level1)->type, Type::tba);
}

TEST_F(UTossOps, InitInvalidOptions) {
  SetupBaseSources();
  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(42U, 42U);
  ASSERT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::broken);
  EXPECT_EQ(ops(level1)->type, Type::rsSynt);
}

TEST_F(UTossOps, InitIncorrectOptions) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks2_s1 = ks2.Emplace(CstType::structured);

  auto equations = std::make_unique<EquationOptions>(ks1_x1, ks2_s1);
  ASSERT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::broken);
  EXPECT_EQ(ops(level1)->type, Type::rsSynt);
}

TEST_F(UTossOps, InitValidToTBA) {
  SetupBaseSources();
  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, nullptr));
  ASSERT_TRUE(ops.InitFor(level1, Type::tba, nullptr));
  EXPECT_EQ(ops.StatusOf(level1), Status::undefined);
  EXPECT_EQ(ops(level1)->type, Type::tba);
}

TEST_F(UTossOps, InitMergeResetParams) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_x1);
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge, nullptr));
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops.IsTranslatable(level1));
  EXPECT_FALSE(ops(level1)->broken);
  EXPECT_EQ(ops.StatusOf(level1), Status::defined);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
  EXPECT_TRUE(ops(level1)->options == nullptr);
}

TEST_F(UTossOps, IsExecutableMissingSource) {
  SetupBaseSources();
  ops.InitFor(level1, Type::rsMerge, nullptr);
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_FALSE(ops(level1)->broken);

  SrcManager().DestroySource(Source(base1));

  EXPECT_FALSE(ops.IsExecutable(level1));
  EXPECT_TRUE(ops(level1)->broken);
}

TEST_F(UTossOps, Execute) {
  ASSERT_TRUE(ExecuteEmptyMerge());
  EXPECT_TRUE(ops.IsExecutable(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::done);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
  EXPECT_TRUE(srcs(level1)->src != nullptr);

  ASSERT_TRUE(ops(level1)->translations != nullptr);
  const auto& translations = *ops(level1)->translations;
  ASSERT_EQ(ssize(translations), 2);
  EXPECT_TRUE(std::empty(translations[0]));
  EXPECT_TRUE(std::empty(translations[1]));
  EXPECT_TRUE(ops.IsTranslatable(level1));
}

TEST_F(UTossOps, ExecuteClosedSrc) {
  SetupBaseSources();
  ASSERT_TRUE(ops.InitFor(level1, Type::rsMerge));
  Source(base1).TriggerClose();
  ASSERT_TRUE(ops.Execute(level1));
  Source(level1).TriggerClose();
  ASSERT_TRUE(ops.Execute(level1));
}

TEST_F(UTossOps, ExecuteMissingSrc) {
  SetupBaseSources();
  ops.InitFor(level1, Type::rsMerge);
  SrcManager().DestroySource(Source(base1));
  EXPECT_FALSE(ops.Execute(level1));
}

TEST_F(UTossOps, ExecuteWritable) {
  ASSERT_TRUE(ExecuteEmptyMerge());
  Source(level1).unwritable = true;
  EXPECT_FALSE(ops.Execute(level1));
}

TEST_F(UTossOps, ExecuteUpdateArgs) {
  ASSERT_TRUE(ExecuteEmptyMerge());

  Schema(base1).Emplace(CstType::term);
  EXPECT_NE(Schema(base1).FullHash(), srcs(base1)->fullHash);
  Schema(level1).Emplace(CstType::base);

  ASSERT_TRUE(ops.Execute(level1));
  EXPECT_EQ(Schema(level1).RSLang().Count(CstType::term), 1U);
  EXPECT_EQ(Schema(level1).RSLang().Count(CstType::base), 1U);
  EXPECT_EQ(Schema(base1).FullHash(), srcs(base1)->fullHash);
  EXPECT_EQ(Schema(level1).FullHash(), srcs(level1)->fullHash);
}

TEST_F(UTossOps, ExecuteDouble) {
  ASSERT_TRUE(ExecuteEmptyMerge());
  EXPECT_TRUE(ops.Execute(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::done);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
  EXPECT_TRUE(srcs(level1)->src != nullptr);
}

TEST_F(UTossOps, ExecuteRedo) {
  ASSERT_TRUE(ExecuteEmptyMerge());
  auto* base = dynamic_cast<FakeTRS*>(srcs(base1)->src);
  base->schema.Emplace(CstType::base, "ks1_x1");
  base->TriggerSave();

  auto* resultSrc = dynamic_cast<FakeTRS*>(srcs(level1)->src);
  resultSrc->schema.Emplace(CstType::base, "result_x1");
  resultSrc->TriggerSave();
  resultSrc->TriggerClose();

  ASSERT_TRUE(ops.Execute(level1));
  EXPECT_EQ(ops.StatusOf(level1), Status::done);

  resultSrc = dynamic_cast<FakeTRS*>(srcs(level1)->src);
  ASSERT_TRUE(resultSrc != nullptr);
  ASSERT_EQ(std::ssize(resultSrc->schema.Core()), 2);

  auto cstIter = std::begin(resultSrc->schema.List());
  EXPECT_EQ(resultSrc->schema.GetRS(*cstIter).definition, "ks1_x1");
  ++cstIter;
  EXPECT_EQ(resultSrc->schema.GetRS(*cstIter).definition, "result_x1");

  ASSERT_TRUE(ops(level1)->translations != nullptr);
  const auto& translations = *ops(level1)->translations;
  ASSERT_EQ(ssize(translations), 2);
  EXPECT_FALSE(std::empty(translations[0]));
  EXPECT_TRUE(std::empty(translations[1]));
}

TEST_F(UTossOps, ExecuteRedoFailedAggregation) {
  SetupBaseSources();

  Schema(base1).Emplace(CstType::base);
  Schema(base2).Emplace(CstType::base);

  ops.InitFor(level1, Type::rsMerge, nullptr);
  ops.Execute(level1);

  SrcManager().ReplaceSourceData(Source(level1), RSForm{});
  EXPECT_FALSE(ops.Execute(level1, false));
  EXPECT_FALSE(ops.IsTranslatable(level1));
  EXPECT_TRUE(ops.Execute(level1, true));
  EXPECT_TRUE(ops.IsTranslatable(level1));
}

TEST_F(UTossOps, ExecuteUnrelatedResultSrc) {
  auto& src1 = SrcManager().CreateNewRS();
  auto& src2 = SrcManager().CreateNewRS();
  auto& unrelated = SrcManager().CreateNewRS();
  const auto unrelatedSchema = &unrelated.schema;

  srcs.ConnectPict2Src(base1, src1);
  srcs.ConnectPict2Src(base2, src2);
  ASSERT_TRUE(ExecuteEmptyMerge());
  EXPECT_EQ(&unrelated.schema, unrelatedSchema);
}

TEST_F(UTossOps, ExecuteSyntReverseEquation) {
  SetupBaseSources();

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  ks1.SetTermFor(ks1_x1, "42");

  ks2.Emplace(CstType::base);
  const auto ks2_d1 = ks2.Emplace(CstType::term, R"(X1 \setminus X1)"_rs);
  ks2.SetTermFor(ks2_d1, "43");

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_d1, Equation{ Equation::Mode::keepDel, "" });
  EXPECT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_TRUE(ops.IsExecutable(level1));
  ASSERT_TRUE(ops.Execute(level1));
  
  const auto& ksResult = Schema(level1);
  EXPECT_EQ(1U, ksResult.RSLang().Count(CstType::base));
}

TEST_F(UTossOps, OnBaseChange) {
  ASSERT_TRUE(ExecuteEmptyMerge());
  auto* base = dynamic_cast<FakeTRS*>(srcs(base1)->src);
  base->schema.Emplace(CstType::base);
  base->TriggerSave();

  EXPECT_EQ(base->schema.CoreHash(), srcs(base1)->coreHash);
  EXPECT_EQ(ops.StatusOf(level1), Status::outdated);
  EXPECT_EQ(ops(level1)->type, Type::rsMerge);
  EXPECT_TRUE(srcs(level1)->src != nullptr);
}

TEST_F(UTossOps, QueryUsage) {
  SetupBaseSources();

  const auto& notRelated = SrcManager().CreateNewRS();

  EXPECT_EQ(Usage::notUsed, ops.QueryEntityUsage(Source(base1), ccl::EntityUID{ 42 }));

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);
  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks1_d1 = ks1.Emplace(CstType::term, R"(X1 \setminus X21)"_rs);
  EXPECT_EQ(ops.QueryEntityUsage(notRelated, ks1_x1), Usage::notUsed);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_x1), Usage::notUsed);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_d1), Usage::notUsed);

  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_x1);
  ASSERT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_x1), Usage::asOption);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base2), ks2_x1), Usage::asOption);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_d1), Usage::notUsed);

  ASSERT_TRUE(ops.Execute(level1));
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_x1), Usage::asOption);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_d1), Usage::asElement);

  auto& synt = Schema(level1);
  synt.Emplace(CstType::term, R"(X1 \setminus D1)"_rs);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_x1), Usage::asOption);
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_d1), Usage::asElement);
}

TEST_F(UTossOps, QueryUsageChained) {
  SetupBaseSources();
  const PictID level2{ oss.InsertOperation(base2, level1)->uid };

  auto& ks1 = Schema(base1);
  auto& ks2 = Schema(base2);

  const auto ks1_x1 = ks1.Emplace(CstType::base);
  const auto ks1_d1 = ks1.Emplace(CstType::term, R"(X1 \setminus X21)"_rs);
  const auto ks2_x1 = ks2.Emplace(CstType::base);

  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(ks1_x1, ks2_x1);
  ASSERT_TRUE(ops.InitFor(level1, Type::rsSynt, std::move(equations)));
  ASSERT_TRUE(ops.Execute(level1));

  equations = std::make_unique<EquationOptions>();
  equations->Insert(ks2_x1, Schema(level1).Core().FindAlias("D1").value());
  ASSERT_TRUE(ops.InitFor(level2, Type::rsSynt, std::move(equations)));
  EXPECT_EQ(ops.QueryEntityUsage(Source(base1), ks1_d1), Usage::asOption);
}