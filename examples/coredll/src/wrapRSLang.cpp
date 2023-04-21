#include "pch.h"

#include "wrapRSLang.h"

#include "ccl\rslang\RSGenerator.h"
#include "ccl\rslang\Parser.h"
#include "ccl\rslang\RSExpr.h"

namespace core {

std::string ToASCII(const std::string& input) {
	return ccl::rslang::ConvertTo(input, ccl::rslang::Syntax::ASCII);
}

std::string ToMath(const std::string& input) {
	return ccl::rslang::ConvertTo(input, ccl::rslang::Syntax::MATH);
}

std::string GetASTString(const std::string& input) {
	ccl::rslang::Parser parser{};
	if (!parser.Parse(input)) {
		return "INVALID_INPUT";
	} 
	auto ast = parser.ExtractAST();
	return ccl::rslang::AST2String::Apply(*ast);
}
	
} // namespace core