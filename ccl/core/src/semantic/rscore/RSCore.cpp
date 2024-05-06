#include "ccl/semantic/RSCore.h"

#include "ccl/rslang/RSExpr.h"

namespace ccl::semantic {

const CstList& RSCore::List() const noexcept { return cstList; }
const Thesaurus& RSCore::Texts() const noexcept { return thesaurus; }
const Schema& RSCore::RSLang() const noexcept { return schema; }

void RSCore::UpdateState() {
  thesaurus.UpdateState();
  schema.UpdateState();
}

bool RSCore::Contains(const EntityUID entity) const {
  return schema.Contains(entity);
}

const RSConcept& RSCore::GetRS(const EntityUID entity) const {
  return schema.At(entity);
}

const TextConcept& RSCore::GetText(const EntityUID entity) const {
  return thesaurus.At(entity);
}

const ParsingInfo& RSCore::GetParse(const EntityUID entity) const {
  return schema.InfoFor(entity);
}

ConceptRecord RSCore::AsRecord(const EntityUID entity) const {
  return ConceptRecord{ schema.At(entity), thesaurus.At(entity) };
}

std::optional<EntityUID> RSCore::FindAlias(const std::string& name) const {
  return RSLang().FindAlias(name);
}

bool RSCore::SetAliasFor(const EntityUID target, const std::string& newValue, const bool substitueMentions) {
  if (!Contains(target)) {
    return false;
  } else if (schema.At(target).alias == newValue) {
    return false;
  } if (!identifiers.TryAlias(schema.At(target).alias, newValue, schema.At(target).type)) {
    return false;
  } else {
    schema.SetAliasFor(target, newValue, substitueMentions);
    thesaurus.SetAliasFor(target, newValue, substitueMentions);
    return true;
  }
}

void RSCore::ResetAliases() {
  StrSubstitutes substitutes{};
  identifiers.Clear();
  for (const auto uid : List()) {
    const auto& cst = GetRS(uid);
    const auto id = identifiers.RegisterEntity(uid, cst.type);
    if (cst.uid != id.uid) {
      throw std::logic_error{ "Generated ID is invalid" };
    }
    if (cst.alias != id.alias) {
      substitutes.insert({ cst.alias, id.alias });
    }
  }
  const auto translator = CreateTranslator(substitutes);
  schema.SubstitueAliases(translator);
  thesaurus.SubstitueAliases(translator);
}

bool RSCore::MoveBefore(const EntityUID what, const ListIterator iWhere) {
  if (!Contains(what)) {
    return false;
  } else if (!cstList.MoveBefore(what, iWhere)) {
    return false;
  } else {
    return true;
  }
}

EntityUID RSCore::Emplace(const CstType type, const std::string& definition) {
  const auto newID = identifiers.GenerateNewID(type);
  schema.Emplace(newID.uid, newID.alias, type, definition);
  thesaurus.Emplace(newID.uid, newID.alias);
  cstList.Insert(newID.uid);
  return newID.uid;
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
EntityUID RSCore::Insert(ConceptRecord&& cst) {
  RSConcept newRS{cst.uid, cst.alias, cst.type, std::move(cst.rs), std::move(cst.convention) };
  TextConcept newText{cst.uid, cst.alias, std::move(cst.term), std::move(cst.definition) };
  const auto newID = identifiers.RegisterID(newRS.uid, newRS.alias, newRS.type);
  newRS.uid = newID.uid;
  newRS.alias = newID.alias;
  newText.uid = newID.uid;
  newText.alias = newID.alias;
  if (cst.alias != newID.alias) {
    const StrSubstitutes substitutes{ { cst.alias, newID.alias } };
    const auto translator = CreateTranslator(substitutes);
    newRS.Translate(translator);
    newText.TranslateRaw(translator);
  }
  schema.Insert(std::move(newRS));
  thesaurus.Insert(std::move(newText));
  cstList.Insert(newID.uid);
  return newID.uid;
}

EntityUID RSCore::InsertCopy(const EntityUID target, const RSCore& source) {
  const auto newID = identifiers.RegisterID(target, source.GetRS(target).alias, source.GetRS(target).type);
  RSConcept newRS = source.GetRS(target);
  newRS.uid = newID.uid;
  if (newRS.alias != newID.alias) {
    const StrSubstitutes substitutes{ { newRS.alias, newID.alias } };
    newRS.alias = newID.alias;
    newRS.Translate(CreateTranslator(substitutes));
  }
  schema.Insert(std::move(newRS));

  TextConcept newText = source.GetText(target);
  newText.uid = newID.uid;
  if (newText.alias != newID.alias) {
    const StrSubstitutes substitutes{ { newText.alias, newID.alias } };
    newText.alias = newID.alias;
    newText.TranslateRaw(CreateTranslator(substitutes));
  }
  thesaurus.Insert(std::move(newText));

  cstList.Insert(newID.uid);
  return newID.uid;
}

EntityUID RSCore::InsertCopy(const ConceptRecord& cst) {
  auto newRS = cst.SpawnRS();
  auto newText = cst.SpawnText();
  const auto newID = identifiers.RegisterID(cst.uid, cst.alias, cst.type);
  newRS.uid = newID.uid;
  newRS.alias = newID.alias;
  newText.uid = newID.uid;
  newText.alias = newID.alias;
  if (cst.alias != newID.alias) {
    const StrSubstitutes substitutes{ { cst.alias, newID.alias } };
    const auto translator = CreateTranslator(substitutes);
    newRS.Translate(translator);
    newText.TranslateRaw(translator);
  }
  schema.Insert(std::move(newRS));
  thesaurus.Insert(std::move(newText));
  cstList.Insert(newID.uid);
  return newID.uid;
}

VectorOfEntities RSCore::InsertCopy(const VectorOfEntities& input, const RSCore& source) {
  VectorOfEntities result{};
  StrSubstitutes replMap{};
  for (const auto uid : input) {
    const auto newID = identifiers.RegisterID(uid, source.GetRS(uid).alias, source.GetRS(uid).type);
    RSConcept newRS = source.GetRS(uid);
    newRS.uid = newID.uid;
    newRS.alias = newID.alias;
    schema.Load(std::move(newRS));

    TextConcept newText = source.GetText(uid);
    newText.uid = newID.uid;
    newText.alias = newID.alias;
    thesaurus.Load(std::move(newText));

    cstList.Insert(newID.uid);
    result.emplace_back(newID.uid);
    if (source.GetRS(uid).alias != newID.alias) {
      replMap.insert({ source.GetRS(uid).alias, newID.alias });
    }
  }
  if (!std::empty(replMap)) {
    const auto translator = CreateTranslator(replMap);
    for (const auto uid : result) {
      Translate(uid, translator);
    }
  }
  UpdateState();
  return result;
}

VectorOfEntities RSCore::InsertCopy(const std::vector<ConceptRecord>& input) {
  VectorOfEntities result{};
  StrSubstitutes replMap{};
  for (const auto& cst : input) {
    const auto newID = identifiers.RegisterID(cst.uid, cst.alias, cst.type);
    RSConcept newRS = cst.SpawnRS();
    newRS.uid = newID.uid;
    newRS.alias = newID.alias;
    schema.Load(std::move(newRS));

    TextConcept newText = cst.SpawnText();
    newText.uid = newID.uid;
    newText.alias = newID.alias;
    thesaurus.Load(std::move(newText));

    cstList.Insert(newID.uid);
    result.emplace_back(newID.uid);
    if (cst.alias != newID.alias) {
      replMap.insert({ cst.alias, newID.alias });
    }
  }
  if (!std::empty(replMap)) {
    const auto translator = CreateTranslator(replMap);
    for (const auto uid : result) {
      Translate(uid, translator);
    }
  }
  UpdateState();
  return result;
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
EntityUID RSCore::Load(ConceptRecord&& cst) {
  RSConcept newRS{ cst.uid, cst.alias, cst.type, std::move(cst.rs), std::move(cst.convention) };
  TextConcept newText{ cst.uid, cst.alias, std::move(cst.term), std::move(cst.definition) };
  const auto newID = identifiers.RegisterID(newRS.uid, newRS.alias, newRS.type);
  newRS.uid = newID.uid;
  newRS.alias = newID.alias;
  newText.uid = newID.uid;
  newText.alias = newID.alias;
  if (cst.alias != newID.alias) {
    const StrSubstitutes substitutes{ { cst.alias, newID.alias } };
    const auto translator = CreateTranslator(substitutes);
    newRS.Translate(translator);
    newText.TranslateRaw(translator);
  }
  schema.Load(std::move(newRS));
  thesaurus.Load(std::move(newText));
  cstList.Insert(newID.uid);
  return newID.uid;
}

bool RSCore::Erase(const EntityUID target) {
  if (!Contains(target)) {
    return false;
  } else {
    identifiers.Erase(target, GetRS(target).alias);
    cstList.Erase(target);
    schema.Erase(target);
    thesaurus.Erase(target);
    return true;
  }
}

size_t RSCore::size() const noexcept {
  return std::size(schema);
}

bool RSCore::SetExpressionFor(const EntityUID target, const std::string& expression) {
  return schema.SetDefinitionFor(target, expression);
}

bool RSCore::SetTermFor(const EntityUID target, const std::string& termRef) {
  return thesaurus.SetTermFor(target, termRef);
}

bool RSCore::SetTermFormFor(const EntityUID target, const std::string& termRef, const lang::Morphology& form) {
  return thesaurus.SetTermFormFor(target, termRef, form);
}

bool RSCore::SetDefinitionFor(const EntityUID target, const std::string& textDef) {
  return thesaurus.SetDefinitionFor(target, textDef);
}

bool RSCore::SetConventionFor(const EntityUID target, const std::string& convention) {
  return schema.SetConventionFor(target, convention);
}

void RSCore::Translate(const EntityUID target, const StrTranslator& old2New) {
  schema.Translate(target, old2New);
  thesaurus.Translate(target, old2New);
}

void RSCore::TranslateAll(const StrTranslator& old2New) {
  schema.TranslateAll(old2New);
  thesaurus.TranslateAll(old2New);
}

void RSCore::TranslateTexts(const EntityUID target, const StrTranslator& old2New) {
  thesaurus.Translate(target, old2New);
}

void RSCore::TranslateDef(const EntityUID target, const StrTranslator& old2New) {
  thesaurus.TranslateDef(target, old2New);
}

void RSCore::TranslateTerm(const EntityUID target, const StrTranslator& old2New) {
  thesaurus.TranslateTerm(target, old2New);
}

RSFormIterator RSCore::begin() const noexcept { return RSFormIterator(std::begin(schema)); }
RSFormIterator RSCore::end() const noexcept {  return RSFormIterator(std::end(schema)); }

RSFormIterator::RSFormIterator(RSIterator iter) noexcept
  : iter{ std::move(iter) } {}

RSFormIterator& RSFormIterator::operator++() noexcept {
  ++iter;
  return *this;
}

RSFormIterator::pointer RSFormIterator::operator->() const noexcept { return &iter.operator->()->uid; }
RSFormIterator::reference RSFormIterator::operator*() const noexcept { return iter.operator*().uid; }
bool RSFormIterator::operator==(const RSFormIterator& iter2) const noexcept {  return iter == iter2.iter; }

} // namespace ccl::semantic
