#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/oss/OSSchema.h"

class UTOSS: public ::testing::Test {
protected:
  using OSSchema = ccl::oss::OSSchema;
  using PictID = ccl::oss::PictID;
  using Pict = ccl::oss::Pict;
  using GridPosition = ccl::oss::GridPosition;
  using OperationHandle = ccl::oss::OperationHandle;
  using DataType = ccl::src::DataType;
  using Descriptor = ccl::src::Descriptor;
  using Handle = ccl::src::Handle;

protected:
  OSSchema oss{};
};

TEST_F(UTOSS, DefaultConstruct) {
  EXPECT_TRUE(std::empty(oss));
  EXPECT_EQ(0, std::ssize(oss));
  EXPECT_EQ(std::begin(oss), std::end(oss));
}

TEST_F(UTOSS, FacetAccess) {
  const auto& constRef = oss;
  EXPECT_EQ(&oss.Grid(), &constRef.Grid());
  EXPECT_EQ(&oss.Graph(), &constRef.Graph());
  EXPECT_EQ(&oss.Src(), &constRef.Src());
  EXPECT_EQ(&oss.Ops(), &constRef.Ops());
}

TEST_F(UTOSS, NewBase) {
  auto pict = oss.InsertBase();
  ASSERT_TRUE(pict != nullptr);
  EXPECT_FALSE(std::empty(oss));
  EXPECT_EQ(std::ssize(oss), 1);

  EXPECT_EQ(pict->dataType, DataType::rsSchema);
}

TEST_F(UTOSS, FindID) {
  auto pict = oss.InsertBase();
  EXPECT_EQ(oss(pict->uid), pict);
}

TEST_F(UTOSS, FindInvalid) {
  EXPECT_EQ(oss(PictID{ 0U }), nullptr);
  EXPECT_EQ(oss(PictID{ 42U }), nullptr);
  EXPECT_EQ(oss(PictID{ static_cast<uint32_t>(-1) }), nullptr);
  auto pict = oss.InsertBase();
  EXPECT_EQ(oss(pict->uid + 1), nullptr);
}

TEST_F(UTOSS, ContainsInvalid) {
  EXPECT_FALSE(oss.Contains(PictID{ 0U }));
  EXPECT_FALSE(oss.Contains(PictID{ 42U }));
  EXPECT_FALSE(oss.Contains(PictID{ static_cast<uint32_t>(-1) }));
  auto pict = oss.InsertBase();
  EXPECT_FALSE(oss.Contains(pict->uid + 1));
}

TEST_F(UTOSS, NewOperation) {
  auto pict1 = oss.InsertBase();
  auto pict2 = oss.InsertBase();
  auto child = oss.InsertOperation(pict1->uid, pict2->uid);
  ASSERT_TRUE(child != nullptr);
  EXPECT_EQ(child->dataType, DataType::rsSchema);
  EXPECT_EQ(oss.Grid()(child->uid).value(), GridPosition(1, 0));
}

TEST_F(UTOSS, NewOperationInvalid) {
  auto pict1 = oss.InsertBase();
  auto invalidID1 = pict1->uid + 1;
  auto invalidID2 = pict1->uid + 2;
  EXPECT_EQ(oss.InsertOperation(invalidID1, invalidID2), nullptr);
  EXPECT_EQ(oss.InsertOperation(pict1->uid, invalidID1), nullptr);
  EXPECT_EQ(oss.InsertOperation(invalidID1, pict1->uid), nullptr);
}

TEST_F(UTOSS, NewOperationSelf) {
  auto pict1 = oss.InsertBase();
  EXPECT_EQ(oss.InsertOperation(pict1->uid, pict1->uid), nullptr);
}

TEST_F(UTOSS, NewOperationDoubleUsage) {
  auto pid1 = oss.InsertBase()->uid;
  auto pid2 = oss.InsertBase()->uid;
  auto level1 = oss.InsertOperation(pid1, pid2);
  ASSERT_TRUE(level1 != nullptr);

  auto level2_1 = oss.InsertOperation(pid1, level1->uid);
  ASSERT_TRUE(level2_1 != nullptr);
  EXPECT_EQ(oss.Grid()(level2_1->uid).value(), GridPosition(2, 0));

  auto level2_2 = oss.InsertOperation(level1->uid, pid1);
  ASSERT_TRUE(level2_2 != nullptr);
  EXPECT_EQ(oss.Grid()(level2_2->uid).value(), GridPosition(2, 1));
}

TEST_F(UTOSS, Iteration) {
  std::unordered_set<PictID> pids{};
  pids.emplace(oss.InsertBase()->uid);
  pids.emplace(oss.InsertBase()->uid);
  pids.emplace(oss.InsertBase()->uid);

  std::unordered_set<PictID> iteration{};
  for (const auto& pict : oss) {
    iteration.emplace(pict.uid);
  }
  EXPECT_EQ(iteration, pids);
  EXPECT_EQ(std::begin(oss)->uid, (*std::begin(oss)).uid);
  EXPECT_EQ(std::begin(oss).operator->(), oss(std::begin(oss)->uid));
}

TEST_F(UTOSS, PictProps) {
  auto pict1 = oss.InsertBase();
  std::string genericString = "no short optimization please";
  oss.SetPictAlias(pict1->uid, genericString);
  EXPECT_EQ(pict1->alias, genericString);
  oss.SetPictTitle(pict1->uid, genericString);
  EXPECT_EQ(pict1->title, genericString);
  oss.SetPictComment(pict1->uid, genericString);
  EXPECT_EQ(pict1->comment, genericString);
  oss.SetPictLink(pict1->uid, { genericString, genericString });
  EXPECT_EQ(pict1->lnk.address, genericString);
  EXPECT_EQ(pict1->lnk.subAddr, genericString);
}

TEST_F(UTOSS, PictPropsInvalid) {
  ASSERT_NO_THROW(oss.SetPictTitle(42U, "test"));
  ASSERT_NO_THROW(oss.SetPictAlias(42U, "test"));
  ASSERT_NO_THROW(oss.SetPictComment(42U, "test"));
  ASSERT_NO_THROW(oss.SetPictLink(42U, { "test", "test" }));
}

TEST_F(UTOSS, PictPropsDuplicate) {
  const auto pict1 = oss.InsertBase();
  EXPECT_NO_THROW(oss.SetPictAlias(pict1->uid, std::string{}));
  EXPECT_NO_THROW(oss.SetPictTitle(pict1->uid, std::string{}));
  EXPECT_NO_THROW(oss.SetPictComment(pict1->uid, std::string{}));
  EXPECT_NO_THROW(oss.SetPictLink(pict1->uid, ccl::oss::MediaLink{}));
}

TEST_F(UTOSS, LoadPict) {
  Pict initialPict{};
  initialPict.uid = 42;
  initialPict.dataType = DataType::tba;
  initialPict.alias = "short";
  initialPict.title = "very long no SSO please";
  initialPict.comment = "";
  initialPict.lnk = { "12", "123" };

  GridPosition pos{ 1, 2 };

  ccl::src::Handle src{};
  src.desc = Descriptor{ ccl::src::SrcType::rsDoc, u8"111.trs" };
  src.coreHash = 42;

  const auto& inserted = oss.LoadPict(Pict{ initialPict }, pos, src, std::make_unique<OperationHandle>());
  EXPECT_EQ(inserted.uid, initialPict.uid);
  EXPECT_EQ(inserted.alias, initialPict.alias);
  EXPECT_EQ(inserted.title, initialPict.title);
  EXPECT_EQ(inserted.comment, initialPict.comment);
  EXPECT_EQ(oss.Ops()(inserted.uid)->type, oss.Ops()(initialPict.uid)->type);
  EXPECT_EQ(oss.Src()(inserted.uid)->desc, src.desc);
  EXPECT_EQ(oss.Src()(inserted.uid)->coreHash, src.coreHash);
  EXPECT_EQ(inserted.lnk, initialPict.lnk);

  const auto maybeID = oss.Grid()(pos);
  ASSERT_TRUE(maybeID.has_value());
  EXPECT_EQ(maybeID.value(), initialPict.uid);
}

TEST_F(UTOSS, LoadPictSameID) {
  Pict pict{};
  pict.uid = 42U;

  const auto& pict1 = oss.LoadPict(Pict{ pict }, GridPosition{}, Handle{}, std::make_unique<OperationHandle>());
  const auto& pict2 = oss.LoadPict(Pict{ pict }, GridPosition{}, Handle{}, std::make_unique<OperationHandle>());
  EXPECT_NE(pict1.uid, pict2.uid);
  EXPECT_EQ(pict1.uid, PictID{ 42U });
  EXPECT_EQ(std::ssize(oss), 2);
}

TEST_F(UTOSS, Erase) {
  const auto pid1 = oss.InsertBase()->uid;
  EXPECT_EQ(std::ssize(oss), 1);
  EXPECT_TRUE(oss.Contains(pid1));
  EXPECT_TRUE(oss.Erase(pid1));
  EXPECT_EQ(std::ssize(oss), 0);
  EXPECT_FALSE(oss.Contains(pid1));
}

TEST_F(UTOSS, EraseInvalid) {
  EXPECT_FALSE(oss.Erase(42U));
  const auto pid1 = oss.InsertBase()->uid;
  EXPECT_FALSE(oss.Erase(pid1 + 1));
}

TEST_F(UTOSS, EraseDouble) {
  const auto pid1 = oss.InsertBase()->uid;
  EXPECT_TRUE(oss.Erase(pid1));
  EXPECT_FALSE(oss.Erase(pid1));
}

TEST_F(UTOSS, EraseUsed) {
  const auto pid1 = oss.InsertBase()->uid;
  const auto pid2 = oss.InsertBase()->uid;
  const auto child = oss.InsertOperation(pid1, pid2)->uid;
  EXPECT_FALSE(oss.Erase(pid1));
  EXPECT_FALSE(oss.Erase(pid2));
  EXPECT_TRUE(oss.Erase(child));
}

TEST_F(UTOSS, EraseMiddle) {
  const auto pid1 = oss.InsertBase()->uid;
  const auto pid2 = oss.InsertBase()->uid;
  const auto pid3 = oss.InsertBase()->uid;
  const auto child1 = oss.InsertOperation(pid1, pid2)->uid;
  const auto child2 = oss.InsertOperation(pid1, pid3)->uid;

  EXPECT_TRUE(oss.Erase(child1));
  EXPECT_FALSE(oss.Contains(child1));
  const std::vector<PictID> parents{ pid1, pid3 };
  EXPECT_EQ(oss.Graph().ParentsOf(child2), parents);
}