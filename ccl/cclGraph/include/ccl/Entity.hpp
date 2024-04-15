#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <cstdint>

namespace ccl {
//! Entity identifier
using EntityUID = uint32_t;

//! Unordered set of entities
using SetOfEntities = std::unordered_set<EntityUID>;

//! Ordered vector of entities
using VectorOfEntities = std::vector<EntityUID>;

class EntityTranslation {
  using Container = std::unordered_map<EntityUID, EntityUID>;
  Container relations{};

public:
  using ConstIter = typename Container::const_iterator;
  using size_type = typename Container::size_type;

  [[nodiscard]] ConstIter begin() const noexcept {
    return std::begin(relations);
  }
  [[nodiscard]] ConstIter end() const noexcept {
    return std::end(relations);
  }
  [[nodiscard]] size_type size() const noexcept {
    return std::size(relations);
  }
  [[nodiscard]] bool empty() const noexcept {
    return std::empty(relations);
  }

  void Clear() noexcept {
    relations.clear();
  }
  [[nodiscard]] bool ContainsValue(const EntityUID value) const {
    return std::any_of(begin(), end(), [&](const auto& el) { return el.second == value; });
  }
  [[nodiscard]] bool ContainsKey(const EntityUID key) const {
    return relations.contains(key);
  }
  [[nodiscard]] EntityUID operator()(const EntityUID key) const {
    return relations.at(key);
  }
  void SubstituteValues(const EntityTranslation& substitutes) {
    for (const auto& [key, value] : relations) {
      if (substitutes.ContainsKey(value)) {
        relations[key] = substitutes(value);
      }
    }
  }

  // Note: doesn't break iterators!
  void ReplaceValue(const EntityUID& key, const EntityUID& newVal) {
    relations[key] = newVal;
  }

  // Note: can break iterators!
  void Insert(EntityUID key, EntityUID val) {
    relations.emplace(key, val);
  }
  void Erase(EntityUID key) noexcept {
    relations.erase(key);
  }

  void SuperposeWith(const EntityTranslation& second) {
    SubstituteValues(second);
    for (const auto& iter : second.relations) {
      if (!relations.contains(iter.first)) {
        relations.insert(iter);
      }
    }
  }

  [[nodiscard]] bool operator==(const EntityTranslation& second) const {
    return relations == second.relations;
  }
  [[nodiscard]] bool operator!=(const EntityTranslation& second) const {
    return relations != second.relations;
  }
};

} // namespace ccl

namespace ccl::tag {

using Func_Name2UID = std::function<std::optional<EntityUID>(const std::string&)>;
using Func_UID2Name = std::function<std::optional<std::string>(EntityUID)>;

} // namespace ccl::tag