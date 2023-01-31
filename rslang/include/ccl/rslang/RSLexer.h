#pragma once

#include "ccl/rslang/LexerBase.hpp"

#include <string>
#include <memory>

namespace ccl::rslang {

namespace rslex { class RSLexerImpl; }

//! Lexer for RS
class RSLexer : public LexerBase<RSLexer> {
	friend class LexerBase<RSLexer>;

	std::unique_ptr<rslex::RSLexerImpl> impl;

public:
	~RSLexer();
	RSLexer();

	RSLexer(const RSLexer&) = delete;
	RSLexer& operator=(const RSLexer&) = delete;
	RSLexer(RSLexer&&) noexcept = default;
	RSLexer& operator=(RSLexer&&) noexcept = default;

	explicit RSLexer(ErrorReporter reporter);
	explicit RSLexer(const std::string& input);

public:
	RSLexer& operator()(const std::string& input);
	void SetInput(const std::string& input);

	[[nodiscard]] StrRange Range() const;
	[[nodiscard]] StrRange RangeInBytes() const;

private:
	[[nodiscard]] std::string GetText() const;
	[[nodiscard]] TokenID DoLex();
};

} // namespace ccl::rslang