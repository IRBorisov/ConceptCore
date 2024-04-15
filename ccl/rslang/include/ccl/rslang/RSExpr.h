#pragma once

#include "ccl/cclMeta.hpp"
#include "ccl/Strings.hpp"
#include "ccl/Substitutes.hpp"
#include "ccl/rslang/RSToken.h"

#include <vector>
#include <unordered_set>
#include <optional>

namespace ccl::rslang {

class SyntaxTree;

[[nodiscard]] meta::UniqueCPPtr<SyntaxTree> GenerateAST(const std::string& str);
[[nodiscard]] std::unordered_set<std::string> ExtractUGlobals(const std::string& str);
[[nodiscard]] std::unordered_set<std::string> ExtractULocals(const std::string& str);

using TokenFilter = std::function<bool(const TokenID&)>;

int32_t TranslateRS(std::string& str, TokenFilter filter, StrTranslator old2New);
int32_t SubstituteGlobals(std::string& str, const StrSubstitutes& substitutes);

struct TFFactory {
  [[nodiscard]] static StrTranslator GetTransition(const StrSubstitutes& substitutes);

  [[nodiscard]] static TokenFilter GetFilter(const std::vector<rslang::TokenID>& matchTokens);
  [[nodiscard]] static const TokenFilter& FilterIdentifiers();
  [[nodiscard]] static const TokenFilter& FilterGlobals();
};

} // namespace ccl::rslang