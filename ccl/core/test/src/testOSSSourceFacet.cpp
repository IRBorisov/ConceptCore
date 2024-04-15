#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeSourceManager.hpp"

#include "ccl/env/cclEnvironment.h"
#include "ccl/oss/OSSchema.h"

class UTossSrcFacet: public ::testing::Test {
protected:
  using Environment = ccl::Environment;
  using SrcType = ccl::src::SrcType;
  using Descriptor = ccl::src::Descriptor;
  using PictID = ccl::oss::PictID;
  using CstType = ccl::semantic::CstType;
  using OSSchema = ccl::oss::OSSchema;
  using ossSourceFacet = ccl::oss::ossSourceFacet;

protected:
  OSSchema oss{};
  ossSourceFacet& srcFacet{ oss.Src() };

  void SetUp() override {
    Environment::Instance().SetSourceManager(std::make_unique<FakeSourceManager>());
  }
  void TearDown() override {
    Environment::Instance().SetSourceManager(std::make_unique<ccl::SourceManager>());
  }

  FakeSourceManager& SrcManager() {
    return dynamic_cast<FakeSourceManager&>(Environment::Sources());  
  }
  auto CreateBasicConnection() {
    auto& src = SrcManager().CreateNewRS();
    auto pid = oss.InsertBase()->uid;
    srcFacet.ConnectPict2Src(pid, src);
    return std::pair(&src, pid);
  }
  FakeTRS& Source(const PictID pid) {
    return SrcManager().DummyCast(*srcFacet(pid)->src);
  }
};

TEST_F(UTossSrcFacet, InvalidId) {
  const ccl::oss::PictID someID{ 42U };
  EXPECT_TRUE(srcFacet(someID) == nullptr);
  EXPECT_TRUE(srcFacet.OpenSrc(someID) == nullptr);
  EXPECT_TRUE(srcFacet.ActiveSrc(someID) == nullptr);
  EXPECT_TRUE(srcFacet.DataFor(someID) == nullptr);
  EXPECT_NO_THROW(srcFacet.Discard(someID));
  EXPECT_FALSE(srcFacet.Rename(42U, u8"42"));
}

TEST_F(UTossSrcFacet, UnrelatedSource) {
  auto& src = SrcManager().CreateNewRS();
  EXPECT_NO_THROW(src.TriggerOpen());
  EXPECT_NO_THROW(src.TriggerSave());
  EXPECT_NO_THROW(src.TriggerClose());
  EXPECT_FALSE(srcFacet.IsConnectedWith(src));
  EXPECT_FALSE(srcFacet.Src2PID(src).has_value());
}

TEST_F(UTossSrcFacet, MissingSrc) {
  auto[src, pid] = CreateBasicConnection();
  SrcManager().DestroySource(*src);

  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_TRUE(srcFacet.OpenSrc(pid) == nullptr);
  EXPECT_TRUE(srcFacet.ActiveSrc(pid) == nullptr);
  EXPECT_TRUE(srcFacet.DataFor(pid) == nullptr);
  EXPECT_EQ(srcFacet(pid)->desc.type, SrcType::rsDoc);
  EXPECT_FALSE(std::empty(srcFacet(pid)->desc.name));
}

TEST_F(UTossSrcFacet, ActiveSrc) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_EQ(srcFacet.ActiveSrc(pid), src);
  src->TriggerClose();
  EXPECT_TRUE(srcFacet.ActiveSrc(pid) == nullptr);
  EXPECT_EQ(srcFacet.OpenSrc(pid), src);
}

TEST_F(UTossSrcFacet, ConnectPict2Src) {
  auto[src, pid] = CreateBasicConnection();
  ASSERT_EQ(srcFacet(pid)->src, src);
  EXPECT_EQ(srcFacet(pid)->desc.type, SrcType::rsDoc);
  EXPECT_TRUE(src->IsClaimed());
  EXPECT_TRUE(srcFacet.ConnectPict2Src(pid, *src));
}

TEST_F(UTossSrcFacet, ConnectUnsavable) {
  auto& src = SrcManager().CreateNewRS();
  auto pid = oss.InsertBase()->uid;
  src.unsavable = true;
  EXPECT_FALSE(srcFacet.ConnectPict2Src(pid, src));
  EXPECT_FALSE(srcFacet.ConnectSrc2Pict(pid, src));
}

TEST_F(UTossSrcFacet, ConnectDual) {
  auto[src, pid] = CreateBasicConnection();
  const auto pid2 = oss.InsertBase()->uid;
  EXPECT_FALSE(srcFacet.ConnectPict2Src(pid2, *src));
}

TEST_F(UTossSrcFacet, ConnectDataIn) {
  auto& src = SrcManager().CreateNewRS();

  const std::string testStr{ "test" };
  src.schema.title = testStr;
  src.schema.alias = testStr;
  src.schema.comment = testStr;
  const auto hash1 = src.schema.CoreHash();
  const auto hash2 = src.schema.FullHash();

  const auto pid = oss.InsertBase()->uid;
  srcFacet.ConnectPict2Src(pid, src);
  const auto* srcHandle = srcFacet(pid);
  EXPECT_TRUE(src.fullName == srcHandle->desc.name);
  EXPECT_EQ(oss(pid)->title, testStr);
  EXPECT_EQ(oss(pid)->alias, testStr);
  EXPECT_EQ(oss(pid)->comment, testStr);
  EXPECT_EQ(srcHandle->desc.type, src.Type());
  EXPECT_EQ(srcHandle->coreHash, hash1);
  EXPECT_EQ(srcHandle->fullHash, hash2);
}

TEST_F(UTossSrcFacet, ConnectDataOut) {
  auto& src = SrcManager().CreateNewRS();
  EXPECT_FALSE(srcFacet.ConnectSrc2Pict(PictID{ 42 }, src));

  const auto pid = oss.InsertBase()->uid;

  const std::string testStr{ "test" };
  oss.SetPictComment(pid, testStr);
  oss.SetPictTitle(pid, testStr);
  oss.SetPictAlias(pid, testStr);

  ASSERT_TRUE(srcFacet.ConnectSrc2Pict(pid, src));
  const auto* srcHandle = srcFacet(pid);
  EXPECT_EQ(src.schema.alias, testStr);
  EXPECT_EQ(src.schema.comment, testStr);
  EXPECT_EQ(src.schema.title, testStr);
  EXPECT_EQ(src.schema.CoreHash(), srcHandle->coreHash);
  EXPECT_EQ(src.schema.FullHash(), srcHandle->fullHash);
}

TEST_F(UTossSrcFacet, ConnectToAnotherDesc) {
  auto& src1 = SrcManager().CreateNewRS();
  auto src1Desc = ccl::Environment::Sources().GetDescriptor(src1);

  auto& src2 = SrcManager().CreateNewRS();
  auto src2Desc = ccl::Environment::Sources().GetDescriptor(src2);
  ASSERT_NE(src1Desc, src2Desc);

  const auto pid = oss.InsertBase()->uid;

  ASSERT_TRUE(srcFacet.ConnectPict2Src(pid, src1));
  EXPECT_EQ(ccl::Environment::Sources().GetDescriptor(src1), src1Desc);
  EXPECT_EQ(srcFacet(pid)->desc, src1Desc);

  ASSERT_TRUE(srcFacet.ConnectPict2Src(pid, src2));
  EXPECT_EQ(ccl::Environment::Sources().GetDescriptor(src1), src1Desc);
  EXPECT_EQ(ccl::Environment::Sources().GetDescriptor(src2), src2Desc);
  EXPECT_EQ(srcFacet(pid)->desc, src2Desc);
  EXPECT_FALSE(src1.IsOpened());

  EXPECT_FALSE(srcFacet.ConnectSrc2Pict(pid, src1));
  src1.TriggerOpen();
  ASSERT_TRUE(srcFacet.ConnectSrc2Pict(pid, src1));
  EXPECT_EQ(ccl::Environment::Sources().GetDescriptor(src1), src1Desc);
  EXPECT_EQ(ccl::Environment::Sources().GetDescriptor(src2), src2Desc);
  EXPECT_EQ(srcFacet(pid)->desc, src1Desc);
}

TEST_F(UTossSrcFacet, ConnectReconnectToAnother) {
  const auto pid = oss.InsertBase()->uid;
  auto& src1 = SrcManager().CreateNewRS();
  auto& src2 = SrcManager().CreateNewRS();
  srcFacet.ConnectSrc2Pict(pid, src1);
  EXPECT_TRUE(src1.IsOpened());
  EXPECT_TRUE(src2.IsOpened());

  srcFacet.ConnectSrc2Pict(pid, src2);
  EXPECT_FALSE(src1.IsOpened());
  EXPECT_TRUE(src2.IsOpened());
}

TEST_F(UTossSrcFacet, IsConnectedWith) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_TRUE(srcFacet.IsConnectedWith(*src));
}

TEST_F(UTossSrcFacet, IsAssociatedWith) {
  auto[src, pid] = CreateBasicConnection();
  oss.InsertBase();
  const auto descriptor = srcFacet(pid)->desc;
  const Descriptor invalid{ SrcType::rsDoc, u8"43" };
  EXPECT_TRUE(srcFacet.IsAssociatedWith(descriptor));
  EXPECT_FALSE(srcFacet.IsAssociatedWith(invalid));

  src->TriggerClose();
  EXPECT_TRUE(srcFacet.IsAssociatedWith(descriptor));
  EXPECT_FALSE(srcFacet.IsAssociatedWith(invalid));
}

TEST_F(UTossSrcFacet, FindID) {
  auto[src, pid] = CreateBasicConnection();
  ASSERT_TRUE(srcFacet.Src2PID(*src).has_value());
  EXPECT_EQ(srcFacet.Src2PID(*src).value(), pid);
}

TEST_F(UTossSrcFacet, IDForUnrelated) {
  auto& src = SrcManager().CreateNewRS();
  oss.InsertBase();
  EXPECT_FALSE(srcFacet.Src2PID(src).has_value());
}

TEST_F(UTossSrcFacet, Rename) {
  auto[src, pid] = CreateBasicConnection();
  auto newName = src->fullName + u8"42";
  EXPECT_TRUE(srcFacet.Rename(pid, newName));
  EXPECT_EQ(srcFacet(pid)->src, src);
  EXPECT_TRUE(newName == srcFacet(pid)->desc.name);
}

TEST_F(UTossSrcFacet, RenameEmpty) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_FALSE(srcFacet.Rename(pid, u8""));
  EXPECT_EQ(srcFacet(pid)->src, src);
  EXPECT_TRUE(src->fullName != u8"");
  EXPECT_TRUE(src->fullName == srcFacet(pid)->desc.name);
}

TEST_F(UTossSrcFacet, RenameSameName) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_FALSE(srcFacet.Rename(pid, srcFacet(pid)->desc.name));
}

TEST_F(UTossSrcFacet, RenameNoSrc) {
  const auto pid = oss.InsertBase()->uid;
  EXPECT_TRUE(std::empty(srcFacet(pid)->desc.name));

  EXPECT_FALSE(srcFacet.Rename(pid, u8"42"));
  EXPECT_TRUE(std::empty(srcFacet(pid)->desc.name));
}

TEST_F(UTossSrcFacet, RenameMissingSrc) {
  auto[src, pid] = CreateBasicConnection();
  SrcManager().DestroySource(*src);

  EXPECT_FALSE(srcFacet.Rename(pid, u8"42"));
  EXPECT_FALSE(std::empty(srcFacet(pid)->desc.name));
}

TEST_F(UTossSrcFacet, RenameNameTakenByOpenSrc) {
  auto[src, pid] = CreateBasicConnection();
  auto& src2 = SrcManager().CreateNewRS();
  EXPECT_FALSE(srcFacet.Rename(pid, src2.fullName));
}

TEST_F(UTossSrcFacet, RenameNameTakenByClosedSrc) {
  auto[src, pid] = CreateBasicConnection();
  auto& src2 = SrcManager().CreateNewRS();
  src2.TriggerClose();
  EXPECT_FALSE(srcFacet.Rename(pid, src2.fullName));
}

TEST_F(UTossSrcFacet, AutoConnectOtherFile) {
  auto[src1, pid] = CreateBasicConnection();
  src1->TriggerClose();
  auto& src2 = SrcManager().CreateNewRS();
  src2.fullName = src1->fullName;
  src1->fullName = u8"";
  src2.TriggerOpen();
  EXPECT_EQ(srcFacet(pid)->src, &src2);
}

TEST_F(UTossSrcFacet, ReconnectAll) {
  auto[src1, pid1] = CreateBasicConnection();
  auto[src2, pid2] = CreateBasicConnection();
  auto[src3, pid3] = CreateBasicConnection();

  src1->TriggerClose();
  src2->TriggerClose();
  src2->OpenNoTrigger();

  srcFacet.ReconnectAll();
  EXPECT_TRUE(srcFacet(pid1)->src == nullptr);
  EXPECT_TRUE(srcFacet(pid2)->src == src2);
  EXPECT_TRUE(srcFacet(pid3)->src == src3);
}

TEST_F(UTossSrcFacet, ReconnectAllNameChanged) {
  auto[src1, pid] = CreateBasicConnection();
  src1->fullName = u8"42";
  ASSERT_TRUE(u8"42" != srcFacet(pid)->desc.name);

  srcFacet.ReconnectAll();
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_FALSE(src1->IsOpened());
  EXPECT_TRUE(u8"42" == src1->fullName);
}

TEST_F(UTossSrcFacet, DataFor) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_EQ(srcFacet.DataFor(pid), &src->schema);

  src->TriggerClose();
  EXPECT_EQ(srcFacet.DataFor(pid), &src->schema);

  const auto pid2 = oss.InsertBase()->uid;
  EXPECT_TRUE(srcFacet.DataFor(pid2) == nullptr);
}

TEST_F(UTossSrcFacet, DiscardSource) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_TRUE(src->IsClaimed());

  srcFacet.Discard(pid);
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_TRUE(std::empty(*srcFacet(pid)));
  EXPECT_FALSE(src->IsClaimed());
}

TEST_F(UTossSrcFacet, DiscardOnErase) {
  auto[src, pid] = CreateBasicConnection();
  EXPECT_TRUE(src->IsClaimed());
  EXPECT_TRUE(src->IsOpened());

  ASSERT_TRUE(oss.Erase(pid));
  EXPECT_FALSE(src->IsClaimed());
  EXPECT_FALSE(src->IsOpened());
}

TEST_F(UTossSrcFacet, DiscardMissingSource) {
  auto[src, pid] = CreateBasicConnection();
  src->TriggerClose();
  SrcManager().DestroySource(*src);

  srcFacet.Discard(pid);
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_TRUE(std::empty(*srcFacet(pid)));
}

TEST_F(UTossSrcFacet, DestructorCloseSource) {
  auto newOss = std::make_unique<ccl::oss::OSSchema>();
  auto& src = SrcManager().CreateNewRS();
  const auto pid = newOss->InsertBase()->uid;
  newOss->Src().ConnectPict2Src(pid, src);
  EXPECT_TRUE(src.IsOpened());
  newOss = nullptr;
  EXPECT_FALSE(src.IsOpened());
}

TEST_F(UTossSrcFacet, OnChangeUpdate) {
  auto[src, pid] = CreateBasicConnection();
  src->schema.Emplace(CstType::base);
  src->schema.comment = "123";
  EXPECT_NE(srcFacet(pid)->coreHash, src->schema.CoreHash());
  EXPECT_NE(oss(pid)->comment, "123");

  src->TriggerSave();

  EXPECT_EQ(srcFacet(pid)->coreHash, src->schema.CoreHash());
  EXPECT_EQ(oss(pid)->comment, "123");
}

TEST_F(UTossSrcFacet, OnCloseDisconnect) {
  auto[src, pid] = CreateBasicConnection();
  src->TriggerClose();
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_FALSE(src->IsClaimed());
}

TEST_F(UTossSrcFacet, OnOpenConnect) {
  auto[src, pid] = CreateBasicConnection();
  auto* src2 = &SrcManager().CreateNewRS();
  src->TriggerClose();
  src2->TriggerClose();
  src2->TriggerOpen();
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);

  src->TriggerOpen();
  EXPECT_EQ(srcFacet(pid)->src, src);
  EXPECT_TRUE(src->IsClaimed());

  src->TriggerClose();
  SrcManager().rejectDomain = true;
  src->TriggerOpen();
  EXPECT_TRUE(srcFacet(pid)->src == nullptr);
  EXPECT_FALSE(src->IsClaimed());
}