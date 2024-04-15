#pragma once

#include "ccl/Entity.hpp"
#include "ccl/Substitutes.hpp"
#include "ccl/lang/LexicalTerm.h"

namespace ccl::semantic {

//! Text form of a concept
class TextConcept {
public:
  EntityUID uid{};
  std::string alias{};

  lang::LexicalTerm term{};
  lang::ManagedText definition{};

public:
  explicit TextConcept() noexcept = default;
  explicit TextConcept(const EntityUID entity, std::string alias,
                       lang::LexicalTerm term = {}, lang::ManagedText definition = {}) noexcept
    : uid{ entity }, alias{ std::move(alias) }, term{ std::move(term) }, definition{ std::move(definition) } {}

public:
  [[nodiscard]] bool operator==(const TextConcept& rhs) const noexcept;
  [[nodiscard]] bool operator!=(const TextConcept& rhs) const noexcept { return !(*this == rhs); }

  [[nodiscard]] bool IsEmpty() const noexcept;
  [[nodiscard]] bool MatchStr(const std::string& searchStr) const;

  void Translate(const StrTranslator& old2New, const lang::EntityTermContext& cntxt);
  void TranslateRaw(const StrTranslator& old2New);
};

} // namespace ccl::semantic
