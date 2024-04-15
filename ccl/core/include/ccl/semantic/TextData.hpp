#pragma once

#include <vector>
#include <optional>
#include <string>
#include <map>
#include <cstdint>

namespace ccl::semantic {

//! Text interpretation
class TextInterpretation {
  using Container = std::map<int32_t, std::string>;
  Container interpretants{};

public:
  using CIter = typename Container::const_iterator;
  using size_type = typename Container::size_type;
  using value_type = typename Container::value_type;
  using mapped_type = typename Container::mapped_type;
  using key_type = typename Container::key_type;

  TextInterpretation() = default;
  explicit TextInterpretation(const std::vector<std::string>& interpr) {
    for (const auto& it : interpr) {
      PushBack(it);
    }
  }
  explicit TextInterpretation(std::initializer_list<value_type> init) {
    for (const auto& [key, value] : init) {
      SetInterpretantFor(key, value);
    }
  }

public:
  void Clear() noexcept { 
    interpretants.clear();
  }
  [[nodiscard]] bool empty() const noexcept {
    return std::empty(interpretants);
  }
  [[nodiscard]] size_type size() const noexcept {
    return std::size(interpretants);
  }
  [[nodiscard]] CIter begin() const noexcept {
    return std::begin(interpretants);
  }
  [[nodiscard]] CIter end() const noexcept {
    return std::end(interpretants);
  }
  
  [[nodiscard]] bool HasInterpretantFor(const int32_t id) const {
    return interpretants.contains(id);
  }
  [[nodiscard]] const std::string& GetInterpretantFor(const int32_t id) const {
    return interpretants.at(id);
  }
  void SetInterpretantFor(const int32_t id, const std::string& val) {
    interpretants[id] = val;
  }
  void EraseInterpretee(const int32_t& id) {
    interpretants.erase(id);
  }

  [[nodiscard]] bool operator==(const TextInterpretation& rhs) const {
    return interpretants == rhs.interpretants;
  }
  [[nodiscard]] bool operator!=(const TextInterpretation& rhs) const {
    return interpretants != rhs.interpretants;
  }

  [[nodiscard]] std::optional<int32_t> GetInterpreteeOf(const std::string& aName) const {
    for (const auto& [key, value] : interpretants) {
      if (value == aName) {
        return key;
      }
    }
    return std::nullopt;
  }

  int32_t PushBack(const std::string& val) {
    // Note: using numeric properties of interpretee
    auto newIndex = static_cast<int32_t>(std::ssize(*this) + 1);
    while (HasInterpretantFor(newIndex)) {
      ++newIndex;
    }
    interpretants.emplace(newIndex, val);
    return newIndex;
  }
};

} // namespace ccl::semantic