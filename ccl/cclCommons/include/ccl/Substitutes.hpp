#pragma once

#include <optional>
#include <functional>
#include <string>

namespace ccl {

using StrSubstitutes = std::unordered_map<std::string, std::string>;
using StrTranslator = std::function<std::optional<std::string>(const std::string&)>;

inline StrTranslator CreateTranslator(const StrSubstitutes& substitutes) {
  return [&substitutes](const std::string& oldVal) -> StrTranslator::result_type {
    if (!substitutes.contains(oldVal)) {
      return std::nullopt;
    } else {
      return substitutes.at(oldVal);
    }
  };
}

inline StrTranslator CreateTranslator(StrSubstitutes&& substitutes) {
  return [substitutes = std::move(substitutes)](const std::string& oldVal) -> StrTranslator::result_type {
    if (!substitutes.contains(oldVal)) {
      return std::nullopt;
    } else {
      return substitutes.at(oldVal);
    }
  };
}

} // namespace ccl