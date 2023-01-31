#pragma once

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"

#include <functional>

namespace ccl::rslang::parse {

struct Node;
using NodePtr = std::shared_ptr<Node>;

struct Node {
	Token token{};
	std::vector<NodePtr> children{};

public:
	explicit Node(Token token) noexcept
		: token{ std::move(token) } {}
};

struct ParserState {
	meta::UniqueCPPtr<SyntaxTree> parsedTree{ nullptr };
	int32_t currentPosition{ 0 };
	int32_t countCriticalErrors{ 0 };

	std::optional<ErrorReporter> reporter{};
	TokenStream* nextTokenCall{ nullptr };

public:
	explicit ParserState(std::optional<ErrorReporter> reporter) noexcept
		: reporter{ std::move(reporter) } {}

public:
	void NewInput(TokenStream* input) noexcept {
		parsedTree = nullptr;
		currentPosition = 0;
		countCriticalErrors = 0;
		nextTokenCall = input;
	}

	void OnError(ParseEID errorCode) {
		OnError(errorCode, currentPosition);
	}

	void OnError(ParseEID errorCode, StrPos pos) {
		Error err{ static_cast<uint32_t>(errorCode), pos };
		if (err.IsCritical()) {
			++countCriticalErrors;
		}
		if (reporter.has_value()) {
			reporter.value()(err);
		}
	}

	void FinalizeASTExpression(NodePtr expr);
	void FinalizeASTCstNoExpr(NodePtr cst, NodePtr mode);
	void FinalizeASTCstExpr(NodePtr cst, NodePtr mode, NodePtr data);
	void FinalizeASTFunction(NodePtr tf, NodePtr mode, NodePtr argdecl, NodePtr expr);
	void CreateSyntaxTree(NodePtr root);
};

} // namespace ccl::rslang::parse