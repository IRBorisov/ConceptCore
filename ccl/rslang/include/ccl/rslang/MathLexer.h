#pragma once

#include "ccl/rslang/LexerBase.hpp"

#include <string>
#include <memory>

namespace ccl::rslang::detail {

namespace rslex { class MathLexerImpl; }

//! Lexer for RS
class MathLexer : public LexerBase<MathLexer> {
  friend class LexerBase<MathLexer>;

  std::unique_ptr<rslex::MathLexerImpl> impl;

public:
  ~MathLexer();
  MathLexer();

  MathLexer(const MathLexer&) = delete;
  MathLexer& operator=(const MathLexer&) = delete;
  MathLexer(MathLexer&&) noexcept = default;
  MathLexer& operator=(MathLexer&&) noexcept = default;

  explicit MathLexer(ErrorReporter reporter);
  explicit MathLexer(const std::string& input);

public:
  MathLexer& operator()(const std::string& input);
  void SetInput(const std::string& input);

  [[nodiscard]] StrRange Range() const;
  [[nodiscard]] StrRange RangeInBytes() const;

private:
  [[nodiscard]] std::string GetText() const;
  [[nodiscard]] TokenID DoLex();
};

} // namespace ccl::rslang::detail