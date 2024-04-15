#pragma once

#include "ccl/Entity.hpp"

#include <unordered_map>
#include <unordered_set>

namespace ccl::graph {

using VertexIndex = int32_t;

//! Entity connections graph
class CGraph {
  struct Vertex {
    EntityUID uid{};
    bool isValid{ true };
    std::vector<VertexIndex> inputs{};
    std::vector<VertexIndex> outputs{};

    Vertex() = default;
    explicit constexpr Vertex(const EntityUID uid) : uid{ uid } {}
  };

  std::vector<Vertex> graph{};
  std::unordered_map<EntityUID, VertexIndex> verticies{};

public:
  using UnorderedItems = SetOfEntities;
  using OrderedItems = VectorOfEntities;
  using ItemsGroup = std::vector<UnorderedItems>;

  void Clear() noexcept;

  [[nodiscard]] bool Contains(EntityUID item) const;
  [[nodiscard]] bool ConnectionExists(EntityUID source, EntityUID dest) const;

  [[nodiscard]] VertexIndex ItemsCount() const noexcept;
  [[nodiscard]] VertexIndex ConnectionsCount() const;
  
  void AddItem(EntityUID item);
  void EraseItem(EntityUID item);
  
  void AddConnection(EntityUID source, EntityUID dest);
  void SetItemInputs(EntityUID item, const UnorderedItems& connectedItems);
  [[nodiscard]] UnorderedItems InputsFor(EntityUID item) const;

  [[nodiscard]] bool IsReachableFrom(EntityUID dest, EntityUID source) const;
  [[nodiscard]] bool HasLoop() const;
  [[nodiscard]] ItemsGroup GetAllLoopsItems() const;

  [[nodiscard]] UnorderedItems ExpandOutputs(const UnorderedItems& input) const;
  [[nodiscard]] UnorderedItems ExpandInputs(const UnorderedItems& input) const;

  [[nodiscard]] OrderedItems TopologicalOrder() const;
  [[nodiscard]] OrderedItems InverseTopologicalOrder() const;
  [[nodiscard]] OrderedItems Sort(const UnorderedItems& input) const;

private:
  [[nodiscard]] VertexIndex IndexFor(EntityUID uid) const;

  VertexIndex AddInternal(EntityUID item);
  VertexIndex EraseInternal(EntityUID item);

  [[nodiscard]] bool HasEdge(VertexIndex source, VertexIndex destination) const;
  [[nodiscard]] std::vector<VertexIndex> InternalOrder() const;
};

template<typename InputIt, typename OutputIt>
void TopologicalSort(const CGraph& graph, InputIt start, InputIt end, OutputIt inserter) {
  if (start != end) {
    for (const auto v : graph.TopologicalOrder()) {
      if (std::find(start, end, v) != end) {
        inserter = v;
      }
    }
  }
}

//! Updatable entity connections graph
class UpdatableGraph : public CGraph {
  std::function<UnorderedItems(EntityUID)> updater;
  bool invalid{ false };

public:
  explicit UpdatableGraph(std::function<UnorderedItems(EntityUID)> externalUpdater);

public:
  void Invalidate() noexcept { invalid = true; }
  void SetValid() noexcept { invalid = false; }
  [[nodiscard]] bool IsBroken() const noexcept { return invalid; }

  void UpdateFor(EntityUID item);
};

} // namespace ccl::graph