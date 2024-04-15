#include "ccl/semantic/RSForm.h"

#include "ccl/rslang/RSExpr.h"

namespace ccl::semantic {

static constexpr auto HASH_MAGIC = 65777;

RSForm::RSForm(const RSForm& rhs) : RSForm() {
  *this = rhs;
}

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26456 ) // Note: false positive warning on hiding deleted function
#endif

RSForm& RSForm::operator=(const RSForm& rhs) {
  if (&rhs != this) {
    title = rhs.title;
    alias = rhs.alias;
    comment = rhs.comment;
    core = rhs.core;
    mods->LoadFrom(rhs.Mods());
    NotifyModification();
  }
  return *this;
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

rsModificationFacet& RSForm::Mods() noexcept { return *mods; }
const rsModificationFacet& RSForm::Mods() const noexcept { return *mods; }
rsOperationFacet& RSForm::Ops() noexcept { return *operations; }
const rsOperationFacet& RSForm::Ops() const noexcept { return *operations; }

const RSCore& RSForm::Core() const noexcept {  return core; }
const CstList& RSForm::List() const noexcept { return core.List(); }
const Thesaurus& RSForm::Texts() const noexcept {  return core.Texts(); }
const Schema& RSForm::RSLang() const noexcept {  return core.RSLang(); }
bool RSForm::Contains(const EntityUID entity) const {  return Core().Contains(entity); }
const RSConcept& RSForm::GetRS(const EntityUID entity) const { return Core().GetRS(entity); }
const TextConcept& RSForm::GetText(const EntityUID entity) const { return Core().GetText(entity); }
const ParsingInfo& RSForm::GetParse(const EntityUID entity) const {  return Core().GetParse(entity); }

void RSForm::UpdateState() {
  core.UpdateState();
}

bool RSForm::SetAliasFor(const EntityUID target, const std::string& newValue, const bool substitue) {
  return NotifyAndReturn(core.SetAliasFor(target, newValue, substitue));
}

void RSForm::ResetAliases() {
  core.ResetAliases();
  NotifyModification();
}

bool RSForm::MoveBefore(const EntityUID what, const ListIterator iWhere) {
  return NotifyAndReturn(core.MoveBefore(what, iWhere));
}

EntityUID RSForm::Load(ConceptRecord&& cst) {
  return core.Load(std::move(cst));
}

EntityUID RSForm::Emplace(const CstType type, const std::string& definition) {
  const auto result = core.Emplace(type, definition);
  NotifyModification();
  return result;
}

EntityUID RSForm::InsertCopy(const EntityUID target, const RSCore& source) {
  const auto result = core.InsertCopy(target, source);
  NotifyModification();
  return result;
}

EntityUID RSForm::InsertCopy(const ConceptRecord& cst) {
  const auto result = core.InsertCopy(cst);
  NotifyModification();
  return result;
}

VectorOfEntities RSForm::InsertCopy(const std::vector<ConceptRecord>& input) {
  auto result = core.InsertCopy(input);
  NotifyModification();
  return result;
}

VectorOfEntities RSForm::InsertCopy(const VectorOfEntities& input, const RSCore& source) {
  auto result = core.InsertCopy(input, source);
  NotifyModification();
  return result;
}

bool RSForm::Erase(const EntityUID target) {
  if (mods->IsTracking(target)) {
    return false;
  } else {
    return NotifyAndReturn(EraseInternal(target));
  }
}

bool RSForm::EraseInternal(const EntityUID target) {
  if (!core.Erase(target)) {
    return false;
  } else {
    mods->Erase(target);
    return true;
  }
}

change::Hash RSForm::FullHash() const {
  std::vector<change::Hash> cstHashes;
  cstHashes.reserve(std::size(core));
  for (const auto uid : core) {
    const auto& rsCst = core.GetRS(uid);
    const auto& textCst = core.GetText(uid);
    change::Hash cstHash = change::HashString(rsCst.alias);
    cstHash += change::HashString(rsCst.definition);
    cstHash += change::HashString(rsCst.convention);
    cstHash += change::HashString(textCst.definition.Raw());
    cstHash += change::HashString(textCst.term.Text().Raw());
    cstHashes.emplace_back(cstHash);
  }

  std::sort(std::begin(cstHashes), std::end(cstHashes));
  change::Hash result{ 1 };
  for (const auto aHash : cstHashes) {
    result = result * HASH_MAGIC + aHash;
  }
  return result;
}

change::Hash RSForm::CoreHash() const {
  std::vector<change::Hash> cstHashes;
  cstHashes.reserve(std::size(core));
  for (const auto uid : core) {
    const auto& rsCst = core.GetRS(uid);
    change::Hash cstHash = change::HashString(rsCst.alias);
    cstHash += change::HashString(rsCst.definition);
    cstHashes.emplace_back(cstHash);
  }

  std::sort(std::begin(cstHashes), std::end(cstHashes));
  change::Hash result{ 1 };
  for (const auto aHash : cstHashes) {
    result = result * HASH_MAGIC + aHash;
  }
  return result;
}

bool RSForm::SetExpressionFor(const EntityUID target, const std::string& expression) {
  if (Mods().IsTracking(target)) {
    return false;
  } else {
    return NotifyAndReturn(core.SetExpressionFor(target, expression));
  }
}

bool RSForm::SetTermFor(const EntityUID target, const std::string& termRef) {
  if (Mods().IsTracking(target) && !Mods()(target)->allowEdit) {
    return false;
  } else if (!core.SetTermFor(target, termRef)) {
    return false;
  } else {
    Mods().OnTermChange(target);
    NotifyModification();
    return true;
  }
}

bool RSForm::SetTermFormFor(const EntityUID target, const std::string& termRef, const lang::Morphology& form) {
  if (Mods().IsTracking(target) && !Mods()(target)->allowEdit) {
    return false;
  } else if (!core.SetTermFormFor(target, termRef, form)) {
    return false;
  } else {
    Mods().OnTermChange(target);
    NotifyModification();
    return true;
  }
}

bool RSForm::SetDefinitionFor(const EntityUID target, const std::string& textDef) {
  if (Mods().IsTracking(target) && !Mods()(target)->allowEdit) {
    return false;
  } else if (!core.SetDefinitionFor(target, textDef)) {
    return false;
  } else {
    Mods().OnDefenitionChange(target);
    NotifyModification();
    return true;
  }
}

bool RSForm::SetConventionFor(const EntityUID target, const std::string& convention) {
  if (Mods().IsTracking(target) && !Mods()(target)->allowEdit) {
    return false;
  } else if (!core.SetConventionFor(target, convention)) {
    return false;
  } else {
    Mods().OnConventionChange(target);
    NotifyModification();
    return true;
  }
}

EntityTranslation RSForm::DeleteDuplicatesInternal() {
  EntityTranslation translation{};
  auto flag = true;
  while (flag) {
    flag = false;
    for (const auto original : List()) {
      const auto& rsCst1 = core.GetRS(original);
      const auto& textCst1 = core.GetText(original);
      if (!rsCst1.IsEmpty() || !textCst1.IsEmpty()) {
        for (const auto copy : List()) {
          const auto& rsCst2 = core.GetRS(copy);
          const auto& textCst2 = core.GetText(copy);
          if (&rsCst1 == &rsCst2 || rsCst1 != rsCst2 || textCst1 != textCst2) {
            continue;
          }
          std::string copyAlias = rsCst2.alias;
          if (EraseInternal(copy)) {
            translation.Insert(copy, original);
            core.TranslateAll(CreateTranslator({ { copyAlias, rsCst1.alias } }));
            flag = true;
            break;
          }
        }
      }
    }
  }
  return translation;
}

void RSForm::EquateTextsOf(const EntityUID delID, const EntityUID hierID, const ops::Equation& params) {
  const auto& delCst = Core().GetText(delID);
  const auto& hierCst = Core().GetText(hierID);

  switch (params.mode) {
  default:
  case ops::Equation::Mode::keepHier:
    break;
  case ops::Equation::Mode::keepDel: {
    core.SetTermFor(hierID, delCst.term.Text().Raw());
    core.SetDefinitionFor(hierCst.uid, delCst.definition.Raw());
    break;
  }
  case ops::Equation::Mode::createNew: {
    core.SetTermFor(hierID, params.arg);
    break;
  }
  }

  StrSubstitutes substitutes{};
  substitutes.emplace(delCst.alias, hierCst.alias);
  core.TranslateTexts(delID, CreateTranslator(substitutes));
}

} // namespace ccl::semantic
