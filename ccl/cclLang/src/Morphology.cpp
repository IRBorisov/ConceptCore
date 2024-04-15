#include "ccl/lang/Morphology.h"

#include <iterator>

#include "ccl/Strings.hpp"

namespace ccl::lang {

namespace {

template<typename TagsContainer, typename FilterContainer>
Grammem ExtractSingleIntersection(const TagsContainer& tags, const FilterContainer& filter) {
  std::vector<Grammem> extracted_set{};
  std::ranges::set_intersection(tags, filter, std::back_inserter(extracted_set));
  if (size(extracted_set) != 1U) {
    return Grammem::invalid;
  }
  else {
    return extracted_set.at(0);
  }
}

} // namespace

std::string Morphology::ToString() const noexcept {
  if (empty()) {
    return {};
  }
  return std::accumulate(next(begin(tags)), end(tags), std::string{ Grammem2Str(*begin(tags))},
    [](std::string result, Grammem arg) {
      result += ",";
      result += Grammem2Str(arg);
      return result;
    }
  );
}

Morphology::Morphology(std::string_view tagsText) 
  : Morphology(SplitBySymbol(tagsText, ',')) {}

Morphology::Morphology(const std::vector<std::string_view>& tags_) {
  for (const auto tag : tags_) {
    const auto gram = Str2Grammem(TrimWhitespace(tag));
    if (gram != Grammem::invalid) {
      tags.insert(gram);
    }
  }
}

bool Morphology::operator==(const Morphology& rval) const noexcept { return tags == rval.tags; }
bool Morphology::operator!=(const Morphology& rval) const noexcept { return !(*this == rval); }
bool Morphology::Contains(Grammem gram) const noexcept { return tags.contains(gram); }
size_t Morphology::size() const noexcept { return std::size(tags); }
bool Morphology::empty() const noexcept { return std::empty(tags); }
Grammem Morphology::GetPOS() const { return ExtractSingleIntersection(tags, ALL_POS); }
Grammem Morphology::GetCase() const {  return ExtractSingleIntersection(tags, ALL_CASES); }
Grammem Morphology::GetNumber() const {  return ExtractSingleIntersection(tags, ALL_NUMBERS); }
Grammem Morphology::GetGender() const {  return ExtractSingleIntersection(tags, ALL_GENDERS); }
Grammem Morphology::GetTense() const { return ExtractSingleIntersection(tags, ALL_TENSES); }
Grammem Morphology::GetPerson() const {  return ExtractSingleIntersection(tags, ALL_PERSONS); }

} // namespace ccl::lang