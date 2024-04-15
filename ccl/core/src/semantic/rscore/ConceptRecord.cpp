#include "ccl/semantic/ConceptRecord.h"

namespace ccl::semantic {

ConceptRecord::ConceptRecord(const RSConcept& rsPart, const TextConcept& textPart) 
  : uid{ rsPart.uid }, alias{ rsPart.alias }, type{ rsPart.type },
  rs{ rsPart.definition }, convention{ rsPart.convention }, term{ textPart.term }, definition{ textPart.definition } {}

RSConcept ConceptRecord::SpawnRS() const {
  return RSConcept(uid, alias, type, rs, convention);
}

TextConcept ConceptRecord::SpawnText() const {
  return TextConcept(uid, alias, term, definition);
}

} // namespace ccl::semantic
