#include "ccl/lang/LexicalTerm.h"

#include "ccl/lang/EntityTermContext.hpp"
#include "ccl/lang/TextEnvironment.h"

namespace ccl::lang {

void LexicalTerm::SetText(std::string_view ref, const EntityTermContext& cntxt) {
  if (ref != text.Raw()) {
    text.InitFrom(ref, cntxt);
    ClearForms();
  }
}

void LexicalTerm::TranslateRefs(const StrTranslator& old2New, const EntityTermContext& cntxt) {
  text.TranslateRefs(old2New, cntxt);
  cachedForms.clear();
}

void LexicalTerm::TranslateRaw(const StrTranslator& old2New) {
  text.TranslateRaw(old2New);
  cachedForms.clear();
}

void LexicalTerm::SetForm(const Morphology& form, std::string_view formText) {
  manualForms[form] = formText;
  cachedForms.erase(form);
}

bool LexicalTerm::operator==(const LexicalTerm& t2) const noexcept {
  return text == t2.text;
}

void LexicalTerm::UpdateFrom(const EntityTermContext& cntxt) {
  text.UpdateFrom(cntxt);
  cachedForms.clear();
}

bool LexicalTerm::MatchStr(const std::string& str) const {
  return TextEnvironment::Processor().IsSubstr(str, GetNominalForm());
}

void LexicalTerm::ClearForms() noexcept {
  manualForms.clear();
  cachedForms.clear();
}

bool LexicalTerm::IsFormManual(const Morphology& form) const {
  return manualForms.contains(form);
}

const std::string& LexicalTerm::GetManualForm(const Morphology& form) const {
  return manualForms.at(form);
}

const std::string& LexicalTerm::GetNominalForm() const {
  return GetForm(Morphology{ Grammem::sing, Grammem::nomn });
}

const std::unordered_map<Morphology, std::string>& LexicalTerm::GetAllManual() const {
  return manualForms;
}

const std::string& LexicalTerm::GetForm(const Morphology& form) const {
  if (manualForms.contains(form)) {
    return manualForms.at(form);
  } else if (cachedForms.contains(form)) {
    return cachedForms.at(form);
  } else if (std::empty(form)) {
    return GetNominalForm();
  }
  cachedForms.emplace(form, TextEnvironment::Processor().Inflect(text.Str(), form));
  return cachedForms.at(form);
}

} // namespace ccl::lang