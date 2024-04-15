#pragma once

#include "ccl/lang/Morphology.h"

#include <optional>

namespace ccl::lang {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

//! Abstract text processor
class TextProcessor {
public:
  virtual ~TextProcessor() noexcept = default;
  TextProcessor() noexcept = default;

protected:
  TextProcessor(const TextProcessor&) = default;
  TextProcessor& operator=(const TextProcessor&) = default;
  TextProcessor(TextProcessor&&) noexcept = default;
  TextProcessor& operator=(TextProcessor&&) noexcept = default;

public:
  [[nodiscard]] virtual std::string Inflect(const std::string& target, const Morphology& form) const;
  [[nodiscard]] virtual std::string InflectDependant(const std::string& dependant, const std::string& main) const;
  [[nodiscard]] virtual bool IsSubstr(const std::string& needle, const std::string& haystack) const;

private:
  [[nodiscard]] static bool BasicSTDMatch(const std::string& needle, const std::string& haystack);
  [[nodiscard]] static bool RegexMatch(const std::string& strRegex, const std::string& strHaystack);
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl::lang