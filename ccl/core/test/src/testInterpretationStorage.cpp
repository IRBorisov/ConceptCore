#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"
#include "ccl/semantic/InterpretationStorage.h"
#include "ccl/rslang/StructuredData.h"

class UTInterpretationStorage : public ::testing::Test {
protected:
  using InterpretationStorage = ccl::semantic::InterpretationStorage;
  using Factory = ccl::object::Factory;

protected:
  UTInterpretationStorage();

  InterpretationStorage storage{};

  static constexpr ccl::EntityUID VALID_UID_BASE = 12345678;
  static constexpr ccl::EntityUID VALID_UID_TERM = 12345;
  static inline const std::string VALID_INTERPRETANT = "Peter";
};

UTInterpretationStorage::UTInterpretationStorage() {
  auto id = storage.AddInterpretantFor(VALID_UID_BASE, VALID_INTERPRETANT);
  auto rsi = Factory::Val(id);
  rsi = Factory::Singleton(rsi);
  storage.SetRSInterpretationFor(VALID_UID_TERM, rsi);
}

TEST_F(UTInterpretationStorage, Clear) {
  storage.Clear();
  EXPECT_FALSE(storage.GetTextInterpretationFor(VALID_UID_BASE) != nullptr);
  EXPECT_FALSE(storage.GetTextInterpretationFor(VALID_UID_TERM) != nullptr);
  EXPECT_FALSE(storage.GetRSIFor(VALID_UID_BASE).has_value());
  EXPECT_FALSE(storage.GetRSIFor(VALID_UID_TERM).has_value());
}

TEST_F(UTInterpretationStorage, ClearUID) {
  storage.Erase(VALID_UID_TERM);
  EXPECT_TRUE(storage.GetTextInterpretationFor(VALID_UID_BASE) != nullptr);
  EXPECT_FALSE(storage.GetTextInterpretationFor(VALID_UID_TERM) != nullptr);
  EXPECT_TRUE(storage.GetRSIFor(VALID_UID_BASE).has_value());
  EXPECT_FALSE(storage.GetRSIFor(VALID_UID_TERM).has_value());
}

TEST_F(UTInterpretationStorage, AddInterpretantToBase) {
  auto initialSize = storage.GetTextInterpretationFor(VALID_UID_BASE)->size();
  auto initialRSI = storage.GetRSIFor(VALID_UID_BASE);
  storage.AddInterpretantFor(VALID_UID_BASE, "Generic Name");
  EXPECT_EQ(storage.GetTextInterpretationFor(VALID_UID_BASE)->size(), initialSize + 1);
  EXPECT_EQ(storage.GetRSIFor(VALID_UID_BASE)->B().Cardinality(), initialRSI->B().Cardinality() + 1);
}

TEST_F(UTInterpretationStorage, AccessInvalidID) {
  EXPECT_FALSE(storage.GetRSIFor(42).has_value());
  EXPECT_TRUE(storage.GetTextInterpretationFor(42) == nullptr);
}