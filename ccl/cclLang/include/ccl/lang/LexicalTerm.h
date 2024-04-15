#pragma once

#include "ccl/lang/Morphology.h"
#include "ccl/lang/ManagedText.h"

#include <unordered_map>

namespace ccl::lang {

class EntityTermContext;

//! Complex term with managed text and cached resolution
class LexicalTerm {
  using Forms = std::unordered_map<Morphology, std::string>;

  ManagedText text{};

  Forms manualForms{};
  mutable Forms cachedForms{};

public:
  LexicalTerm() noexcept = default;
  explicit LexicalTerm(std::string ref, std::string resolved) noexcept 
    : text{ ManagedText(std::move(ref), std::move(resolved)) } {}
  explicit LexicalTerm(std::string ref) noexcept 
    : text{ std::move(ref) } {}
  explicit LexicalTerm(ManagedText txt) noexcept 
    : text{ std::move(txt) } {}

public:
  [[nodiscard]] const std::string& Nominal() const noexcept {
    return text.Str();
  }
  [[nodiscard]] const ManagedText& Text()  const  noexcept {
    return text;
  }
  void SetText(std::string_view ref, const EntityTermContext& cntxt);
  void TranslateRefs(const StrTranslator& old2New, const EntityTermContext& cntxt);
  void TranslateRaw(const StrTranslator& old2New);

  [[nodiscard]] bool operator==(const LexicalTerm& t2) const noexcept;
  [[nodiscard]] bool operator!=(const LexicalTerm& t2) const noexcept { return !(*this == t2); }

  void UpdateFrom(const EntityTermContext& cntxt);

  [[nodiscard]] bool MatchStr(const std::string& str) const;

  [[nodiscard]] const std::string& GetForm(const Morphology& form) const;
  [[nodiscard]] const std::string& GetNominalForm() const;
  [[nodiscard]] const std::unordered_map<Morphology, std::string>& GetAllManual() const;

  void SetForm(const Morphology& form, std::string_view formText);
  [[nodiscard]] bool IsFormManual(const Morphology& form) const;
  [[nodiscard]] const std::string& GetManualForm(const Morphology& form) const;

  void ClearForms() noexcept;
};

} // namespace ccl::lang