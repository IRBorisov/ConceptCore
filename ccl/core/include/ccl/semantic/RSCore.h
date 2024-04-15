#pragma once

#include "ccl/cclMeta.hpp"

#include "ccl/semantic/ConceptRecord.h"
#include "ccl/semantic/Schema.h"
#include "ccl/semantic/Thesaurus.h"
#include "ccl/semantic/CstList.h"
#include "ccl/semantic/IdentityManager.h"

namespace ccl::semantic {

class RSFormIterator;

//! Core of system of concepts
class RSCore {
  friend class RSFormIterator;

private:
  IdentityManager identifiers{};
  Thesaurus thesaurus{};
  Schema schema{};
  CstList cstList{ [&core = *this] (const EntityUID uid) { return core.RSLang().At(uid).type; } };

public:
  [[nodiscard]] RSFormIterator begin() const noexcept;
  [[nodiscard]] RSFormIterator end() const noexcept;
  [[nodiscard]] size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }

  [[nodiscard]] bool Contains(EntityUID entity) const;
  [[nodiscard]] std::optional<EntityUID> FindAlias(const std::string& name) const;

  [[nodiscard]] const CstList& List() const noexcept;
  [[nodiscard]] const Thesaurus& Texts() const noexcept;
  [[nodiscard]] const Schema& RSLang() const noexcept;

  //! requires Contains(entity) == true
  [[nodiscard]] const RSConcept& GetRS(EntityUID entity) const;
  [[nodiscard]] const TextConcept& GetText(EntityUID entity) const;
  [[nodiscard]] const ParsingInfo& GetParse(EntityUID entity) const;
  [[nodiscard]] ConceptRecord AsRecord(EntityUID entity) const;

  EntityUID Emplace(CstType type, const std::string& definition = {});
  EntityUID Insert(ConceptRecord&& cst);
  EntityUID InsertCopy(EntityUID target, const RSCore& source);
  EntityUID InsertCopy(const ConceptRecord& cst);
  VectorOfEntities InsertCopy(const VectorOfEntities& input, const RSCore& source);
  VectorOfEntities InsertCopy(const std::vector<ConceptRecord>& input);

  bool MoveBefore(EntityUID what, ListIterator iWhere);

  bool Erase(EntityUID target);

  bool SetAliasFor(EntityUID target, const std::string& newValue, bool substitueMentions = true);
  bool SetExpressionFor(EntityUID target, const std::string& expression);
  bool SetConventionFor(EntityUID target, const std::string& convention);

  bool SetTermFor(EntityUID target, const std::string& termRef);
  bool SetTermFormFor(EntityUID target, const std::string& termRef, const lang::Morphology& form);
  bool SetDefinitionFor(EntityUID target, const std::string& textDef);

  void Translate(EntityUID target, const StrTranslator& old2New);
  void TranslateAll(const StrTranslator& old2New);

  void TranslateTexts(EntityUID target, const StrTranslator& old2New);
  void TranslateDef(EntityUID target, const StrTranslator& old2New);
  void TranslateTerm(EntityUID target, const StrTranslator& old2New);

  void ResetAliases();

  EntityUID Load(ConceptRecord&& cst);
  void UpdateState();
};

//! RS iterator
class RSFormIterator {
  friend class RSCore;

  RSIterator iter;

private:
  explicit RSFormIterator(RSIterator iter) noexcept;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = EntityUID;
  using difference_type = std::ptrdiff_t;
  using pointer = const EntityUID*;
  using reference = const EntityUID&;

  [[nodiscard]] bool operator==(const RSFormIterator& iter2) const noexcept;
  [[nodiscard]] bool operator!=(const RSFormIterator& iter2) const noexcept { return !(*this == iter2); }

  RSFormIterator& operator++() noexcept;

  [[nodiscard]] pointer operator->() const noexcept;
  [[nodiscard]] reference operator*() const noexcept;
};

} // namespace ccl::semantic
