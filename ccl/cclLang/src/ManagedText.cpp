#include "ccl/lang/ManagedText.h"

#include "ccl/lang/RefsManager.h"
#include "ccl/lang/EntityTermContext.hpp"
#include "ccl/lang/TextEnvironment.h"

namespace ccl::lang {

bool ManagedText::operator==(const ManagedText& str2) const noexcept { return rawText == str2.rawText; }
bool ManagedText::operator!=(const ManagedText& str2) const noexcept {  return rawText != str2.rawText; }
bool ManagedText::empty() const noexcept { return std::empty(rawText); }
const std::string& ManagedText::Str() const noexcept { return std::empty(cache) ? rawText : cache; }
const std::string& ManagedText::Raw() const noexcept { return rawText; }

void ManagedText::InitFrom(std::string_view ref, const EntityTermContext& cntxt) {
  rawText = ref;
  UpdateFrom(cntxt);
}

void ManagedText::SetRaw(std::string_view ref) {
  rawText = ref;
  cache.clear();
}

void ManagedText::TranslateRefs(const StrTranslator& old2New, const EntityTermContext& cntxt) {
  TranslateRaw(old2New);
  UpdateFrom(cntxt);
}

void ManagedText::TranslateRaw(const StrTranslator& old2New) {
  auto refs = Reference::ExtractAll(rawText);
  for (auto ref = rbegin(refs); ref != rend(refs); ++ref) {
    if (ref->IsEntity() && ref->TranslateEntity(old2New)) {
      const auto start = UTF8Iterator(rawText, ref->position.start).BytePosition();
      const auto oldLength = UTF8Iterator(rawText, ref->position.finish).BytePosition() - start;
      rawText.replace(start, oldLength, ref->ToString());
    }
  }
}

void ManagedText::UpdateFrom(const EntityTermContext& cntxt) {
  if (!TextEnvironment::Instance().skipResolving) {
    cache = lang::RefsManager{ cntxt }.Resolve(rawText);
  }
}

std::unordered_set<std::string> ManagedText::Referals() const {
  const auto refs = Reference::ExtractAll(rawText);
  std::unordered_set<std::string> result{};
  for (const auto& ref : refs) {
    if (ref.IsEntity()) {
      result.emplace(ref.GetEntity());
    }
  }
  return result;
}

} // namespace ccl::lang