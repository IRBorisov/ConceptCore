#pragma once

#include "ccl/semantic/RSConcept.h"
#include "ccl/semantic/TextConcept.h"

namespace ccl::semantic {

//! Constituent of RSCore
class ConceptRecord {
public:
  EntityUID uid{};
  std::string alias{};

  CstType type{ CstType::base };

  std::string rs{};
  std::string convention{};

  lang::LexicalTerm term{};
  lang::ManagedText definition{};

public:
  explicit ConceptRecord() = default;
  explicit ConceptRecord(const RSConcept& rsPart, const TextConcept& textPart);

public:
  RSConcept SpawnRS() const;
  TextConcept SpawnText() const;
};

} // namespace ccl::semantic
