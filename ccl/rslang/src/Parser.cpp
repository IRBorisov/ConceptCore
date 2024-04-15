#pragma once

#include "ccl/rslang/Parser.h"

namespace ccl::rslang {

Syntax Parser::EstimateSyntax(std::string_view expression) noexcept {
  using Syntax::UNDEF;
  using Syntax::MATH;
  using Syntax::ASCII;
  static constexpr std::array<Syntax, 256> symbolHints = {
  // 0.8     1.9     2.A     3.B     4.C     5.D     6.E     7.F
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x00
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x08
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x10
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x18
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  MATH,    UNDEF,  // 0x20 &
    UNDEF,  UNDEF,  UNDEF,  MATH,    UNDEF,  MATH,    UNDEF,  UNDEF,  // 0x28 + -
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x30
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  MATH,    MATH,    MATH,    UNDEF,  // 0x38 < = >
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x40
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x48
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x50
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x58
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x60
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x68
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x70
    UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  UNDEF,  // 0x78
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0x80 UNICODE
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0x88
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0x90
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0x98
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xA0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xA8
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xB0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xB8
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xC0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xC8
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xD0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xD8
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xE0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xE8
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    // 0xF0
    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH,    MATH    // 0xF8
  };
  for (const auto& symbol: expression) {
    if (symbolHints.at(static_cast<unsigned char>(symbol)) == MATH) {
      return Syntax::MATH;
    }
  }
  return Syntax::ASCII;
}

TokenStream Parser::Lex(const std::string& expression, const Syntax syntaxHint) {
  if (syntaxHint != Syntax::UNDEF) {
    syntax = syntaxHint;
  } else {
    syntax = EstimateSyntax(expression);
  }
  if (syntax == Syntax::MATH) {
    return math(expression).Stream();
  } else {
    return ascii(expression).Stream();
  }
}

bool Parser::Parse(const std::string& expr, const Syntax syntaxHint) {
  log.Clear();
  return parser.Parse(Lex(expr, syntaxHint));
}

const ErrorLogger& Parser::Errors() const noexcept {
  return log;
}
const SyntaxTree& Parser::AST() const noexcept {
  return parser.AST();
}

meta::UniqueCPPtr<SyntaxTree> Parser::ExtractAST() noexcept {
  return parser.ExtractAST();
}

} // namespace ccl::rslang