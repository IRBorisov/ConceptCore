#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/oss/OSSchema.h"

class UTossGrid: public ::testing::Test {
protected:
  using PictID = ccl::oss::PictID;
  using OSSchema = ccl::oss::OSSchema;
  using ossGridFacet = ccl::oss::ossGridFacet;

protected:
  OSSchema oss{};
  ossGridFacet& grid{ oss.Grid() };

  PictID pid1, pid2, level1;
};

TEST_F(UTossGrid, Empty) {
  EXPECT_FALSE(grid(ccl::oss::ossGridFacet::origin).has_value());
  EXPECT_EQ(grid.MaxColumnIn(0U), 0);
  EXPECT_EQ(grid.MaxColumnIn(42U), 0);
  EXPECT_EQ(grid.CountRows(), 0);
}

TEST_F(UTossGrid, InvalidIDs) {
  const auto invalid = ccl::oss::PictID{ 42U };
  EXPECT_FALSE(grid(invalid).has_value());
  EXPECT_FALSE(grid.ShiftPict(invalid, 1));
}

TEST_F(UTossGrid, Access) {
  pid1 = oss.InsertBase()->uid;
  ASSERT_TRUE(grid(pid1).has_value());
  const auto pos = grid(pid1).value();
  ASSERT_TRUE(grid(pos).has_value());
  EXPECT_EQ(grid(pos).value(), pid1);
}

TEST_F(UTossGrid, CountRows) {
  pid1 = oss.InsertBase()->uid;
  pid2 = oss.InsertBase()->uid;
  EXPECT_EQ(grid.CountRows(), 1);
  level1 = oss.InsertOperation(pid1, pid2)->uid;
  EXPECT_EQ(grid.CountRows(), 2);
}

TEST_F(UTossGrid, MaxColumn) {
  EXPECT_EQ(grid.MaxColumnIn(0), 0);

  pid1 = oss.InsertBase()->uid;
  pid2 = oss.InsertBase()->uid;
  const auto pid3 = oss.InsertBase()->uid;
  level1 = oss.InsertOperation(pid1, pid3)->uid;
  oss.Erase(pid2);
  EXPECT_EQ(grid.MaxColumnIn(0), 3);
}

TEST_F(UTossGrid, ShiftPict) {
  pid1 = oss.InsertBase()->uid;
  const auto basePos = grid(pid1).value();
  ASSERT_TRUE(grid.ShiftPict(pid1, 0));
  ASSERT_TRUE(grid.ShiftPict(pid1, 1));
  EXPECT_EQ(grid(pid1)->column, basePos.column + 1);
  ASSERT_TRUE(grid.ShiftPict(pid1, -1));
  EXPECT_EQ(grid(pid1)->column, basePos.column);
}

TEST_F(UTossGrid, ShiftPictErrors) {
  EXPECT_FALSE(grid.ShiftPict(PictID{42}, PictID{42}));
  EXPECT_FALSE(grid.ShiftPict(PictID{42}, 1));

  pid1 = oss.InsertBase()->uid;
  const auto basePos = grid(pid1).value();
  EXPECT_FALSE(grid.ShiftPict(pid1, -1 - basePos.column));
}

TEST_F(UTossGrid, ShiftPictSwap) {
  pid1 = oss.InsertBase()->uid;
  pid2 = oss.InsertBase()->uid;
  const auto pos1 = grid(pid1).value();
  const auto pos2 = grid(pid2).value();
  grid.ShiftPict(pid1, 1);
  EXPECT_EQ(grid(pid1).value(), pos2);
  EXPECT_EQ(grid(pid2).value(), pos1);
  grid.ShiftPict(pid1, -1);
  EXPECT_EQ(grid(pid1).value(), pos1);
  EXPECT_EQ(grid(pid2).value(), pos2);
}