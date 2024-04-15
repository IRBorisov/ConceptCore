#pragma once

#include "ccl/tools/EntityGenerator.h"
#include "ccl/tools/CstNameGenerator.h"
#include "ccl/Substitutes.hpp"

namespace ccl::semantic {

//! Concept identifier pair in RSCore
struct ConceptID {
  EntityUID uid{};
  std::string alias{};

  [[nodiscard]] constexpr bool operator==(const ConceptID& rhs) const {
    return uid == rhs.uid && alias == rhs.alias;
  }
  [[nodiscard]] constexpr bool operator!=(const ConceptID& rhs) const {
    return !operator==(rhs);
  }
};

//! Constituent identification facet of RSCore
class IdentityManager {
private:
  tools::EntityGenerator idGenerator{};
  tools::CstNameGenerator aliasGenerator{};

public:
  [[nodiscard]] ConceptID GenerateNewID(CstType type);
  [[nodiscard]] ConceptID RegisterID(EntityUID uid, const std::string& name, CstType type);
  [[nodiscard]] ConceptID RegisterEntity(EntityUID uid, CstType type);

  bool TryAlias(const std::string& oldValue, const std::string& newValue, CstType type);
  void Erase(EntityUID target, const std::string& name) noexcept;
  void Clear() noexcept;

private:
  [[nodiscard]] bool NeedNameChangeFor(const std::string& name, CstType type) const;
  
};

} // namespace ccl::semantic
