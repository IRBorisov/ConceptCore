#pragma once

#include "ccl/semantic/TextConcept.h"
#include "ccl/lang/EntityTermContext.hpp"
#include "ccl/lang/RefsManager.h"
#include "ccl/graph/CGraph.h"

#include <map>

namespace ccl::semantic {

class Thesaurus;

//! RSCore term context
class TextContext : public lang::EntityTermContext {
public:
  explicit TextContext(const Thesaurus& thesaurus) noexcept
    : thesaurus{ thesaurus } {}

private:
  const Thesaurus& thesaurus;

public:
  [[nodiscard]] bool Contains(const std::string& entity) const override;
  [[nodiscard]] const lang::LexicalTerm* At(const std::string& entity) const override;
  [[nodiscard]] std::unordered_map<std::string, std::string>
    MatchingTerms(const std::string& input) const override;
};

class TextIterator;

//! System of text definition for concepts
class Thesaurus {
  friend class TextIterator;

  // Note: using map for consistent iteration order
  using Storage = std::map<EntityUID, TextConcept>;
  Storage storage{};

  TextContext context;
  mutable graph::UpdatableGraph termGraph;
  mutable graph::UpdatableGraph defGraph;

public:
  ~Thesaurus() noexcept = default;
  explicit Thesaurus();

  Thesaurus(const Thesaurus& rhs);
  Thesaurus& operator=(const Thesaurus& rhs);
  Thesaurus(Thesaurus&& rhs) noexcept;
  Thesaurus& operator=(Thesaurus&& rhs) noexcept;

public:
  [[nodiscard]] TextIterator begin() const noexcept;
  [[nodiscard]] TextIterator end() const noexcept;
  [[nodiscard]] size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }

  [[nodiscard]] bool Contains(EntityUID entity) const;
  [[nodiscard]] std::optional<EntityUID> FindAlias(const std::string& alias) const;

  //! requires Contains(entity) == true
  [[nodiscard]] const TextConcept& At(EntityUID entity) const;

  [[nodiscard]] const lang::EntityTermContext& Context() const noexcept;
  [[nodiscard]] const graph::CGraph& TermGraph() const;
  [[nodiscard]] const graph::CGraph& DefGraph() const;

  bool Emplace(EntityUID newID, std::string name,
               lang::LexicalTerm term = {}, lang::ManagedText definition = {});
  bool Insert(TextConcept&& cst);
  bool InsertCopy(const TextConcept& cst);
  void Load(TextConcept&& cst);
  void Erase(EntityUID target);

  bool SetAliasFor(EntityUID target, const std::string& newValue, bool substitueMentions = true);
  bool SetTermFor(EntityUID target, const std::string& termRef);
  bool SetTermFormFor(EntityUID target, const std::string& termRef, const lang::Morphology& form);
  bool SetDefinitionFor(EntityUID target, const std::string& textDef);

  void SubstitueAliases(const StrTranslator& old2New);
  void Translate(EntityUID target, const StrTranslator& old2New);
  void TranslateTerm(EntityUID target, const StrTranslator& old2New);
  void TranslateDef(EntityUID target, const StrTranslator& old2New);
  void TranslateAll(const StrTranslator& old2New);

  [[nodiscard]] bool HasHomonyms() const;
  [[nodiscard]] StrSubstitutes GetHomonyms() const;

  void UpdateState();

private:
  void OnTermChange(EntityUID target);
};

//! Text concept iterator
class TextIterator {
  friend class Thesaurus;
  using Container = Thesaurus::Storage;
  Container::const_iterator iterator;

private:
  explicit TextIterator(Container::const_iterator iter) noexcept;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = TextConcept;
  using difference_type = std::ptrdiff_t;
  using pointer = const TextConcept*;
  using reference = const TextConcept&;

  [[nodiscard]] bool operator==(const TextIterator& iter2) const noexcept;
  [[nodiscard]] bool operator!=(const TextIterator& iter2) const noexcept { return !(*this == iter2); }

  TextIterator& operator++() noexcept;

  [[nodiscard]] pointer operator->() const noexcept;
  [[nodiscard]] reference operator*() const noexcept;
};

} // namespace ccl::semantic
