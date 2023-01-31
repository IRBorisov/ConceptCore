#pragma once

#include "ccl/rslang/DataContext.hpp"
#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/RSParser.h"
#include "ccl/rslang/RSGenerator.h"
#include "ccl/rslang/ErrorLogger.h"
#include "ccl/rslang/RSLexer.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/ASTInterpreter.h"

namespace ccl::rslang {

template<typename Lexer = RSLexer>
class Interpreter {
	ErrorLogger log{};
	Lexer lexer{ log.SendReporter() };
	RSParser parser{ log.SendReporter() };
	TypeAuditor auditor;
	ASTInterpreter interpreter;
	SyntaxTreeContext astContext;

	meta::UniqueCPPtr<SyntaxTree> ast{ nullptr };

public:
	explicit Interpreter(const TypeContext& types, SyntaxTreeContext ast, DataContext data)
		: auditor{ types, log.SendReporter() }, 
			interpreter{ std::move(data), log.SendReporter() },
			astContext{ std::move(ast) } {}

public:
	[[nodiscard]] std::optional<ExpressionValue> Evaluate(const std::string& expr) {
		log.Clear();
		ast = nullptr;
		if (std::empty(expr)) {
			return std::nullopt;
		}
		
		lexer.SetInput(expr);
		if (!parser.Parse(lexer.Stream())) {
			return std::nullopt;
		} else {
			ast = parser.ExtractAST();
		}
		
		if (!auditor.CheckType(*ast)) {
			return std::nullopt;
		} else {
			ast->Normalize(astContext);
			return interpreter.Evaluate(*ast);
		}
	}

	[[nodiscard]] const ErrorLogger& Errors() const noexcept {
		return log;
	}
	[[nodiscard]] const SyntaxTree& NormalizedParseTree() const noexcept {
		return *ast;
	}
	[[nodiscard]] int32_t Iterations() const noexcept {
		return interpreter.Iterations();
	}
};

} // namespace ccl::rslang