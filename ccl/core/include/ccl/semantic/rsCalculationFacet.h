#pragma once

#include "ccl/Entity.hpp"
#include "ccl/rslang/Interpreter.h"

namespace ccl::semantic {

class RSModel;

//! Evaluation status enumeration
enum class EvalStatus : uint8_t {
  UNKNOWN = 0, //
  NEVER_CALCULATED = 1, // Интерпретация не вычислялась
  INCALCULABLE = 2, // Невозможно вычислить
  AXIOM_FAIL = 3, // Значение аксиомы = FALSE (только для аксиом)
  EMPTY = 4, // Значение пусто (только для множеств)
  HAS_DATA = 5, // Интерпретация вычислена и непуста
};

//! Evaluation facet for RSModel
class rsCalculationFacet final : public meta::MutatorFacet<RSModel> {
  friend class RSModel;

  SetOfEntities calculatedEntities{};
  std::unique_ptr<rslang::Interpreter> calculator{ nullptr };

public:
  explicit rsCalculationFacet(RSModel& core);

public:
  [[nodiscard]] EvalStatus operator()(EntityUID entity) const;
  [[nodiscard]] bool WasCalculated(EntityUID entity) const;

  [[nodiscard]] rslang::DataContext Context() const;
  void RecalculateAll();
  bool Calculate(EntityUID target);

  [[nodiscard]] size_t Count(EvalStatus status) const;

  // Note: Use ONLY in serialization
  void LoadCalulated(const SetOfEntities& calculated) { 
    calculatedEntities = calculated;
  }

private:
  void Erase(EntityUID target) noexcept;
  void ResetFor(EntityUID target) noexcept;

private:
  bool CalculateCstInternal(EntityUID target);
};

} // namespace ccl::semantic
