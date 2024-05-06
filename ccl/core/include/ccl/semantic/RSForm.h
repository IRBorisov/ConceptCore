#pragma once

#include "ccl/Source.hpp"
#include "ccl/cclChange.hpp"

#include "ccl/semantic/RSCore.h"
#include "ccl/semantic/rsModificationFacet.h"
#include "ccl/semantic/rsOperationFacet.h"

namespace ccl::ops { class RSAggregator; } // namespace ccl::ops

namespace ccl::semantic {

//! RSForm of system of concepts
class RSForm : public src::DataStream, public change::ObservableMods {
  friend class ops::RSEquationProcessor;
  friend class ops::RSAggregator;

  friend class rsOperationFacet;
  friend class rsModificationFacet;
  
public:
  std::string title{};
  std::string alias{};
  std::string comment{};

private:
  RSCore core{};
  std::unique_ptr<rsModificationFacet> mods{ std::make_unique<rsModificationFacet>(*this) };
  std::unique_ptr<rsOperationFacet> operations{ std::make_unique<rsOperationFacet>(*this) };

public:
  ~RSForm() noexcept override = default;
  RSForm() = default;
  RSForm(const RSForm& rhs);
  RSForm& operator=(const RSForm& rhs);

public:
  [[nodiscard]] src::DataType Type() const noexcept override { return src::DataType::rsSchema; }

  [[nodiscard]] const RSCore& Core() const noexcept;
  [[nodiscard]] const CstList& List() const noexcept;
  [[nodiscard]] const Thesaurus& Texts() const noexcept;
  [[nodiscard]] const Schema& RSLang() const noexcept;

  [[nodiscard]] rsModificationFacet& Mods() noexcept;
  [[nodiscard]] const rsModificationFacet& Mods() const noexcept;
  [[nodiscard]] rsOperationFacet& Ops() noexcept;
  [[nodiscard]] const rsOperationFacet& Ops() const noexcept;

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

  bool SetAliasFor(EntityUID target, const std::string& newValue, bool substitue = true);
  bool SetExpressionFor(EntityUID target, const std::string& expression);
  bool SetTermFor(EntityUID target, const std::string& termRef);
  bool SetTermFormFor(EntityUID target, const std::string& termRef, const lang::Morphology& form);
  bool SetDefinitionFor(EntityUID target, const std::string& textDef);
  bool SetConventionFor(EntityUID target, const std::string& convention);

  void ResetAliases();

  EntityUID Load(ConceptRecord&& cst);
  void UpdateState();
  [[nodiscard]] RSCore& LoadCore() noexcept { return core; }
  
  // Note: hashes ignore ordering
  [[nodiscard]] change::Hash CoreHash() const;
  [[nodiscard]] change::Hash FullHash() const;

private:
  [[nodiscard]] bool NotifyAndReturn(const bool result) {
    if (result) {
      NotifyModification();
    }
    return result;
  }
  [[nodiscard]] bool EraseInternal(EntityUID target);
  EntityTranslation DeleteDuplicatesInternal();
  void EquateTextsOf(EntityUID delID, EntityUID hierID, const ops::Equation& params);
};

} // namespace ccl::semantic
