#pragma once

#include "ccl/lang/Morphology.h"

#include <string>
#include <optional>
#include <vector>

namespace ccl::lang {

class LexicalTerm;

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

//! Term context
class EntityTermContext {
public:
  virtual ~EntityTermContext() noexcept = default;

protected:
  EntityTermContext() = default;
  EntityTermContext(const EntityTermContext&) = default;
  EntityTermContext& operator=(const EntityTermContext&) = default;
  EntityTermContext(EntityTermContext&&) noexcept = default;
  EntityTermContext& operator=(EntityTermContext&&) noexcept = default;

public:
  [[nodiscard]] virtual const LexicalTerm* At(const std::string& /*entity*/) const {
    return nullptr;
  }
  [[nodiscard]] virtual bool Contains(const std::string& /*entity*/) const {
    return false;
  }

  [[nodiscard]] virtual std::unordered_map<std::string, std::string>
    MatchingTerms(const std::string& /*input*/) const {
    return {};
  }
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl::lang