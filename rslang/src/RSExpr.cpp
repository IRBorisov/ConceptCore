#include "ccl/rslang/RSExpr.h"

#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/RSLexer.h"

#include <vector>
#include <unordered_set>

namespace ccl::rslang {

meta::UniqueCPPtr<SyntaxTree> GenerateAST(const std::string& str) {
	RSParser parser{};
	if (!parser.Parse(RSLexer{ str }.Stream())) {
		return nullptr;
	} else {
		return parser.ExtractAST();
	}
}

[[nodiscard]] std::unordered_set<std::string> ExtractUGlobals(const std::string& str) {
	// TODO: figure out why it doesnt work for MFC build but works in google test
	/*static const reflex::Pattern pattern("\\<[XCSFPDAT][0-9]+\\>");
	reflex::Matcher matcher(pattern, str);
	std::unordered_set<std::string> result{};
	while (matcher.find()) {
		result.emplace(matcher.text());
	}*/
	std::unordered_set<std::string> result{};
	RSLexer lex{ str };
	for (auto token = lex.lex(); token != TokenID::END; token = lex.lex()) {
		if (TFFactory::FilterGlobals()(token)) {
			result.emplace(lex.Text());
		}
	}
	return result;
}

std::unordered_set<std::string> ExtractULocals(const std::string &str) {
	std::unordered_set<std::string> result{};
	RSLexer lex{ str };
	for (auto token = lex.lex(); token != TokenID::END; token = lex.lex()) {
		if (token == TokenID::ID_LOCAL) {
			result.emplace(lex.Text());
		}
	}
	return result;
}

int32_t SubstituteGlobals(std::string& str, const StrSubstitutes& substitutes) {
	return TranslateRS(str, TFFactory::FilterGlobals(), CreateTranslator(substitutes));
}

int32_t TranslateRS(std::string& str, TokenFilter filter, StrTranslator old2New) {
	int32_t count = 0;
	const std::string copy = str;
	rslang::RSLexer lex{ copy };
	int32_t offset = 0;
	for (auto token = lex.lex(); token != TokenID::END; token = lex.lex()) {
		if (filter(token)) {
			const auto& name = lex.Text();
			const auto newName = old2New(name);
			if (newName.has_value() && newName.value() != name) {
				str.replace(static_cast<size_t>(lex.RangeInBytes().start + offset), name.length(), newName.value()); // NOLINT
				offset += static_cast<int32_t>(ssize(newName.value()) - ssize(name));
				++count;
			}
		}
	}
	return count;
}

StrTranslator TFFactory::GetTransition(const StrSubstitutes& substitutes) {
	static const std::string_view errorSuffix = R"(_ERROR)";
	return [&](const std::string& oldVal) -> StrTranslator::result_type { // Note: saving reference!
		if (!substitutes.contains(oldVal)) {
			if (size(oldVal) <= size(errorSuffix)) {
				std::string result = oldVal;
				result += errorSuffix;
				return result;
			} else {
				return oldVal;
			}
		} else {
			return substitutes.at(oldVal);
		}
	};
}

TokenFilter TFFactory::GetFilter(const std::vector<rslang::TokenID>& matchTokens) {
	return [=](const rslang::TokenID& token) noexcept { // NOLINT (bugprone-exception-escape)
		return std::find(begin(matchTokens), end(matchTokens), token) != end(matchTokens);
	};
}

const TokenFilter& TFFactory::FilterGlobals() {
	static auto filter = GetFilter({
		rslang::TokenID::ID_GLOBAL,
		rslang::TokenID::ID_FUNCTION,
		rslang::TokenID::ID_PREDICATE
	});
	return filter;
}

const TokenFilter& TFFactory::FilterIdentifiers() {
	static auto filter = GetFilter({
		rslang::TokenID::ID_GLOBAL,
		rslang::TokenID::ID_FUNCTION,
		rslang::TokenID::ID_PREDICATE,
		rslang::TokenID::ID_LOCAL
																 });
	return filter;
}

} // namespace ccl::rslang