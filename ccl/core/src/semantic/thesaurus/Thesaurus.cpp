#include "ccl/semantic/Thesaurus.h"

namespace ccl::semantic {

Thesaurus::Thesaurus()
  : context(*this),
  termGraph{ [&thesaurus = *this] (const EntityUID entity) {
      auto globalIDs = thesaurus.At(entity).term.Text().Referals();
      SetOfEntities uids{};
      for (const auto& name : globalIDs) {
        if (const auto uid = thesaurus.FindAlias(name); uid.has_value()) {
          uids.emplace(uid.value());
        }
      }
      return uids;
    }
  }, defGraph{ [&thesaurus = *this] (const EntityUID entity) {
    auto globalIDs = thesaurus.At(entity).definition.Referals();
      SetOfEntities uids{};
      for (const auto& name : globalIDs) {
        if (const auto uid = thesaurus.FindAlias(name); uid.has_value()) {
          uids.emplace(uid.value());
        }
      }
      return uids;
    }
  }
{}

Thesaurus::Thesaurus(const Thesaurus& rhs) : Thesaurus() {
  storage = rhs.storage; // NOLINT
  termGraph.Invalidate();
  defGraph.Invalidate();
}

Thesaurus& Thesaurus::operator=(const Thesaurus& rhs) {
  if (&rhs != this) {
    storage = rhs.storage;
    termGraph.Clear();
    termGraph.Invalidate();
    defGraph.Clear();
    defGraph.Invalidate();
  }
  return *this;
}

// NOLINTNEXTLINE(bugprone-exception-escape) TODO: fix noexcept
Thesaurus::Thesaurus(Thesaurus&& rhs) noexcept : Thesaurus() {
  storage = std::move(rhs.storage); // NOLINT
  termGraph.Invalidate();
  defGraph.Invalidate();
}

Thesaurus& Thesaurus::operator=(Thesaurus&& rhs) noexcept {
  if (&rhs != this) {
    storage = std::move(rhs.storage);
    termGraph.Clear();
    termGraph.Invalidate();
    defGraph.Clear();
    defGraph.Invalidate();
  }
  return *this;
}

bool Thesaurus::Contains(const EntityUID entity) const {
  return storage.contains(entity);
}

const TextConcept& Thesaurus::At(const EntityUID entity) const {
  return storage.at(entity);
}

std::optional<EntityUID> Thesaurus::FindAlias(const std::string& alias) const {
  auto result = std::find_if(begin(), end(),
                             [&alias](const auto& record) noexcept { return record.alias == alias; });
  if (result != end()) {
    return result->uid;
  } else {
    return std::nullopt;
  }
}

const graph::CGraph& Thesaurus::TermGraph() const {
  if (termGraph.IsBroken()) {
    termGraph.Clear();
    termGraph.SetValid();
    for (const auto& cst : *this) {
      termGraph.UpdateFor(cst.uid);
    }
  }
  return termGraph;
}

const graph::CGraph& Thesaurus::DefGraph() const {
  if (defGraph.IsBroken()) {
    defGraph.Clear();
    defGraph.SetValid();
    for (const auto& cst : *this) {
      defGraph.UpdateFor(cst.uid);
    }
  }
  return defGraph;
}

const lang::EntityTermContext& Thesaurus::Context() const noexcept { return context; }

bool Thesaurus::SetAliasFor(const EntityUID target, const std::string& newValue, const bool substitueMentions) {
  if (!Contains(target)) {
    return false;
  } else if (At(target).alias == newValue) {
    return false;
  }
  termGraph.Invalidate();
  defGraph.Invalidate();
  if (!substitueMentions) {
    storage.at(target).alias = newValue;
    UpdateState();
  } else {
    const auto oldValue = At(target).alias;
    storage.at(target).alias = newValue;
    TranslateAll(CreateTranslator({ { oldValue, newValue } }));
  }
  return true;
}

bool Thesaurus::SetTermFor(const EntityUID target, const std::string& termRef) {
  if (!Contains(target)) {
    return false;
  } else if (storage.at(target).term.Text().Raw() == termRef) {
    return false;
  } else {
    storage.at(target).term.SetText(termRef, context);
    termGraph.UpdateFor(target);
    OnTermChange(target);
    return true;
  }
}

bool Thesaurus::SetTermFormFor(const EntityUID target, const std::string& termRef, const lang::Morphology& form) {
  if (!Contains(target)) {
    return false;
  } 
  auto& term = storage.at(target).term;
  if (term.IsFormManual(form) && term.GetManualForm(form) == termRef) {
    return false;
  } else {
    term.SetForm(form, termRef);
    OnTermChange(target);
    return true;
  }
}

bool Thesaurus::SetDefinitionFor(const EntityUID target, const std::string& textDef) {
  if (!Contains(target)) {
    return false;
  } else if (storage.at(target).definition.Raw() == textDef) {
    return false;
  } else {
    storage.at(target).definition.InitFrom(textDef, context);
    defGraph.UpdateFor(target);
    return true;
  }
}

void Thesaurus::SubstitueAliases(const StrTranslator& old2New) {
  for (auto& cst : storage) {
    auto& name = cst.second.alias;
    if (const auto newName = old2New(name); newName.has_value()) {
      name = newName.value();
    }
  }
  defGraph.Invalidate();
  termGraph.Invalidate();
  TranslateAll(old2New);
}

void Thesaurus::Translate(const EntityUID target, const StrTranslator& old2New) {
  storage.at(target).Translate(old2New, context);
  defGraph.UpdateFor(target);
  termGraph.UpdateFor(target);
  OnTermChange(target);
}

void Thesaurus::TranslateDef(const EntityUID target, const StrTranslator& old2New) {
  storage.at(target).definition.TranslateRefs(old2New, context);
  defGraph.UpdateFor(target);
}

void Thesaurus::TranslateTerm(const EntityUID target, const StrTranslator& old2New) {
  storage.at(target).term.TranslateRefs(old2New, context);
  termGraph.UpdateFor(target);
  OnTermChange(target);
}

void Thesaurus::TranslateAll(const StrTranslator& old2New) {
  for (const auto& cst : *this) {
    storage.at(cst.uid).Translate(old2New, context);
    defGraph.UpdateFor(cst.uid);
    termGraph.UpdateFor(cst.uid);
  }
  UpdateState();
}

bool Thesaurus::Emplace(const EntityUID newID, std::string name, 
                        lang::LexicalTerm term, lang::ManagedText definition) {
  if (storage.emplace(std::pair{ newID, TextConcept{ newID, std::move(name), std::move(term), std::move(definition) } }).second) {
    defGraph.Invalidate();
    termGraph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

bool Thesaurus::Insert(TextConcept&& cst) {
  if (storage.emplace(std::pair{ cst.uid, std::move(cst) }).second) {
    defGraph.Invalidate();
    termGraph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

bool Thesaurus::InsertCopy(const TextConcept& cst) {
  if (storage.emplace(std::pair{ cst.uid, cst }).second) {
    defGraph.Invalidate();
    termGraph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

void Thesaurus::Load(TextConcept&& cst) {
  storage[cst.uid] = std::move(cst);
  defGraph.Invalidate();
  termGraph.Invalidate();
}

void Thesaurus::Erase(const EntityUID target) {
  if (!Contains(target)) {
    return;
  }
  termGraph.EraseItem(target);
  defGraph.EraseItem(target);
  storage.erase(target);
  UpdateState();
}

void Thesaurus::UpdateState() {
  const auto order = TermGraph().TopologicalOrder();
  for (const auto entity : order) {
    storage.at(entity).term.UpdateFrom(context);
  }
  for (const auto entity : order) {
    storage.at(entity).definition.UpdateFrom(context);
  }
}

void Thesaurus::OnTermChange(const EntityUID target) {
  auto expansion = TermGraph().ExpandOutputs({ target });
  const auto ordered = TermGraph().Sort(expansion);
  for (const auto entity : ordered) {
    storage.at(entity).term.UpdateFrom(Context());
  }
  expansion = DefGraph().ExpandOutputs(expansion);
  for (const auto entity : expansion) {
    storage.at(entity).definition.UpdateFrom(Context());
  }
}

bool Thesaurus::HasHomonyms() const {
  for (auto it = begin(); it != end(); ++it) {
    for (auto it2 = std::next(it); it2 != end(); ++it2) {
      if (!std::empty(it->term.Nominal()) &&
          it->term.Nominal() == it2->term.Nominal()) {
        return true;
      }
    }
  }
  return false;
}

StrSubstitutes Thesaurus::GetHomonyms() const {
  StrSubstitutes result{};
  for (auto it = begin(); it != end(); ++it) {
    for (auto it2 = std::next(it); it2 != end(); ++it2) {
      if (!std::empty(it->term.Nominal()) &&
          it->term.Nominal() == it2->term.Nominal()) {
        if (result.contains(it->alias)) {
          result.emplace(it2->alias, it->alias);
        } else {
          result.emplace(it->alias, it2->alias);
        }
      }
    }
  }
  return result;
}

TextIterator Thesaurus::begin() const noexcept { return TextIterator(std::begin(storage)); }
TextIterator Thesaurus::end() const noexcept { return TextIterator(std::end(storage)); }
size_t Thesaurus::size() const noexcept {  return std::size(storage); }

TextIterator::TextIterator(Container::const_iterator iter) noexcept
  : iterator{ std::move(iter) } {}

TextIterator& TextIterator::operator++() noexcept {
  ++iterator;
  return *this;
}

TextIterator::pointer TextIterator::operator->() const noexcept { return &iterator->second; }
TextIterator::reference TextIterator::operator*() const noexcept { return iterator->second; }

bool TextIterator::operator==(const TextIterator& iter2) const noexcept {
  return iterator == iter2.iterator;
}

bool TextContext::Contains(const std::string& entity) const {
  return thesaurus.FindAlias(entity).has_value();
}

const lang::LexicalTerm* TextContext::At(const std::string& entity) const {
  if (const auto uid = thesaurus.FindAlias(entity); uid.has_value()) {
    return &thesaurus.At(uid.value()).term;
  } else {
    return nullptr;
  }
}

std::unordered_map<std::string, std::string>
TextContext::MatchingTerms(const std::string& input) const {
  std::unordered_map<std::string, std::string> result{};
  for (const auto& cst : thesaurus) {
    if (!empty(cst.term.Text().Raw()) && cst.term.MatchStr(input)) {
      if (!result.contains(cst.alias)) {
        result.emplace(cst.alias, cst.term.Nominal());
      }
    }
  }
  return result;
}

} // namespace ccl::semantic
