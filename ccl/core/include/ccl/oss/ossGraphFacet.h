#pragma once

#include "ccl/oss/Pict.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::oss {

class OSSchema;

class ossGraphFacet final : public meta::ConstFacet<OSSchema> {
  friend class OSSchema;

  using AdjacencyList = std::vector<std::vector<size_t>>;
  AdjacencyList graph{};
  std::vector<PictID> items{};

public:
  explicit ossGraphFacet(const OSSchema& core) noexcept
    : ConstFacet<OSSchema>(core) {}

public:
  [[nodiscard]] std::vector<PictID> ParentsOf(PictID pid) const;
  [[nodiscard]] std::vector<PictID> ChildrenOf(PictID pid) const;
  [[nodiscard]] std::optional<size_t> ParentIndex(PictID parent, PictID child) const;

  [[nodiscard]] std::vector<std::pair<PictID, PictID>> EdgeList() const;

  [[nodiscard]] std::vector<PictID> ExecuteOrder() const;

  // Note: use only for Serialization!
  bool LoadParent(PictID child, PictID parent);

private:
  void AddItem(PictID item, const std::vector<PictID>& connectedItems);
  void Erase(PictID item);

  size_t InsertKeyValue(PictID item);
  [[nodiscard]] size_t Item2ID(PictID item);
  [[nodiscard]] std::vector<size_t> Item2ID(const std::vector<PictID>& input);
  [[nodiscard]] std::optional<size_t> FindItemIndex(PictID item) const;
  [[nodiscard]] std::vector<PictID> Index2PIDs(const std::vector<size_t>& input) const;
};

} // namespace ccl::oss