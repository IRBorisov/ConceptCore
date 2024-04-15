#pragma once

#include "ccl/Substitutes.hpp"
#include "ccl/lang/Reference.h"

#include <unordered_set>

namespace ccl::lang {

class EntityTermContext;

//! Text with managed references
class ManagedText {
  std::string rawText{};
  mutable std::string cache{};

public:
  ManagedText() noexcept = default;
  explicit ManagedText(std::string stringWithRefs, std::string cache) noexcept
    : rawText{ std::move(stringWithRefs) }, cache{ std::move(cache) } {}
  explicit ManagedText(std::string stringWithRefs) noexcept
    : ManagedText(std::move(stringWithRefs), std::string{}) {}

public:
  [[nodiscard]] bool operator==(const ManagedText& str2) const noexcept;
  [[nodiscard]] bool operator!=(const ManagedText& str2) const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] const std::string& Str() const noexcept;
  [[nodiscard]] const std::string& Raw() const noexcept;

  void SetRaw(std::string_view ref);
  void InitFrom(std::string_view ref, const EntityTermContext& cntxt);
  void UpdateFrom(const EntityTermContext& cntxt);

  void TranslateRaw(const StrTranslator& old2New);
  void TranslateRefs(const StrTranslator& old2New, const EntityTermContext& cntxt);

  [[nodiscard]] std::unordered_set<std::string> Referals() const;
};

} // namespace ccl::lang