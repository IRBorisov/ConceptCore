#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/IdentityManager.h"

class UTIdentityManager: public ::testing::Test {
protected:
  using CstType = ccl::semantic::CstType;
  using ConceptID = ccl::semantic::ConceptID;
  using IdentityManager = ccl::semantic::IdentityManager;
  using EntityUID = ccl::EntityUID;

protected:
  static constexpr EntityUID x1{ 42 };
  static constexpr EntityUID x2{ 43 };
  static constexpr EntityUID d1{ 44 };

  IdentityManager manager{};
};

TEST_F(UTIdentityManager, ConceptIDComparison) {
  {
    EXPECT_EQ(ConceptID{}, ConceptID{});
  }

  const ConceptID test{x1, "X1"};
  {
    const ConceptID copy{ test };
    EXPECT_EQ(test, test);
    EXPECT_EQ(test, copy);
  }
  {
    ConceptID modifiedID{ x2, "X1" };
    EXPECT_NE(modifiedID, test);
  }
  {
    ConceptID modifiedName{ x1, "X2" };
    EXPECT_NE(modifiedName, test);
  }
  {
    ConceptID modifiedBoth{ x2, "X2" };
    EXPECT_NE(modifiedBoth, test);
  }
}

TEST_F(UTIdentityManager, GenerateID) {
  const auto x1ID = manager.GenerateNewID(CstType::base);
  const auto x2ID = manager.GenerateNewID(CstType::base);
  const auto c1ID = manager.GenerateNewID(CstType::constant);
  EXPECT_EQ(x1ID.alias, "X1");
  EXPECT_EQ(x2ID.alias, "X2");
  EXPECT_EQ(c1ID.alias, "C1");
  EXPECT_NE(x1ID.uid, c1ID.uid);
  EXPECT_NE(x1ID.uid, x2ID.uid);
  EXPECT_NE(x2ID.uid, c1ID.uid);
}

TEST_F(UTIdentityManager, RegisterID) {
  const auto x1ID = manager.RegisterID(x1, "X10", CstType::base);
  EXPECT_EQ(x1ID.alias, "X10");
  EXPECT_EQ(x1ID.uid, x1);

  const auto x2ID = manager.RegisterID(x2, "X10", CstType::base);
  EXPECT_EQ(x2ID.alias, "X1");
  EXPECT_EQ(x2ID.uid, x2);

  const auto x3ID = manager.RegisterID(x2, "X3", CstType::base);
  EXPECT_EQ(x3ID.alias, "X3");
  EXPECT_NE(x3ID.uid, x2);

  const auto c1ID = manager.RegisterID(d1, "X2", CstType::term);
  EXPECT_EQ(c1ID.alias, "D1");
  EXPECT_EQ(c1ID.uid, d1);
}

TEST_F(UTIdentityManager, RegisterEntity) {
  const auto x1ID = manager.RegisterEntity(x1, CstType::base);
  EXPECT_EQ(x1ID.alias, "X1");
  EXPECT_EQ(x1ID.uid, x1);

  const auto x2ID = manager.RegisterEntity(x2, CstType::base);
  EXPECT_EQ(x2ID.alias, "X2");
  EXPECT_EQ(x2ID.uid, x2);

  const auto x3ID = manager.RegisterEntity(x2, CstType::base);
  EXPECT_EQ(x3ID.alias, "X3");
  EXPECT_NE(x3ID.uid, x2);
}

TEST_F(UTIdentityManager, TryAlias) {
  EXPECT_FALSE(manager.TryAlias("X1", "X2", CstType::base));

  const auto x1ID = manager.RegisterID(x1, "X1", CstType::base);
  EXPECT_FALSE(manager.TryAlias(x1ID.alias, x1ID.alias, CstType::base));
  EXPECT_FALSE(manager.TryAlias(x1ID.alias, "X2", CstType::constant));
  EXPECT_TRUE(manager.TryAlias(x1ID.alias, "X2", CstType::base));
  EXPECT_FALSE(manager.TryAlias(x1ID.alias, "C1", CstType::constant));
  EXPECT_TRUE(manager.TryAlias("X2", "C1", CstType::constant));

  const auto d1ID = manager.RegisterID(d1, "D1", CstType::term);
  EXPECT_FALSE(manager.TryAlias(d1ID.alias, "C1", CstType::constant));
  EXPECT_TRUE(manager.TryAlias(d1ID.alias, "C2", CstType::constant));
}

TEST_F(UTIdentityManager, Erase) {
  ASSERT_NO_THROW(manager.Erase(x1, "X1"));

  const auto x1ID = manager.GenerateNewID(CstType::base);
  const auto x2ID = manager.RegisterID(x1ID.uid, x1ID.alias, CstType::base);
  EXPECT_NE(x1ID, x2ID);
  manager.Erase(x1ID.uid, x1ID.alias);
  const auto x3ID = manager.RegisterID(x1ID.uid, x1ID.alias, CstType::base);
  EXPECT_EQ(x1ID, x3ID);
}

TEST_F(UTIdentityManager, Clear) {
  ASSERT_NO_THROW(manager.Clear());

  const auto x1ID = manager.GenerateNewID(CstType::base);
  manager.Clear();
  const auto x2ID = manager.GenerateNewID(CstType::base);
  EXPECT_EQ(x1ID.alias, x2ID.alias);
}