#pragma once

#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/ErrorLogger.h"
#include "ccl/rslang/RSLexer.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ValueAuditor.h"

#include <memory>

namespace ccl::rslang {

//! Auditor for RS expressions
template<typename Lexer = RSLexer>
class Auditor {
public:
	bool isParsed{ false };
	bool isTypeCorrect{ false };
	bool isValueCorrect{ false };

private:
	ErrorLogger log{};
	Lexer lexer{ log.SendReporter() };
	RSParser parser{ log.SendReporter() };
	TypeAuditor typeAuditor;
	ValueAuditor valueAuditor;

public:
	explicit Auditor(const TypeContext& types,
									 ValueClassContext globalClass,
									 SyntaxTreeContext globalAST)
		: typeAuditor{ types, log.SendReporter() },
			valueAuditor{ std::move(globalClass), std::move(globalAST), log.SendReporter() } {}

public:
	bool CheckType(const std::string& expr) {
		log.Clear();
		isValueCorrect = false;
		lexer.SetInput(expr);

		isParsed = parser.Parse(lexer.Stream());
		isTypeCorrect = isParsed && typeAuditor.CheckType(parser.AST());
		return isTypeCorrect;
	}
	bool CheckValue() {
		isValueCorrect = isParsed && valueAuditor.Check(parser.AST());
		return isValueCorrect;
	}

	[[nodiscard]] const ErrorLogger& Errors() const noexcept {
		return log;
	}
	[[nodiscard]] const SyntaxTree& AST() const noexcept {
		return parser.AST();
	}
	[[nodiscard]] meta::UniqueCPPtr<SyntaxTree> ExtractAST() noexcept {
		return parser.ExtractAST();
	}
	[[nodiscard]] const FunctionArguments& GetDeclarationArgs() const noexcept {
		return typeAuditor.GetDeclarationArgs();
	}
	[[nodiscard]] const ExpressionType& GetType() const noexcept {
		return typeAuditor.GetType();
	}
	[[nodiscard]] ValueClass GetValueClass() const noexcept {
		if (isValueCorrect) {
			return valueAuditor.VType();
		} else {
			return ValueClass::invalid;
		}
	}
};

} // namespace ccl::rslang