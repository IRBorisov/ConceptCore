#pragma once

#include "ParserState.hpp"

namespace ccl::rslang::detail {

class RSParserImpl;

//! Parser for RS
class RSParser {
  ParserState state;
  std::unique_ptr<RSParserImpl> impl;

public:
  ~RSParser();
  RSParser(const RSParser&) = delete;
  RSParser& operator=(const RSParser&) = delete;
  RSParser(RSParser&& rhs) noexcept;
  RSParser& operator=(RSParser&& rhs) noexcept;

  explicit RSParser(std::optional<ErrorReporter> reporter = std::nullopt);

public:
  [[nodiscard]] bool Parse(TokenStream input);

  [[nodiscard]] meta::UniqueCPPtr<SyntaxTree> ExtractAST() noexcept {
    return std::move(state.parsedTree);
  }

  // Note: only available if Parse returns true and AST is not stolen!
  [[nodiscard]] const SyntaxTree& AST() const noexcept {
    return *state.parsedTree;
  }
};

} // namespace ccl::rslang::detail