#pragma once

#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/ErrorLogger.h"
#include "ccl/rslang/MathLexer.h"
#include "ccl/rslang/AsciiLexer.h"

namespace ccl::rslang {

//! Parser for RS expressions
class Parser {
public:
  ErrorLogger log{};
  Syntax syntax{ Syntax::UNDEF };

private:
  detail::AsciiLexer ascii{log.SendReporter()};
  detail::MathLexer math{ log.SendReporter() };
  detail::RSParser parser{ log.SendReporter() };

public:
  static Syntax EstimateSyntax(std::string_view expression) noexcept;
  
  TokenStream Lex(const std::string& expression, Syntax syntaxHint = Syntax::UNDEF);
  bool Parse(const std::string& expr, Syntax syntaxHint = Syntax::UNDEF);
  [[nodiscard]] const ErrorLogger& Errors() const noexcept;
  [[nodiscard]] const SyntaxTree& AST() const noexcept;
  [[nodiscard]] meta::UniqueCPPtr<SyntaxTree> ExtractAST() noexcept;
};

} // namespace ccl::rslang