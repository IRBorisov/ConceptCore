#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/graph/CGraph.h"

#include <vector>

class UTCGraph : public ::testing::Test {
protected:
  using CGraph = ccl::graph::CGraph;
  CGraph graph{};
  const ccl::VectorOfEntities uids{ 0, 1, 42, 3, 44, 5, 6, 7, 8, 9 };

protected:
  void PrepareGraphForExpansion();
};

void UTCGraph::PrepareGraphForExpansion() {
  graph.AddConnection(uids[0], uids[0]);
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[0], uids[2]);
  graph.AddConnection(uids[1], uids[3]);
  graph.AddConnection(uids[3], uids[1]);
  graph.AddConnection(uids[3], uids[4]);
}

TEST_F(UTCGraph, ItemsCount) {
  EXPECT_EQ(graph.ItemsCount(), 0);
  graph.AddItem(uids[0]);
  EXPECT_TRUE(graph.Contains(uids[0]));
  EXPECT_EQ(graph.ItemsCount(), 1);
}

TEST_F(UTCGraph, AddDuplicateItem) {
  graph.AddItem(uids[0]);
  EXPECT_TRUE(graph.Contains(uids[0]));

  graph.AddItem(uids[0]);
  EXPECT_TRUE(graph.Contains(uids[0]));
  EXPECT_EQ(graph.ItemsCount(), 1);
}

TEST_F(UTCGraph, ForeignItem) {
  graph.AddItem(uids[0]);
  EXPECT_FALSE(graph.Contains(uids[1]));
  EXPECT_TRUE(empty(graph.InputsFor(uids[1])));
  EXPECT_TRUE(empty(graph.ExpandOutputs({ uids[1] })));
  EXPECT_TRUE(empty(graph.ExpandInputs({ uids[1] })));

  EXPECT_FALSE(graph.ConnectionExists(uids[1], uids[1]));
  EXPECT_FALSE(graph.ConnectionExists(uids[0], uids[1]));
  EXPECT_FALSE(graph.ConnectionExists(uids[1], uids[0]));

  EXPECT_FALSE(graph.IsReachableFrom(uids[1], uids[1]));
  EXPECT_FALSE(graph.IsReachableFrom(uids[1], uids[0]));
  EXPECT_FALSE(graph.IsReachableFrom(uids[0], uids[1]));
}

TEST_F(UTCGraph, EmptyGroupInput) {
  PrepareGraphForExpansion();
  EXPECT_TRUE(empty(graph.ExpandOutputs({})));
  EXPECT_TRUE(empty(graph.ExpandInputs({})));
}

TEST_F(UTCGraph, Clear) {
  graph.AddItem(uids[0]);
  EXPECT_TRUE(graph.Contains(uids[0]));
  graph.Clear();
  EXPECT_FALSE(graph.Contains(uids[0]));
}

TEST_F(UTCGraph, CopyAndMove) {
  graph.AddItem(uids[0]);
  CGraph copy{ graph };
  EXPECT_TRUE(copy.Contains(uids[0]));
  copy.EraseItem(uids[0]);
  EXPECT_FALSE(copy.Contains(uids[0]));
  EXPECT_TRUE(graph.Contains(uids[0]));

  copy = graph;
  EXPECT_TRUE(copy.Contains(uids[0]));

  CGraph moved(std::move(copy));
  EXPECT_TRUE(moved.Contains(uids[0]));

  CGraph secondMove{};
  secondMove = std::move(moved);
  EXPECT_TRUE(secondMove.Contains(uids[0]));
}

TEST_F(UTCGraph, AddConnection) {
  graph.AddItem(uids[0]);
  graph.AddItem(uids[1]);
  EXPECT_EQ(ssize(graph.InputsFor(uids[0])), 0);
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 0);

  graph.AddConnection(uids[0], uids[1]);
  EXPECT_TRUE(graph.ConnectionExists(uids[0], uids[1]));
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 1);
  EXPECT_EQ(ssize(graph.InputsFor(uids[0])), 0);
}

TEST_F(UTCGraph, ConnectionsCount) {
  EXPECT_EQ(graph.ConnectionsCount(), 0);
  graph.AddItem(uids[0]);
  graph.AddItem(uids[1]);
  graph.AddConnection(uids[0], uids[1]);
  EXPECT_EQ(graph.ConnectionsCount(), 1);
}

TEST_F(UTCGraph, AddDoubleConnection) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[0], uids[1]);
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 1);
  EXPECT_EQ(graph.ConnectionsCount(), 1);
}

TEST_F(UTCGraph, AddSelfConnection) {
  graph.AddConnection(uids[0], uids[0]);
  EXPECT_EQ(ssize(graph.InputsFor(uids[0])), 1);
  EXPECT_EQ(graph.ConnectionsCount(), 1);
}

TEST_F(UTCGraph, AddBiConnection) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[0]);
  EXPECT_TRUE(graph.ConnectionExists(uids[0], uids[1]));
  EXPECT_TRUE(graph.ConnectionExists(uids[1], uids[0]));
  EXPECT_EQ(ssize(graph.InputsFor(uids[0])), 1);
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 1);
  EXPECT_EQ(graph.ConnectionsCount(), 2);
}

TEST_F(UTCGraph, AddConnectionWithItems) {
  EXPECT_EQ(graph.ItemsCount(), 0);

  graph.AddConnection(uids[1], uids[0]);
  EXPECT_EQ(ssize(graph.InputsFor(uids[0])), 1);
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 0);
  EXPECT_EQ(graph.ConnectionsCount(), 1);
}

TEST_F(UTCGraph, CheckInvalidConnection) {
  graph.AddConnection(uids[0], uids[1]);
  EXPECT_TRUE(graph.ConnectionExists(uids[0], uids[1]));
  EXPECT_FALSE(graph.ConnectionExists(uids[1], uids[0]));
  EXPECT_FALSE(graph.ConnectionExists(uids[0], uids[0]));
  EXPECT_FALSE(graph.ConnectionExists(uids[0], uids[2]));
  EXPECT_FALSE(graph.ConnectionExists(uids[2], uids[0]));
  EXPECT_FALSE(graph.ConnectionExists(uids[2], uids[3]));
}

TEST_F(UTCGraph, EraseItem) {
  graph.AddItem(uids[0]);
  graph.AddItem(uids[1]);
  graph.EraseItem(uids[0]);
  EXPECT_EQ(graph.ItemsCount(), 1);
  EXPECT_TRUE(graph.Contains(uids[1]));
  EXPECT_FALSE(graph.Contains(uids[0]));
  EXPECT_EQ(graph.ConnectionsCount(), 0);
}

TEST_F(UTCGraph, EraseConnectedItem) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[0]);
  graph.EraseItem(uids[0]);
  EXPECT_EQ(graph.ItemsCount(), 1);
  EXPECT_EQ(graph.ConnectionsCount(), 0);
  EXPECT_EQ(ssize(graph.InputsFor(uids[1])), 0);

  graph.EraseItem(uids[1]);
  EXPECT_EQ(graph.ItemsCount(), 0);
  EXPECT_EQ(graph.ConnectionsCount(), 0);
}

TEST_F(UTCGraph, SetConnections) {
  graph.SetItemInputs(uids[0], { begin(uids), end(uids) });
  EXPECT_EQ(graph.ItemsCount(), ssize(uids));
  EXPECT_EQ(graph.ConnectionsCount(), ssize(uids));
}

TEST_F(UTCGraph, SetConnectionsWithPreexisting) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[0]);
  graph.SetItemInputs(uids[0], { begin(uids), end(uids) });
  EXPECT_TRUE(graph.ConnectionExists(uids[0], uids[1]));
  EXPECT_EQ(graph.ItemsCount(), ssize(uids));
  EXPECT_EQ(graph.ConnectionsCount(), ssize(uids) + 1);
}

TEST_F(UTCGraph, SetConnectionsReplacing) {
  graph.AddConnection(uids[2], uids[0]);
  graph.AddConnection(uids[0], uids[0]);
  graph.SetItemInputs(uids[0], { uids[1] });
  EXPECT_TRUE(graph.ConnectionExists(uids[1], uids[0]));
  EXPECT_FALSE(graph.ConnectionExists(uids[2], uids[0]));
  EXPECT_EQ(graph.ConnectionsCount(), 1);
}

TEST_F(UTCGraph, EraseAfterSetConnections) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[0]);
  graph.SetItemInputs(uids[0], { uids[2], uids[3] });
  graph.EraseItem(uids[1]);
  EXPECT_EQ(graph.ConnectionsCount(), 2);
}

TEST_F(UTCGraph, SimpleLoop) {
  EXPECT_FALSE(graph.HasLoop());
  graph.AddConnection(uids[0], uids[0]);
  EXPECT_TRUE(graph.HasLoop());
  bool loopFoundCorrect = graph.GetAllLoopsItems() == CGraph::ItemsGroup{ { uids[0] } };
  EXPECT_TRUE(loopFoundCorrect);
}

TEST_F(UTCGraph, ComplexGraph) {
  graph.AddItem(uids[1]);
  graph.AddItem(uids[0]);
  graph.AddItem(uids[9]);
  graph.AddItem(uids[3]);
  graph.AddItem(uids[8]);
  graph.AddItem(uids[5]);
  graph.AddItem(uids[2]);
  graph.AddItem(uids[6]);
  graph.AddItem(uids[4]);
  graph.AddItem(uids[7]);

  graph.AddConnection(uids[2], uids[0]);
  graph.AddConnection(uids[3], uids[0]);
  graph.AddConnection(uids[3], uids[1]);
  graph.AddConnection(uids[4], uids[3]);
  graph.AddConnection(uids[4], uids[2]);
  graph.AddConnection(uids[6], uids[3]);
  graph.AddConnection(uids[6], uids[4]);
  graph.AddConnection(uids[6], uids[2]);
  graph.AddConnection(uids[7], uids[6]);
  graph.AddConnection(uids[7], uids[5]);
  graph.AddConnection(uids[5], uids[0]);
  graph.AddConnection(uids[5], uids[3]);
  graph.AddConnection(uids[8], uids[5]);
  graph.AddConnection(uids[8], uids[7]);
  graph.AddConnection(uids[9], uids[3]);
  graph.AddConnection(uids[9], uids[5]);
  graph.AddConnection(uids[9], uids[8]);

  EXPECT_FALSE(graph.IsReachableFrom(uids[0], uids[1]));
  EXPECT_FALSE(graph.IsReachableFrom(uids[1], uids[0]));
  EXPECT_FALSE(graph.HasLoop());
}

TEST_F(UTCGraph, ComplexLoop) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[2]);
  graph.AddConnection(uids[2], uids[0]);
  graph.AddConnection(uids[2], uids[3]);
  ASSERT_TRUE(graph.HasLoop());

  const std::vector<ccl::SetOfEntities> loop{ { uids[0], uids[1], uids[2] } };
  EXPECT_EQ(graph.GetAllLoopsItems(), loop);
}

TEST_F(UTCGraph, MultipleLoops) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[2]);
  graph.AddConnection(uids[2], uids[0]);
  graph.AddConnection(uids[2], uids[3]);
  graph.AddConnection(uids[3], uids[3]);
  ASSERT_TRUE(graph.HasLoop());

  auto loopsItems = graph.GetAllLoopsItems();
  ASSERT_EQ(ssize(loopsItems), 2);
  const auto loop1 = CGraph::ItemsGroup::value_type{ uids[0], uids[1], uids[2] };
  const auto loop2 = CGraph::ItemsGroup::value_type{ uids[3] };

  EXPECT_TRUE((loopsItems[0] == loop1 && loopsItems[1] == loop2) || 
              (loopsItems[1] == loop1 && loopsItems[0] == loop2));
}

TEST_F(UTCGraph, IsReachableFrom) {
  PrepareGraphForExpansion();
  EXPECT_TRUE(graph.IsReachableFrom(uids[0], uids[0]));
  EXPECT_FALSE(graph.IsReachableFrom(uids[4], uids[4]));
  EXPECT_TRUE(graph.IsReachableFrom(uids[1], uids[0]));
  EXPECT_FALSE(graph.IsReachableFrom(uids[0], uids[1]));
  EXPECT_TRUE(graph.IsReachableFrom(uids[4], uids[0]));
}

TEST_F(UTCGraph, ExpandOutputs) {
  PrepareGraphForExpansion();
  auto expansion = graph.ExpandOutputs({ uids[0] });
  ccl::SetOfEntities testingVector{ uids[0], uids[1], uids[2], uids[3], uids[4] };
  EXPECT_EQ(expansion, testingVector);
  for (const auto id : expansion) {
    if (id != uids[0]) {
      EXPECT_TRUE(graph.IsReachableFrom(id, uids[0]));
    }
  }
}

TEST_F(UTCGraph, ExpandIntoReachableReverse) {
  PrepareGraphForExpansion();
  auto expansion = graph.ExpandInputs({ uids[4] });
  ccl::SetOfEntities testingVector{ uids[0], uids[1], uids[3], uids[4] };
  EXPECT_EQ(expansion, testingVector);
}

TEST_F(UTCGraph, InverseTopologicalOrder) {
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[0], uids[2]);
  graph.AddConnection(uids[2], uids[1]);
  graph.AddConnection(uids[4], uids[3]);
  graph.AddItem(uids[5]);

  auto sorted = graph.InverseTopologicalOrder();
  ASSERT_FALSE(empty(sorted));
  for (auto i = 0U; i < size(sorted) - 1; ++i) {
    for (auto j = i + 1; j < size(sorted); ++j) {
      EXPECT_FALSE(graph.IsReachableFrom(sorted[j], sorted[i]));
    }
  }
}

TEST_F(UTCGraph, TopologicalOrder) {
  EXPECT_TRUE(empty(graph.TopologicalOrder()));
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[0], uids[2]);
  graph.AddConnection(uids[2], uids[1]);
  graph.AddConnection(uids[4], uids[3]);
  graph.AddItem(uids[5]);

  auto sorted = graph.TopologicalOrder();
  ASSERT_FALSE(empty(sorted));
  for (auto i = 0U; i < size(sorted) - 1; ++i) {
    for (auto j = i + 1; j < size(sorted); ++j) {
      EXPECT_FALSE(graph.IsReachableFrom(sorted[i], sorted[j]));
    }
  }
}

TEST_F(UTCGraph, InverseTopologicalOrderLoop) {
  EXPECT_TRUE(empty(graph.InverseTopologicalOrder()));
  graph.AddConnection(uids[0], uids[1]);
  graph.AddConnection(uids[1], uids[2]);
  graph.AddConnection(uids[2], uids[0]);
  graph.AddConnection(uids[3], uids[0]);
  auto sorted = graph.InverseTopologicalOrder();
  EXPECT_TRUE(std::find(begin(sorted), end(sorted), uids[3]) >
              std::find(begin(sorted), end(sorted), uids[0]));
}