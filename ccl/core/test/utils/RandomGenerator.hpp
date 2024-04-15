#pragma once

#include <random>
#include <vector>
#include <unordered_set>

class RNG_Helper {
  std::mt19937 generator;

public:
  explicit RNG_Helper(const uint32_t seed = 42U)
    : generator{ seed } {}

public:
  uint32_t operator()() {
    return static_cast<uint32_t>(generator());
  }

  std::vector<uint32_t> Sequence(uint32_t count) {
    std::vector<uint32_t> result{};
    while (size(result) < count) {
      result.emplace_back(operator()());
    }
    return result;
  }

  std::unordered_set<uint32_t> Unique(uint32_t count) {
    std::unordered_set<uint32_t> result{};
    while (size(result) < count) {
      result.insert(operator()());
    }
    return result;
  }
};