#include "ccl/semantic/RSModel.h"

namespace ccl::semantic {

rsValuesFacet& RSModel::Values() noexcept { return *dataFacet; }
const rsValuesFacet& RSModel::Values() const noexcept { return *dataFacet; }
rsCalculationFacet& RSModel::Calculations() noexcept { return *calulatorFacet; }
const rsCalculationFacet& RSModel::Calculations() const noexcept { return *calulatorFacet; }

const RSCore& RSModel::Core() const noexcept {
  return core;
}

const CstList& RSModel::List() const noexcept {
  return core.List();
}

const Thesaurus& RSModel::Texts() const noexcept {
  return core.Texts();
}

const Schema& RSModel::RSLang() const noexcept {
  return core.RSLang();
}

bool RSModel::Contains(const EntityUID entity) const {
  return Core().Contains(entity);
}

const RSConcept& RSModel::GetRS(const EntityUID entity) const {
  return Core().GetRS(entity);
}

const TextConcept& RSModel::GetText(const EntityUID entity) const {
  return Core().GetText(entity);
}

const ParsingInfo& RSModel::GetParse(const EntityUID entity) const {
  return Core().GetParse(entity);
}

EntityUID RSModel::Emplace(const CstType type, const std::string& definition) {
  const auto result = core.Emplace(type, definition);
  AfterInsert(result);
  NotifyModification();
  return result;
}

EntityUID RSModel::InsertCopy(const EntityUID target, const RSCore& source) {
  const auto result = core.InsertCopy(target, source);
  AfterInsert(result);
  NotifyModification();
  return result;
}

EntityUID RSModel::InsertCopy(const ConceptRecord& cst) {
  const auto result = core.InsertCopy(cst);
  AfterInsert(result);
  NotifyModification();
  return result;
}

VectorOfEntities RSModel::InsertCopy(const std::vector<ConceptRecord>& input) {
  auto result = core.InsertCopy(input);
  for (const auto uid : result) {
    AfterInsert(uid);
  }
  NotifyModification();
  return result;
}

VectorOfEntities RSModel::InsertCopy(const VectorOfEntities& input, const RSCore& source) {
  auto result = core.InsertCopy(input, source);
  for (const auto uid : result) {
    AfterInsert(uid);
  }
  NotifyModification();
  return result;
}

bool RSModel::MoveBefore(const EntityUID what, const ListIterator iWhere) {
  return NotifyAndReturn(core.MoveBefore(what, iWhere));
}

bool RSModel::SetAliasFor(const EntityUID target, const std::string& newName, const bool substitue) {
  return NotifyAndReturn(core.SetAliasFor(target, newName, substitue));
}

void RSModel::ResetAliases() {
  core.ResetAliases();
  NotifyModification();
}

bool RSModel::Erase(const EntityUID target) {
  if (!core.Erase(target)) {
    return false;
  } else {
    dataFacet->Erase(target);
    calulatorFacet->Erase(target);
    ResetDependants(target);
    NotifyModification();
    return true;
  }
}

void RSModel::UpdateState() {
  core.UpdateState();
}

bool RSModel::SetExpressionFor(const EntityUID target, const std::string& expression) {
  if (!core.SetExpressionFor(target, expression)) {
    return false;
  } else {
    dataFacet->ResetFor(target);
    calulatorFacet->ResetFor(target);
    NotifyModification();
    return true;
  }
}

bool RSModel::SetTermFor(const EntityUID target, const std::string& termRef) {
  return NotifyAndReturn(core.SetTermFor(target, termRef));
}

bool RSModel::SetTermFormFor(const EntityUID target, const std::string& termRef, const lang::Morphology& form) {
  return NotifyAndReturn(core.SetTermFormFor(target, termRef, form));
}

bool RSModel::SetDefinitionFor(const EntityUID target, const std::string& textDef) {
  return NotifyAndReturn(core.SetDefinitionFor(target, textDef));
}

bool RSModel::SetConventionFor(const EntityUID target, const std::string& convention) {
  return NotifyAndReturn(core.SetConventionFor(target, convention));
}

void RSModel::ResetDependants(const EntityUID target) {
  for (const auto dependant : core.RSLang().Graph().ExpandOutputs({ target })) {
    if (const auto type = core.GetRS(dependant).type;
        dependant != target &&
        !IsBaseSet(type)) {
      if (type == CstType::structured) {
        Values().PruneStructure(dependant);
      } else {
        Calculations().ResetFor(dependant);
        Values().ResetFor(dependant);
      }
    }
  }
}

void RSModel::AfterInsert(const EntityUID target) {
  dataFacet->ResetFor(target);
  calulatorFacet->ResetFor(target);
}

EntityUID RSModel::Load(ConceptRecord&& cst) {
  const auto result = core.Load(std::move(cst));
  AfterInsert(result);
  return result;
}

void RSModel::FinalizeLoadingCore() {
  for (const auto uid : core.List()) {
    AfterInsert(uid);
  }
}

} // namespace ccl::semantic
