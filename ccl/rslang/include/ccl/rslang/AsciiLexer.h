#pragma once

#include "ccl/rslang/LexerBase.hpp"

#include <string>
#include <optional>

namespace ccl::rslang::detail {

namespace asciilex { class AsciiLexerImpl; }

//! Lexer for ASCII syntax of RSLanguage
class AsciiLexer : public LexerBase<AsciiLexer> {
  friend class LexerBase<AsciiLexer>;

  std::unique_ptr<asciilex::AsciiLexerImpl> impl;

public:
  ~AsciiLexer();
  AsciiLexer();

  AsciiLexer(const AsciiLexer&) = delete;
  AsciiLexer& operator=(const AsciiLexer&) = delete;
  AsciiLexer(AsciiLexer&&) noexcept = default;
  AsciiLexer& operator=(AsciiLexer&&) noexcept = default;

  explicit AsciiLexer(ErrorReporter reporter);
  explicit AsciiLexer(const std::string& input);

public:
  AsciiLexer& operator()(const std::string& input);
  void SetInput(const std::string& input);

  [[nodiscard]] StrRange Range() const;
  [[nodiscard]] StrRange RangeInBytes() const;

private:
  [[nodiscard]] std::string GetText() const;
  [[nodiscard]] TokenID DoLex();
};

} // namespace ccl::rslang::detail