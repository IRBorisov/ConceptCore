#include "ccl/semantic/RSConcept.h"

#include "ccl/lang/TextEnvironment.h"
#include "ccl/rslang/RSExpr.h"

namespace ccl::semantic {

bool RSConcept::IsEmpty() const noexcept {
  return empty(definition) && empty(convention);
}

void RSConcept::Translate(const StrTranslator& old2New) {
  rslang::TranslateRS(definition, rslang::TFFactory::FilterGlobals(), old2New);
  rslang::TranslateRS(convention, rslang::TFFactory::FilterGlobals(), old2New);
}

bool RSConcept::operator==(const RSConcept& rhs) const noexcept {
  return &rhs == this ||
    (type == rhs.type && definition == rhs.definition && convention == rhs.convention);
}

bool RSConcept::MatchStr(const std::string& searchStr) const {
  return lang::TextEnvironment::Processor().IsSubstr(searchStr, convention);
}

} // namespace ccl::semantic
