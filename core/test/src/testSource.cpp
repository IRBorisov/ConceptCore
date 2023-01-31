#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/Source.hpp"

using ccl::src::CallbackManager;

struct FakeSource : ccl::src::Source {
	using DataStream = ccl::src::DataStream;
	using Hash = ccl::change::Hash;
	using SrcType = ccl::src::SrcType;

	[[nodiscard]] Hash CoreHash() const override { return 42; }
	[[nodiscard]] Hash FullHash() const override { return 43; }
	[[nodiscard]] SrcType Type() const noexcept override { return SrcType::rsDoc; }

	bool WriteData(ccl::meta::UniqueCPPtr<DataStream> /*data*/) override { return true; }
	[[nodiscard]] const DataStream* ReadData() const override { return nullptr; }
	[[nodiscard]] DataStream* AccessData() override { return nullptr; }
};

struct FakeCallback : ccl::src::SourceCallback {
	mutable uint32_t queryUseCount{ 0 };
	mutable uint32_t queryConnectCount{ 0 };

	ccl::change::Usage QueryEntityIsUsed(const ccl::src::Source& /*from*/, 
																			 ccl::EntityUID /*entity*/) const override {
		++queryUseCount;
		return ccl::change::Usage::asElement;
	}
	bool IsConnectedWith(const ccl::src::Source& /*src*/) const override {
		++queryConnectCount;
		return false;
	}
};

TEST(UTSource, SourceClaim) {
	FakeSource src{};
	EXPECT_FALSE(src.IsClaimed());
	src.Claim();
	EXPECT_TRUE(src.IsClaimed());
}

TEST(UTSource, SourceReleaseClaim) {
	FakeSource src{};
	src.Claim();
	EXPECT_TRUE(src.IsClaimed());
	src.ReleaseClaim();
	EXPECT_FALSE(src.IsClaimed());
}

TEST(UTSource, SourceDoubleClaim) {
	FakeSource src{};
	src.Claim();
	src.Claim();
	EXPECT_TRUE(src.IsClaimed());
	src.ReleaseClaim();
	EXPECT_FALSE(src.IsClaimed());
}

TEST(UTSource, DescriptorCompare) {
	using ccl::src::Descriptor;
	EXPECT_EQ(Descriptor{}, Descriptor{});
	const Descriptor desc{ ccl::src::SrcType::rsDoc, u8"42" };
	const auto copy = desc;
	EXPECT_EQ(desc, copy);
	EXPECT_EQ(desc, Descriptor(ccl::src::SrcType::rsDoc, u8"42"));
	EXPECT_NE(desc, Descriptor(ccl::src::SrcType::rsDoc, u8"43"));
	EXPECT_NE(desc, Descriptor(ccl::src::SrcType::tba, u8"42"));
}

TEST(UTSource, HandleIsEmpty) {
	ccl::src::Handle handle{ ccl::src::SrcType::rsDoc };
	EXPECT_TRUE(std::empty(handle));
	
	handle.desc.name = u8"42";
	EXPECT_FALSE(std::empty(handle));
	handle.desc.name = u8"";
	EXPECT_TRUE(std::empty(handle));
	
	FakeSource src{};
	handle.src = &src;
	EXPECT_FALSE(std::empty(handle));
}

TEST(UTSource, HandleUpdateHashes) {
	ccl::src::Handle handle{ ccl::src::SrcType::rsDoc };
	FakeSource src{};
	handle.src = &src;
	EXPECT_EQ(0U, handle.fullHash);
	EXPECT_EQ(0U, handle.coreHash);

	handle.UpdateHashes();
	EXPECT_EQ(43u, handle.fullHash);
	EXPECT_EQ(42u, handle.coreHash);
}

TEST(UTSource, HandleDiscardSrc) {
	ccl::src::Handle handle{ ccl::src::SrcType::rsDoc };
	handle.desc.name = u8"42";
	FakeSource src{};
	handle.src = &src;
	handle.UpdateHashes();

	handle.DiscardSrc();
	EXPECT_EQ(0U, handle.fullHash);
	EXPECT_EQ(0U, handle.coreHash);
	EXPECT_EQ(nullptr, handle.src);
	EXPECT_TRUE(u8"" == handle.desc.name);
}

TEST(UTSource, AddCallback) {
	FakeCallback cb1{};
	FakeCallback cb2{};
	CallbackManager manager{};

	EXPECT_TRUE(manager.AddCallback(cb1));
	EXPECT_FALSE(manager.AddCallback(cb1));
	EXPECT_TRUE(manager.AddCallback(cb2));
}

TEST(UTSource, RemoveCallback) {
	FakeCallback cb{};
	CallbackManager manager{};

	EXPECT_NO_THROW(manager.RemoveCallback(cb));
	
	EXPECT_TRUE(manager.AddCallback(cb));
	EXPECT_NO_THROW(manager.RemoveCallback(cb));
	EXPECT_TRUE(manager.AddCallback(cb));

	EXPECT_NO_THROW(manager.RemoveCallback(cb));
}

TEST(UTSource, QueryIsUsed) {
	FakeCallback cb{};
	CallbackManager manager{};
	manager.AddCallback(cb);

	const FakeSource src{};
	EXPECT_FALSE(manager.QuerySrcUse(src));
	EXPECT_EQ(1u, cb.queryConnectCount);
	EXPECT_EQ(0U, cb.queryUseCount);
	EXPECT_FALSE(manager.QuerySrcUse(src));
	EXPECT_EQ(2u, cb.queryConnectCount);
}

TEST(UTSource, QueryEntityUse) {
	FakeCallback cb{};
	CallbackManager manager{};
	manager.AddCallback(cb);

	const FakeSource src{};
	const ccl::EntityUID uid{ 42 };
	EXPECT_EQ(ccl::change::Usage::asElement, manager.QueryEntityUse(src, uid));
	EXPECT_EQ(0U, cb.queryConnectCount);
	EXPECT_EQ(1u, cb.queryUseCount);
	EXPECT_EQ(ccl::change::Usage::asElement, manager.QueryEntityUse(src, uid));
	EXPECT_EQ(2u, cb.queryUseCount);
}

TEST(UTSource, ImportCallbacksFrom) {
	FakeCallback cb{};
	CallbackManager manager1{};
	manager1.AddCallback(cb);
	EXPECT_NO_THROW(manager1.ImportCallbacksFrom(manager1));

	CallbackManager manager2{};
	EXPECT_NO_THROW(manager2.ImportCallbacksFrom(manager1));

	const FakeSource src{};
	EXPECT_FALSE(manager1.QuerySrcUse(src));
	EXPECT_EQ(1u, cb.queryConnectCount);

	EXPECT_FALSE(manager2.QuerySrcUse(src));
	EXPECT_EQ(2u, cb.queryConnectCount);
}