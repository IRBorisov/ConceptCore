#pragma once

#include "ccl/Entity.hpp"
#include "ccl/semantic/CstType.hpp"
#include "ccl/rslang/RSExpr.h"

namespace ccl::semantic {

//! RS form of a concept
struct RSConcept {
  EntityUID uid{};
  std::string alias{};

  CstType type{ CstType::base };

  std::string definition{};
  std::string convention{};

  explicit RSConcept(
    const EntityUID entity,
    std::string alias,
    const CstType type = CstType::base,
    std::string definition = {},
    std::string convention = {}
  ) noexcept :
    uid{ entity },
    alias{ std::move(alias) },
    type{ type },
    definition{ std::move(definition) },
    convention{ std::move(convention) }
  {}

  RSConcept() = default;

  [[nodiscard]] bool operator==(const RSConcept& rhs) const noexcept;
  [[nodiscard]] bool operator!=(const RSConcept& rhs) const noexcept { return !(*this == rhs); }

  [[nodiscard]] bool IsEmpty() const noexcept;
  [[nodiscard]] bool MatchStr(const std::string& searchStr) const;

  void Translate(const StrTranslator& old2New);
};

} // namespace ccl::semantic
