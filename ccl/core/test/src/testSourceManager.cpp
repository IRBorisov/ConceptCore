#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/env/SourceManager.hpp"

class EmptySource : public ccl::src::Source {
  using ccl::src::Source::Source;

public:
  [[nodiscard]] ccl::change::Hash CoreHash() const override { return 0; }
  [[nodiscard]] ccl::change::Hash FullHash() const override { return 0; }
  [[nodiscard]] ccl::src::SrcType Type() const noexcept override { return ccl::src::SrcType::tba; }

  bool WriteData(ccl::meta::UniqueCPPtr<ccl::src::DataStream> /*data*/) override { return false; }
  [[nodiscard]] const ccl::src::DataStream* ReadData() const override { return nullptr; }
  [[nodiscard]] ccl::src::DataStream* AccessData() override { return nullptr; }
};

class UTSourceManager : public ::testing::Test {
protected:
  using Descriptor = ccl::src::Descriptor;
  using SrcType = ccl::src::SrcType;
  using Message = ccl::types::Message;
  using SourceManager = ccl::SourceManager;

protected:
  SourceManager manager{};
};

TEST_F(UTSourceManager, SourceManagerDefault) {
  const Descriptor empty{};
  EmptySource src{};
  EXPECT_FALSE(manager.TestDomain(empty, std::u8string{}));
  EXPECT_EQ(manager.Convert2Global(empty, std::u8string{}), empty);
  EXPECT_EQ(manager.Convert2Local(empty, std::u8string{}), empty);
  EXPECT_EQ(manager.CreateLocalDesc(SrcType{}, std::u8string{}), empty);
  EXPECT_EQ(manager.Find(Descriptor{}), nullptr);
  EXPECT_EQ(manager.GetDescriptor(src), empty);
  EXPECT_EQ(manager.ChangeDescriptor(empty, empty), false);
  EXPECT_EQ(manager.CreateNew(empty), nullptr);
  EXPECT_EQ(manager.Open(empty), nullptr);
  EXPECT_NO_THROW(manager.Close(src));
  EXPECT_EQ(manager.SaveState(src), false);
  EXPECT_NO_THROW(manager.Discard(empty));
}

TEST_F(UTSourceManager, SourceMessage) {
  Descriptor descriptor{ SrcType::rsDoc, u8"testName" };
  const SourceManager::SrcMessage msg{ descriptor };
  EXPECT_EQ(msg.srcID, descriptor);
  EXPECT_EQ(SourceManager::SrcOpened{ Descriptor{} }.Type(), Message::srcCode);
  EXPECT_EQ(SourceManager::SrcClosed{ Descriptor{} }.Type(), Message::srcCode);
  EXPECT_EQ(SourceManager::SrcChanged{ Descriptor{} }.Type(), Message::srcCode);
}