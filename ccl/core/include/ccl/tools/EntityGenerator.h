#pragma once

#include "ccl/Entity.hpp"

#include <random>

namespace ccl::tools {

//! Entity ID generator
class EntityGenerator {
  std::uniform_int_distribution<> distribution{ 0, std::numeric_limits<int32_t>::max() };
  SetOfEntities entities{};

public:
  void Clear() noexcept;

  [[nodiscard]] EntityUID NewUID();
  void AddUID(const EntityUID& newUID);
  void FreeUID(const EntityUID& returnUID) noexcept;
  [[nodiscard]] bool IsTaken(const EntityUID& uid) const;
};

} // namespace ccl::tools