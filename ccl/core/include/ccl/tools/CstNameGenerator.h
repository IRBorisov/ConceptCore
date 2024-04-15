#pragma once

#include "ccl/semantic/CstType.hpp"

#include <string>
#include <unordered_set>
#include <optional>

namespace ccl::tools {

//! Constituent name generator
class CstNameGenerator {
  std::unordered_set<std::string> names{};

public:
  struct NameElements {
    const semantic::CstType type{ semantic::CstType::base };
    uint32_t index{0};
  };

public:
  void Clear() noexcept;

  [[nodiscard]] std::string NewUID();
  void AddUID(const std::string& newUID);
  void FreeUID(const std::string& returnUID) noexcept;
  [[nodiscard]] bool IsTaken(const std::string& name) const;

  [[nodiscard]] std::string NewNameFor(semantic::CstType type);

  [[nodiscard]] static bool IsNameCorrect(const std::string& name) noexcept;
  [[nodiscard]] static std::optional<semantic::CstType> GetTypeForName(const std::string& name);
};

} // namespace ccl::tools