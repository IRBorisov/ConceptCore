#pragma once

#include "ccl/semantic/InterpretationStorage.h"

namespace ccl::semantic {

class RSModel;
class rsCalculationFacet;

//! Constituent values manager facet of RSModel
class rsValuesFacet final : public meta::MutatorFacet<RSModel> {
  friend class RSModel;
  friend class rsCalculationFacet;

  std::unique_ptr<InterpretationStorage> storage{ nullptr };
  std::unordered_map<EntityUID, bool> statements{};

public:
  explicit rsValuesFacet(RSModel& core);

public:
  [[nodiscard]] std::optional<object::StructuredData> SDataFor(EntityUID entity) const;
  [[nodiscard]] std::optional<bool> StatementFor(EntityUID entity) const;
  [[nodiscard]] const TextInterpretation* TextFor(EntityUID entity) const;

  void ResetDataFor(EntityUID target);

  std::optional<int32_t> AddBasicElement(EntityUID target, const std::string& name);
  bool SetBasicText(EntityUID target, const TextInterpretation& newInterp);

  // Note: runtime requires CstType == STRUCTURE
  bool SetStructureData(EntityUID target, object::StructuredData input);

  // Note: Serialization only!!
  void LoadData(EntityUID target, object::StructuredData input);
  void LoadData(EntityUID target, bool logicVal);
  void LoadData(EntityUID target, const TextInterpretation& newInterp);

private:
  bool SetRSInternal(EntityUID target, const object::StructuredData& input);
  bool SetTextInternal(EntityUID target, const TextInterpretation& newInterp);
  bool SetStatementInternal(EntityUID target, bool logicVal);

  void Erase(EntityUID target) noexcept;
  void ResetFor(EntityUID target);
  void PruneStructure(EntityUID target);
  void ResetAll();
  void ResetAllExceptCore();

private:
  [[nodiscard]] bool CheckBasicElements(const object::StructuredData& data, const rslang::Typification& type) const;
};

} // namespace ccl::semantic
