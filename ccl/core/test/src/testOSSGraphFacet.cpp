#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/oss/OSSchema.h"

class UTossGraph: public ::testing::Test {
protected:
  using PictID = ccl::oss::PictID;
  using OSSchema = ccl::oss::OSSchema;
  using ossGraphFacet = ccl::oss::ossGraphFacet;
  using GridIndex = ccl::oss::GridIndex;

protected:
  OSSchema oss{};
  ossGraphFacet& graph{ oss.Graph() };

  const PictID pid1{ oss.InsertBase()->uid };
  const PictID pid2{ oss.InsertBase()->uid };
  const PictID level1{ oss.InsertOperation(pid1, pid2)->uid };
};

TEST_F(UTossGraph, InvalidID) {
  PictID invalid{ 0 };
  while (oss.Contains(invalid)) {
    ++invalid;
  }

  EXPECT_FALSE(graph.LoadParent(invalid, invalid));
  EXPECT_TRUE(std::empty(graph.ParentsOf(invalid)));
  EXPECT_TRUE(std::empty(graph.ChildrenOf(invalid)));
  EXPECT_FALSE(graph.ParentIndex(invalid, invalid).has_value());
  EXPECT_FALSE(graph.ParentIndex(invalid, pid1).has_value());
  EXPECT_FALSE(graph.ParentIndex(invalid, level1).has_value());
  EXPECT_FALSE(graph.ParentIndex(pid1, invalid).has_value());
  EXPECT_FALSE(graph.ParentIndex(level1, invalid).has_value());
}

TEST_F(UTossGraph, ParentsOf) {
  EXPECT_TRUE(std::empty(graph.ParentsOf(pid1)));
  EXPECT_TRUE(std::empty(graph.ParentsOf(pid2)));

  const std::vector<PictID> parents{ pid1, pid2 };
  EXPECT_EQ(graph.ParentsOf(level1), parents);

  const std::vector<PictID> reverseParents = { pid2, pid1 };
  EXPECT_NE(graph.ParentsOf(level1), reverseParents);
}

TEST_F(UTossGraph, ChildrenOf) {
  EXPECT_TRUE(std::empty(graph.ChildrenOf(level1)));
  const std::vector<PictID> children{ level1 };
  EXPECT_EQ(graph.ChildrenOf(pid1), children);
  EXPECT_EQ(graph.ChildrenOf(pid2), children);
}

TEST_F(UTossGraph, ParentIndexInvalid) {
  EXPECT_FALSE(graph.ParentIndex(pid1, pid2).has_value());
  EXPECT_FALSE(graph.ParentIndex(pid2, pid1).has_value());
  EXPECT_FALSE(graph.ParentIndex(pid1, pid1).has_value());
  EXPECT_FALSE(graph.ParentIndex(level1, level1).has_value());
  EXPECT_FALSE(graph.ParentIndex(level1, pid1).has_value());
  EXPECT_FALSE(graph.ParentIndex(level1, pid2).has_value());
}

TEST_F(UTossGraph, ParentIndexUnrelated) {
  const auto pid3 = oss.InsertBase()->uid;
  EXPECT_FALSE(graph.ParentIndex(pid3, pid1).has_value());
  EXPECT_FALSE(graph.ParentIndex(pid1, pid3).has_value());
  EXPECT_FALSE(graph.ParentIndex(pid3, level1).has_value());
  EXPECT_FALSE(graph.ParentIndex(level1, pid3).has_value());
}

TEST_F(UTossGraph, ParentIndex) {
  EXPECT_EQ(graph.ParentIndex(pid1, level1).value(), 0U);
  EXPECT_EQ(graph.ParentIndex(pid2, level1).value(), 1U);

  const PictID level2{ oss.InsertOperation(pid2, level1)->uid };
  EXPECT_EQ(graph.ParentIndex(pid2, level2).value(), 0U);
  EXPECT_EQ(graph.ParentIndex(level1, level2).value(), 1U);
}

TEST_F(UTossGraph, ExecuteOrder) {
  const auto pid3 = oss.InsertBase()->uid;
  for (auto i = 0; i < 10; ++i) {
    oss.InsertBase();
  }
  const auto level2 = oss.InsertOperation(level1, pid3)->uid;
  std::unordered_map<PictID, GridIndex> indexes{};
  auto id = 0;
  for (const auto uid : graph.ExecuteOrder()) {
    indexes.insert({ uid, id });
    ++id;
  }
  ASSERT_EQ(id, 2);
  EXPECT_TRUE(indexes.at(level1) < indexes.at(level2));
}

TEST_F(UTossGraph, AddParentSelf) {
  EXPECT_FALSE(graph.LoadParent(pid1, pid1));
  EXPECT_FALSE(graph.LoadParent(level1, level1));
}

TEST_F(UTossGraph, AddParentDouble) {
  const auto pid3 = oss.InsertBase()->uid;
  EXPECT_TRUE(graph.LoadParent(level1, pid3));
  EXPECT_FALSE(graph.LoadParent(level1, pid3));
}

TEST_F(UTossGraph, AddParentSimmetricalEdge) {
  EXPECT_FALSE(graph.LoadParent(pid1, level1));
  EXPECT_FALSE(graph.LoadParent(pid2, level1));
}

TEST_F(UTossGraph, EdgeList) {
  const auto edges = graph.EdgeList();
  ASSERT_EQ(size(edges), 2U);
  EXPECT_TRUE(std::find(begin(edges), end(edges), std::pair(level1, pid1)) != end(edges));
  EXPECT_TRUE(std::find(begin(edges), end(edges), std::pair(level1, pid2)) != end(edges));
}