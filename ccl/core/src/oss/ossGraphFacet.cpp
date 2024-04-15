#include "ccl/oss/ossGraphFacet.h"

#include "ccl/oss/OSSchema.h"

namespace ccl::oss {

bool ossGraphFacet::LoadParent(const PictID child, const PictID parent) {
  if (child == parent) {
    return false;
  } else {
    const auto iChild = Item2ID(child);
    const auto iParent = Item2ID(parent);
    if (std::find(begin(graph.at(iChild)), end(graph.at(iChild)), iParent) != end(graph.at(iChild)) ||
        std::find(begin(graph.at(iParent)), end(graph.at(iParent)), iChild) != end(graph.at(iParent))) {
      return false;
    } else {
      graph.at(iChild).emplace_back(iParent);
      return true;
    }
  }
}

void ossGraphFacet::AddItem(const PictID item, const std::vector<PictID>& connectedItems) {
  graph.at(Item2ID(item)) = Item2ID(connectedItems);
}

void ossGraphFacet::Erase(const PictID item) {
  if (core.Contains(item)) {
    if (const auto index = FindItemIndex(item); index.has_value()) {
      graph.erase(begin(graph) + static_cast<ptrdiff_t>(index.value()));
      items.erase(begin(items) + static_cast<ptrdiff_t>(index.value()));
      std::for_each(begin(graph), end(graph), [&](auto& adjecentItems) {
        std::for_each(begin(adjecentItems), end(adjecentItems), [&](auto& refIndex) {
          if (refIndex > index.value()) {
            --refIndex;
          }
        });
      });
    }
  }
}

std::vector<PictID> ossGraphFacet::ParentsOf(const PictID pid) const {
  if (const auto index = FindItemIndex(pid); index.has_value()) {
    return Index2PIDs(graph.at(index.value()));
  } else {
    return std::vector<PictID>{};
  }
}

std::vector<PictID> ossGraphFacet::ChildrenOf(const PictID pid) const {
  if (auto index = FindItemIndex(pid); !index.has_value()) {
    return std::vector<PictID>{};
  } else {
    std::vector<size_t> result{};
    for (auto i = 0U; i < size(graph); ++i) {
      if (i != index.value()) {
        for (auto j : graph.at(i)) {
          if (j == index.value()) {
            result.push_back(i);
          }
        }
      }
    }
    return Index2PIDs(result);
  }
}

std::optional<size_t> ossGraphFacet::ParentIndex(const PictID parent, const PictID child) const {
  const auto parentID = FindItemIndex(parent);
  if (!parentID.has_value()) {
    return std::nullopt;
  } 
  const auto childID = FindItemIndex(child);
  if (!childID.has_value()) {
    return std::nullopt;
  }

  size_t index{ 0 };
  for (const auto& parentNode : graph.at(childID.value())) {
    if (parentNode == parentID.value()) {
      return index;
    }
    ++index;
  }
  return std::nullopt;
}

std::vector<std::pair<PictID, PictID>> ossGraphFacet::EdgeList() const {
  std::vector<std::pair<PictID, PictID>> result{};
  for (auto i = 0U; i < size(graph); ++i) {
    for (auto j : graph.at(i)) {
      result.emplace_back(items.at(i), items.at(j));
    }
  }
  return result;
}

std::vector<PictID> ossGraphFacet::ExecuteOrder() const {
  std::vector<PictID> result{};
  result.reserve(size(items));
  for (auto i = 0U; i < size(graph); ++i) {
    if (size(graph.at(i)) > 0) {
      result.emplace_back(items.at(i));
    }
  }
  return result;
}

std::optional<size_t> ossGraphFacet::FindItemIndex(const PictID item) const {
  size_t index{ 0 };
  for (const auto& pid : items) {
    if (pid == item) {
      return index;
    }
    ++index;
  }
  return std::nullopt;
}

size_t ossGraphFacet::InsertKeyValue(const PictID item) {
  graph.emplace_back(std::vector<size_t>{});
  items.emplace_back(item);
  return size(items) - 1U;
}

size_t ossGraphFacet::Item2ID(const PictID item) {
  if (const auto index = FindItemIndex(item); index.has_value()) {
    return index.value();
  } else {
    return InsertKeyValue(item);
  }
}

std::vector<size_t> ossGraphFacet::Item2ID(const std::vector<PictID>& input) {
  std::vector<size_t> result{};
  result.reserve(size(input));
  for (auto uid : input) {
    result.emplace_back(Item2ID(uid));
  }
  return result;
}

std::vector<PictID> ossGraphFacet::Index2PIDs(const std::vector<size_t>& input) const {
  std::vector<PictID> result{};
  result.reserve(size(input));
  for (auto index : input) {
    result.emplace_back(items.at(index));
  }
  return result;
}

} // namespace ccl::oss