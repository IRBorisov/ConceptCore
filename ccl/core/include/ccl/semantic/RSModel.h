#pragma once

#include "ccl/semantic/RSCore.h"

#include "ccl/semantic/rsValuesFacet.h"
#include "ccl/semantic/rsCalculationFacet.h"
#include "ccl/cclChange.hpp"

namespace ccl::semantic {

//! RSModel - data interpretation of RSForm system of concepts
class RSModel : public change::ObservableMods {
  friend class rsValuesFacet;
  friend class rsCalculationFacet;

public:
  std::string title{};
  std::string alias{};
  std::string comment{};

private:
  RSCore core{};
  std::unique_ptr<rsValuesFacet> dataFacet{ std::make_unique<rsValuesFacet>(*this) };
  std::unique_ptr<rsCalculationFacet> calulatorFacet{ std::make_unique<rsCalculationFacet>(*this) };

public:
  ~RSModel() noexcept = default;
  RSModel() = default;
  RSModel(const RSModel&) = delete;
  RSModel& operator=(const RSModel&) = delete;

public:
  [[nodiscard]] const RSCore& Core() const noexcept;
  [[nodiscard]] const CstList& List() const noexcept;
  [[nodiscard]] const Thesaurus& Texts() const noexcept;
  [[nodiscard]] const Schema& RSLang() const noexcept;

  [[nodiscard]] rsValuesFacet& Values() noexcept;
  [[nodiscard]] const rsValuesFacet& Values() const noexcept;
  [[nodiscard]] rsCalculationFacet& Calculations() noexcept;
  [[nodiscard]] const rsCalculationFacet& Calculations() const noexcept;

  [[nodiscard]] bool Contains(EntityUID entity) const;
  [[nodiscard]] const RSConcept& GetRS(EntityUID entity) const;
  [[nodiscard]] const TextConcept& GetText(EntityUID entity) const;
  [[nodiscard]] const ParsingInfo& GetParse(EntityUID entity) const;

  EntityUID Emplace(CstType type, const std::string& definition = {});
  EntityUID InsertCopy(EntityUID target, const RSCore& source);
  EntityUID InsertCopy(const ConceptRecord& cst);
  VectorOfEntities InsertCopy(const VectorOfEntities& input, const RSCore& source);
  VectorOfEntities InsertCopy(const std::vector<ConceptRecord>& input);

  bool MoveBefore(EntityUID what, ListIterator iWhere);

  bool Erase(EntityUID target);

  bool SetAliasFor(EntityUID target, const std::string& newName, bool substitue = true);
  bool SetExpressionFor(EntityUID target, const std::string& expression);
  bool SetTermFor(EntityUID target, const std::string& termRef);
  bool SetTermFormFor(EntityUID target, const std::string& termRef, const lang::Morphology& form);
  bool SetDefinitionFor(EntityUID target, const std::string& textDef);
  bool SetConventionFor(EntityUID target, const std::string& convention);

  void ResetAliases();

  // Note: use only for creating from empty state
  EntityUID Load(ConceptRecord&& cst);
  [[nodiscard]] RSCore& LoadCore() noexcept { return core; }
  void FinalizeLoadingCore();
  void UpdateState();

private:
  [[nodiscard]] inline bool NotifyAndReturn(const bool result) {
    if (result) {
      NotifyModification();
    }
    return result;
  }
  void AfterInsert(EntityUID target);
  void ResetDependants(EntityUID target);
};

} // namespace ccl::semantic
