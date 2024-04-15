#pragma once

#include "ccl/rslang/TypeContext.hpp"
#include "ccl/rslang/Typification.h"
#include "ccl/ops/EquationOptions.h"
#include "ccl/Substitutes.hpp"

namespace ccl::semantic { class RSForm; }

namespace ccl::ops {

//! Constituents equation processor
class RSEquationProcessor {
  semantic::RSForm& schema;
  EntityTranslation translation{};

  mutable const EquationOptions* equations{ nullptr };
  mutable StrSubstitutes nameSubstitutes{};

public:
  explicit RSEquationProcessor(semantic::RSForm& schema);

public:
  [[nodiscard]] bool Evaluate(const EquationOptions& options) const;
  [[nodiscard]] bool Execute(const EquationOptions& options);
  [[nodiscard]] const EntityTranslation& Translation() const noexcept { return translation; }

private:
  void Clear() noexcept;
  void ClearMutable() const noexcept;

  bool ResolveCstAndPrecheck() const;
  bool PrecheckFor(EntityUID key, EntityUID value) const;

  bool CheckNonBasicEquations() const;

  void ChangeEquatedCsts();
  void RemoveEquatedCsts();
  void UpdateExpressions();

  [[nodiscard]] rslang::ExpressionType Evaluate(EntityUID uid) const;
};

} // namespace ccl::ops