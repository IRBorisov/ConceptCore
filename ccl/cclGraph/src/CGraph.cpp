#include "ccl/graph/CGraph.h"

#include <numeric>
#include <iterator>

 // Disable warnings about using operator[]
#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26446 )
#endif

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wsign-conversion"
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsign-conversion"
#endif
namespace ccl::graph {

void CGraph::Clear() noexcept {
  graph.clear();
  verticies.clear();
}

bool CGraph::Contains(const EntityUID item) const {
  return verticies.contains(item);
}

VertexIndex CGraph::IndexFor(const EntityUID uid) const {
  return verticies.at(uid);
}

void CGraph::AddItem(const EntityUID item) {
  AddInternal(item);
}

VertexIndex CGraph::AddInternal(const EntityUID item) {
  if (!Contains(item)) {
    graph.emplace_back(item);
    const auto index = static_cast<VertexIndex>(size(graph) - 1);
    verticies.emplace(item, index);
    return index;
  } else {
    return IndexFor(item);
  }
}

bool CGraph::ConnectionExists(const EntityUID source, const EntityUID dest) const {
  return Contains(source) && Contains(dest) && 
    HasEdge(IndexFor(source), IndexFor(dest));
}

bool CGraph::HasEdge(const VertexIndex source, const VertexIndex destination) const {
  const auto& outputs = graph[source].outputs;
  return std::find(begin(outputs), end(outputs), destination) != end(outputs);
}

void CGraph::AddConnection(const EntityUID source, const EntityUID dest)  {
  if (!ConnectionExists(source, dest)) {
    const auto srcID = AddInternal(source);
    const auto destID = AddInternal(dest);
    graph[srcID].outputs.push_back(destID);
    graph[destID].inputs.push_back(srcID);
  }
}

void CGraph::EraseItem(const EntityUID item) {
  if (Contains(item)) {
    EraseInternal(item);
  }
}

VertexIndex CGraph::EraseInternal(const EntityUID item) {
  const auto itemID = IndexFor(item);
  for (const auto destination : graph[itemID].outputs) {
    auto& inputs = graph[destination].inputs;
    inputs.erase(std::find(begin(inputs), end(inputs), itemID));
  }
  for (const auto source : graph[itemID].inputs) {
    auto& outputs = graph[source].outputs;
    outputs.erase(std::find(begin(outputs), end(outputs), itemID));
  }
  graph[itemID].isValid = false;
  graph[itemID].inputs.clear();
  graph[itemID].outputs.clear();
  verticies.erase(item);
  return itemID;
}

VertexIndex CGraph::ItemsCount() const noexcept {
  return static_cast<VertexIndex>(ssize(verticies));
}

void CGraph::SetItemInputs(const EntityUID item, const UnorderedItems& connectedItems) {
  const auto itemID = AddInternal(item);
  for (const auto source : graph[itemID].inputs) {
    auto& outputs = graph[source].outputs;
    outputs.erase(std::find(begin(outputs), end(outputs), itemID));
  }
  graph[itemID].inputs.clear();

  for (const auto uid : connectedItems) {
    const auto srcID = AddInternal(uid);
    graph[itemID].inputs.push_back(srcID);
    graph[srcID].outputs.push_back(itemID);
  }
}

CGraph::UnorderedItems CGraph::InputsFor(const EntityUID item) const {
  if (!Contains(item)) {
    return UnorderedItems{};
  } else {
    UnorderedItems result{};
    for (const auto index : graph[IndexFor(item)].inputs) {
      result.emplace(graph[index].uid);
    }
    return result;
  }
}

VertexIndex CGraph::ConnectionsCount() const {
  return std::accumulate(begin(graph), end(graph), 0, 
    [](const VertexIndex val, const Vertex& item) noexcept { return val + static_cast<VertexIndex>(ssize(item.outputs)); });
}

bool CGraph::HasLoop() const {
  std::vector<int8_t> status(size(graph), 0);
  std::vector<VertexIndex> toVisit{};

  for (VertexIndex index = 0; index < ssize(graph); ++index) {
    if (status[index] > 1) {
      continue;
    }
    toVisit.push_back(index);
    while (!empty(toVisit)) {
      const auto item = toVisit.back();
      if (status[item] == 0) {
        status[item] = 1;
        for (const auto child : graph[item].outputs) {
          if (status[child] == 1) {
            return true;
          } else if (status[child] == 0) {
            toVisit.push_back(child);
          }
        }
      } else {
        toVisit.pop_back();
        status[item] = 2;
      }
    }
  }
  return false;
}

bool CGraph::IsReachableFrom(const EntityUID dest, const EntityUID source) const {
  if (ConnectionExists(source, dest)) {
    return true;
  } else if (source == dest) {
    return false;
  } else {
    const auto reachables = ExpandOutputs({ source });
    return std::find(begin(reachables), end(reachables), dest) != end(reachables);
  }
}

CGraph::UnorderedItems CGraph::ExpandOutputs(const UnorderedItems& input) const {
  std::vector<VertexIndex> toVisit{};
  std::vector<bool> marked(size(graph), false);
  for (const auto uid : input) {
    if (Contains(uid)) {
      const auto index = IndexFor(uid);
      marked[index] = true;
      toVisit.push_back(index);
    }
  }

  UnorderedItems result{};
  while (!empty(toVisit)) {
    const auto item = toVisit.back();
    toVisit.pop_back();
    result.emplace(graph[item].uid);
    for (const auto child : graph[item].outputs) {
      if (!marked[child]) {
        marked[child] = true;
        toVisit.push_back(child);
      }
    }
  }

  return result;
}

CGraph::UnorderedItems CGraph::ExpandInputs(const UnorderedItems& input) const {
  std::vector<VertexIndex> toVisit{};
  std::vector<bool> marked(size(graph), false);
  for (const auto uid : input) {
    if (Contains(uid)) {
      const auto index = IndexFor(uid);
      marked[index] = true;
      toVisit.push_back(index);
    }
  }

  UnorderedItems result{};
  while (!empty(toVisit)) {
    const auto item = toVisit.back();
    toVisit.pop_back();
    result.emplace(graph[item].uid);
    for (const auto child : graph[item].inputs) {
      if (!marked[child]) {
        marked[child] = true;
        toVisit.push_back(child);
      }
    }
  }

  return result;
}

std::vector<VertexIndex> CGraph::InternalOrder() const {
  std::vector<VertexIndex> outOrder{};
  outOrder.reserve(size(verticies));

  std::vector<VertexIndex> toVisit{};
  std::vector<int8_t> status(size(graph), 0);
  for (VertexIndex index = 0; index < ssize(graph); ++index) {
    if (status[index] != 0 || !graph[index].isValid) {
      continue;
    }
    toVisit.push_back(index);
    while (!empty(toVisit)) {
      const auto item = toVisit.back();
      if (status[item] == 0) {
        status[item] = 1;
        for (const auto child : graph[item].outputs) {
          if (status[child] == 0) {
            toVisit.push_back(child);
          }
        }
      } else {
        toVisit.pop_back();
        if (status[item] != 2) {
          status[item] = 2;
          outOrder.push_back(item);
        }
      }
    }
  }
  return outOrder;
}

CGraph::OrderedItems CGraph::InverseTopologicalOrder() const {
  OrderedItems outOrder{};
  outOrder.reserve(size(verticies));
  for (const auto index : InternalOrder()) {
    outOrder.push_back(graph[index].uid);
  }
  return outOrder;
}

CGraph::OrderedItems CGraph::Sort(const UnorderedItems& input) const {
  OrderedItems result{};
  TopologicalSort(*this, begin(input), end(input), std::back_inserter(result));
  return result;
}

CGraph::OrderedItems CGraph::TopologicalOrder() const {
  auto result = InverseTopologicalOrder();
  std::reverse(begin(result), end(result));
  return result;
}

CGraph::ItemsGroup CGraph::GetAllLoopsItems() const {
  ItemsGroup result{};

  std::vector<bool> marked(size(graph), false);
  std::vector<VertexIndex> toVisit{};
  std::vector<VertexIndex> component{};
  for (const auto index : InternalOrder()) {
    if (marked[index]) {
      continue;
    }
    component.clear();
    toVisit.push_back(index);
    marked[index] = true;
    while (!empty(toVisit)) {
      const auto item = toVisit.back();
      toVisit.pop_back();
      component.push_back(item);
      for (const auto child : graph[item].outputs) {
        if (!marked[child]) {
          toVisit.push_back(child);
          marked[child] = true;
        }
      }
    }
    if (size(component) != 1 || HasEdge(index, index)) {
      result.emplace_back();
      result.back().reserve(size(component));
      for (const auto item : component) {
        result.back().emplace(graph[item].uid);
      }
    }
  }
  return result;
}

UpdatableGraph::UpdatableGraph(std::function<UnorderedItems(const EntityUID)> externalUpdater)
  : updater{ std::move(externalUpdater) } {}

void UpdatableGraph::UpdateFor(const EntityUID item)  {
  if (!IsBroken()) {
    SetItemInputs(item, updater(item));
  }
}

} // namespace ccl::graph

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif