#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/RSForm.h"

class UTRSMods : public ::testing::Test {
protected:
  using RSForm = ccl::semantic::RSForm;
  using rsModificationFacet = ccl::semantic::rsModificationFacet;
  using CstType = ccl::semantic::CstType;
  using Morphology = ccl::lang::Morphology;

protected:
  RSForm schema{};
  rsModificationFacet& mods{ schema.Mods() };
  const ccl::EntityUID x1{ schema.Emplace(CstType::base) };
  const ccl::EntityUID x_2{ schema.Emplace(CstType::base) };
  const ccl::EntityUID x_3{ schema.Emplace(CstType::base) };
};

TEST_F(UTRSMods, Track) {
  EXPECT_FALSE(mods.IsTracking(x1));
  mods.Track(x1);
  EXPECT_TRUE(mods.IsTracking(x1));
  EXPECT_FALSE(mods.IsTracking(x_2));
  EXPECT_FALSE(mods.IsTracking(x_3));

  EXPECT_FALSE(mods(x1)->definition);
  EXPECT_FALSE(mods(x1)->term);
  EXPECT_FALSE(mods(x1)->convention);
  EXPECT_FALSE(mods(x1)->allowEdit);
}

TEST_F(UTRSMods, TrackInvalid) {
  EXPECT_TRUE(schema.Erase(x1));
  ASSERT_FALSE(schema.Contains(x1));
  EXPECT_FALSE(mods.IsTracking(x1));
  mods.Track(x1);
  EXPECT_FALSE(mods.IsTracking(x1));
}

TEST_F(UTRSMods, StopTracking) {
  mods.Track(x1);
  EXPECT_TRUE(mods.IsTracking(x1));
  EXPECT_FALSE(mods(x1) == nullptr);

  mods.StopTracking(x1);
  EXPECT_FALSE(mods.IsTracking(x1));
  EXPECT_TRUE(mods(x1) == nullptr);
}

TEST_F(UTRSMods, StopTrackingInvalid) {
  EXPECT_TRUE(schema.Erase(x1));
  EXPECT_NO_THROW(mods.StopTracking(x1 + 1));
}

TEST_F(UTRSMods, AccessInvalid) {
  EXPECT_TRUE(schema.Erase(x1));
  ASSERT_FALSE(schema.Contains(x1));
  EXPECT_EQ(mods(x1), nullptr);
}

TEST_F(UTRSMods, Access) {
  EXPECT_TRUE(mods(x1) == nullptr);
  mods.Track(x1);
  EXPECT_FALSE(mods(x1) == nullptr);
}

TEST_F(UTRSMods, ResetAll) {
  mods.Track(x1);
  mods.Track(x_2);

  mods.ResetAll();
  EXPECT_FALSE(mods.IsTracking(x1));
  EXPECT_FALSE(mods.IsTracking(x_2));
}

TEST_F(UTRSMods, RSExprDefault) {
  mods.Track(x1);
  EXPECT_FALSE(schema.SetExpressionFor(x1, "42"));
  EXPECT_TRUE(schema.SetExpressionFor(x_2, "42"));
}

TEST_F(UTRSMods, RSExprEnabledEdit) {
  mods.Track(x1);
  mods(x1)->allowEdit = true;
  EXPECT_FALSE(schema.SetExpressionFor(x1, "42"));
  EXPECT_TRUE(schema.SetExpressionFor(x_2, "42"));
}

TEST_F(UTRSMods, DontAllowEdit) {
  mods.Track(x1);
  mods(x1)->allowEdit = false;

  EXPECT_FALSE(schema.SetConventionFor(x1, "42"));
  EXPECT_TRUE(schema.SetConventionFor(x_2, "42"));

  EXPECT_FALSE(schema.SetDefinitionFor(x1, "42"));
  EXPECT_TRUE(schema.SetDefinitionFor(x_2, "42"));

  EXPECT_FALSE(schema.SetTermFor(x1, "42"));
  EXPECT_TRUE(schema.SetTermFor(x_2, "42"));
}

TEST_F(UTRSMods, TrackChanges) {
  mods.Track(x1);
  auto* tracker = mods(x1);
  tracker->allowEdit = true;

  EXPECT_FALSE(tracker->definition);
  EXPECT_TRUE(schema.SetDefinitionFor(x1, "42"));
  EXPECT_TRUE(tracker->definition);

  EXPECT_FALSE(tracker->convention);
  EXPECT_TRUE(schema.SetConventionFor(x1, "42"));
  EXPECT_TRUE(tracker->convention);

  EXPECT_FALSE(tracker->term);
  EXPECT_TRUE(schema.SetTermFor(x1, "42"));
  EXPECT_TRUE(tracker->term);

  tracker->term = false;
  EXPECT_TRUE(schema.SetTermFormFor(x1, "43", Morphology{}));
  EXPECT_TRUE(tracker->term);
}