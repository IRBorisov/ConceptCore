#include "ccl/semantic/TextConcept.h"

#include "ccl/lang/TextEnvironment.h"

namespace ccl::semantic {

bool TextConcept::IsEmpty() const noexcept {
  return empty(definition.Raw()) && empty(term.Nominal());
}

void TextConcept::TranslateRaw(const StrTranslator& old2New) {
  term.TranslateRaw(old2New);
  definition.TranslateRaw(old2New);
}

void TextConcept::Translate(const StrTranslator& old2New, const lang::EntityTermContext& cntxt) {
  term.TranslateRefs(old2New, cntxt);
  definition.TranslateRefs(old2New, cntxt);
}

bool TextConcept::operator==(const TextConcept& rhs) const noexcept {
  return &rhs == this || (term == rhs.term && definition == rhs.definition);
}

bool TextConcept::MatchStr(const std::string& searchStr) const {
  return term.MatchStr(searchStr) ||
    lang::TextEnvironment::Processor().IsSubstr(searchStr, definition.Str());
}

} // namespace ccl::semantic
