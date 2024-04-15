#pragma once

#include "ccl/semantic/TextData.hpp"
#include "ccl/rslang/StructuredData.h"
#include "ccl/Entity.hpp"

namespace ccl::semantic {

//! Entity interpretation manager
class InterpretationStorage {
  using TextContainer = std::unordered_map<EntityUID, TextInterpretation>;
  using InterpContainer = std::unordered_map<EntityUID, object::StructuredData>;

  TextContainer textData{};
  InterpContainer rsData{};

public:
  void Clear() noexcept;
  void Erase(EntityUID target) noexcept;

  [[nodiscard]] const TextInterpretation* GetTextInterpretationFor(EntityUID uid) const;
  int32_t AddInterpretantFor(EntityUID uid, const std::string& name);
  void SetTextInterpretationFor(EntityUID uid, const TextInterpretation& newInterp);

  [[nodiscard]] std::optional<object::StructuredData> GetRSIFor(EntityUID uid) const;
  void SetRSInterpretationFor(EntityUID uid, object::StructuredData val);
};

} // namespace ccl::semantic