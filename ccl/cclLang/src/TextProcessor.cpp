#include "ccl/lang/TextProcessor.h"

#include <regex>

namespace ccl::lang {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

std::string TextProcessor::Inflect(const std::string& target, const Morphology& /*morpho*/) const {
  return target;
}

std::string TextProcessor::InflectDependant(const std::string& dependant, const std::string& /*main*/) const {
  return dependant;
}

bool TextProcessor::IsSubstr(const std::string& needle, const std::string& haystack) const {
  if (needle == haystack) {
    return true;
  } else if (empty(needle)) {
    return true;
  } else if (BasicSTDMatch(needle, haystack)) {
    return true;
  } else {
    return RegexMatch(needle, haystack);
  }
}

bool TextProcessor::BasicSTDMatch(const std::string& needle, const std::string& haystack) {
  const auto it = std::search(
    begin(haystack), end(haystack),
    begin(needle), end(needle),
    [](char ch1, char ch2) noexcept { return ::toupper(ch1) == ::toupper(ch2); }
  );
  return (it != end(haystack));
}

bool TextProcessor::RegexMatch(const std::string& strRegex, const std::string& strHaystack) {
  try {
    const std::regex testRegex(std::data(strRegex), std::size(strRegex));
    return std::sregex_iterator(begin(strHaystack), end(strHaystack), testRegex) != std::sregex_iterator();
  }
  catch (const std::regex_error&) {
    return false;
  }
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl::lang