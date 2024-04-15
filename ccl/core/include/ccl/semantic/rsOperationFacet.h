#pragma once

#include "ccl/ops/EquationOptions.h"
#include "ccl/ops/RSEquationProcessor.h"

namespace ccl::semantic {

class RSForm;

//! Operations facet of RSForm
class rsOperationFacet final : public meta::MutatorFacet<RSForm> {
  friend class RSForm;

  std::unique_ptr<ops::RSEquationProcessor> equationProcessor;

public:
  ~rsOperationFacet() noexcept = default;
  rsOperationFacet(const rsOperationFacet&) = delete;
  rsOperationFacet& operator=(const rsOperationFacet&) = delete;

  explicit rsOperationFacet(RSForm& core);

public:
  [[nodiscard]] EntityTranslation Identity() const;

  std::optional<EntityTranslation> ExtrapolateFromPrevious(const RSForm& oldShema, 
                                                           const EntityTranslation& oldToNew);
  EntityTranslation MergeWith(const RSForm& schema2);
  EntityTranslation DeleteDuplicates();

  [[nodiscard]] bool IsEquatable(const ops::EquationOptions& equations) const;
  std::optional<EntityTranslation> Equate(const ops::EquationOptions& equations);
};

} // namespace ccl::semantic
