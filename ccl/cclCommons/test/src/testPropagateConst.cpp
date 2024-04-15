#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/cclMeta.hpp"

// helper classes
struct Base {
  int32_t state{ 1337 };

  virtual ~Base() = default;

  virtual int32_t overloadedFunc() const {
    return 42;
  }
  virtual int32_t overloadedFunc() {
    return 43;
  }
};

struct Derived1 : Base {
  int32_t additionalState{ 42 };

  int32_t overloadedFunc() const override {
    return 1337;
  }
};

struct Derived2 : Base {
  std::string additionalState{ "str42" };

  int32_t overloadedFunc() override {
    return 1337;
  }
};

TEST(UTPropagateConst, AccessValue) {
  using ccl::meta::UniqueCPPtr;
  UniqueCPPtr<int32_t> intCP{ std::make_unique<int32_t>(42) };
  const auto& constRef = intCP;

  EXPECT_EQ(intCP.operator*(), 42);
  EXPECT_EQ(constRef.operator*(), 42);

  EXPECT_EQ(intCP.get(), constRef.get());
  EXPECT_EQ(intCP.operator->(), constRef.get());
  EXPECT_EQ(static_cast<int32_t*>(intCP), constRef.get());
  EXPECT_EQ(static_cast<const int32_t*>(constRef), constRef.get());
}

TEST(UTPropagateConst, Nullptr) {
  using ccl::meta::UniqueCPPtr;
  const UniqueCPPtr<int32_t> intPtr{};
  EXPECT_TRUE(intPtr == nullptr);
  EXPECT_FALSE(intPtr != nullptr);
  EXPECT_EQ(intPtr, UniqueCPPtr<int32_t>(nullptr));

  UniqueCPPtr<int32_t> notNull{ std::make_unique<int32_t>(42) };
  EXPECT_FALSE(notNull == nullptr);
  EXPECT_TRUE(notNull != nullptr);

  notNull = nullptr;
  EXPECT_TRUE(notNull == nullptr);

  notNull = std::make_unique<int32_t>(0);
  EXPECT_FALSE(notNull == nullptr);
}

TEST(UTPropagateConst, PointerEquality) {
  using ccl::meta::UniqueCPPtr;
  UniqueCPPtr<int32_t> int1{ std::make_unique<int32_t>(42) };
  UniqueCPPtr<int32_t> int2{ std::make_unique<int32_t>(42) };
  UniqueCPPtr<int32_t> int3{ std::make_unique<int32_t>(1337) };
  const auto& constRef1 = int1;
  const auto& constRef2 = int2;
  const auto& constRef3 = int3;

  EXPECT_EQ(int1, constRef1);
  EXPECT_NE(int1, int2);
  EXPECT_NE(int1, int3);
  EXPECT_NE(constRef1, int3);
  EXPECT_NE(constRef1, constRef3);
  EXPECT_NE(int1, constRef2);
  EXPECT_NE(constRef1, constRef2);

}

TEST(UTPropagateConst, Swap) {
  using ccl::meta::UniqueCPPtr;
  UniqueCPPtr<int32_t> int1{ std::make_unique<int32_t>(42) };
  std::swap(int1, int1);
  EXPECT_EQ(*int1, 42);

  UniqueCPPtr<int32_t> int2{ std::make_unique<int32_t>(1337) };
  std::swap(int1, int2);
  EXPECT_EQ(*int2, 42);
  EXPECT_EQ(*int1, 1337);
}

TEST(UTPropagateConst, Propagation) {
  using ccl::meta::UniqueCPPtr;
  UniqueCPPtr<Base> nonConstPtr{ std::make_unique<Base>() };
  const auto& constRef = nonConstPtr;
  EXPECT_EQ(nonConstPtr->overloadedFunc(), 43);
  EXPECT_EQ(constRef->overloadedFunc(), 42);
}

TEST(UTPropagateConst, Polymorphism) {
  using ccl::meta::UniqueCPPtr;
  UniqueCPPtr<Base> basePtrToDerived1{}, basePtrToDerived2{};
  basePtrToDerived1 = UniqueCPPtr<Base>{ std::make_unique<Derived1>() };
  basePtrToDerived2 = UniqueCPPtr<Base>{ std::make_unique<Derived2>() };

  ASSERT_FALSE(dynamic_cast<Derived1*>(basePtrToDerived1.get()) == nullptr);
  EXPECT_TRUE(dynamic_cast<Derived2*>(basePtrToDerived1.get()) == nullptr);
  EXPECT_EQ(dynamic_cast<Derived1*>(basePtrToDerived1.get())->additionalState, 42);
  EXPECT_EQ(dynamic_cast<Derived2*>(basePtrToDerived2.get())->additionalState, "str42");
  
  const auto& constRef1 = basePtrToDerived1;
  EXPECT_EQ(basePtrToDerived1->overloadedFunc(), 43);
  EXPECT_EQ(constRef1->overloadedFunc(), 1337);

  const auto& constRef2 = basePtrToDerived2;
  EXPECT_EQ(basePtrToDerived2->overloadedFunc(), 1337);
  EXPECT_EQ(constRef2->overloadedFunc(), 42);
}