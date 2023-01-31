#include "ccl/rslang/RSParser.h"

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 26434 26440 26446 26447 26493 )
#endif

#include "RSParserImpl.h"

#ifdef _MSC_VER
	#pragma warning( pop )
#endif


#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 26418 26415 26440 )
#endif

namespace ccl::rslang {

using RawNode = parse::NodePtr;

RSParser::RSParser(std::optional<ErrorReporter> reporter)
	: state{ reporter }, impl{ std::make_unique<parse::RSParserImpl>(&state) } {}

RSParser::~RSParser() = default;
RSParser::RSParser(RSParser&& rhs) noexcept = default;
RSParser& RSParser::operator=(RSParser&& rhs) noexcept = default;

bool RSParser::Parse(TokenStream input) {
	state.NewInput(&input); // Note: using reference to function scoped object
	const auto success = impl->parse() == 0 && state.countCriticalErrors == 0;
	if (!success) {
		if (state.countCriticalErrors == 0) {
			state.OnError(ParseEID::syntax);
		}
	}
	return success;
}

namespace parse {

int yylex(RawNode* yylval, ParserState* state) {
	static constexpr auto endParsing = 0;
	const auto token = (*state->nextTokenCall)();
	state->currentPosition = token.pos.start;

	if (token.id == TokenID::INTERRUPT) {
		++state->countCriticalErrors;
		*yylval = nullptr;
		return endParsing;
	} else if (token.id == TokenID::END) {
		*yylval = nullptr;
		return endParsing;
	} else {
		*yylval = std::make_shared<parse::Node>(token);
		return static_cast<int>(token.id);
	}
}

void RSParserImpl::error(const std::string& /*msg*/) {}

RawNode AddNode(const TokenID token, RawNode son) {
	auto result = std::make_shared<parse::Node>(Token(token, son->token.pos));

	result->children.emplace_back(son);
	return result;
}

NodePtr AddNode(const TokenID token, NodePtr son1, NodePtr son2) {
	auto result = std::make_shared<parse::Node>(
		Token{ token, StrRange{ son1->token.pos.start, son2->token.pos.finish } });
	result->children.emplace_back(son1);
	result->children.emplace_back(son2);
	return result;
}

RawNode BinaryOperation(RawNode op1, RawNode operation, RawNode op2) {
	operation->token.pos = StrRange{ op1->token.pos.start, op2->token.pos.finish };
	operation->children.emplace_back(op1);
	operation->children.emplace_back(op2);
	return operation;
}

RawNode UnaryOperation(RawNode operation, RawNode operand) {
	operation->token.pos.finish = operand->token.pos.finish;
	operation->children.emplace_back(operand);
	return operation;
}

RawNode RemoveBrackets(RawNode br1, RawNode operand, RawNode br2) {
	operand->token.pos = StrRange{ br1->token.pos.start, br2->token.pos.finish };
	auto bracketNode = std::make_shared<parse::Node>(Token(TokenID::PUNC_PL, operand->token.pos));
	bracketNode->children.emplace_back(operand);
	return bracketNode;
}

RawNode ReplaceBrackets(const TokenID token, RawNode br1, RawNode argList, RawNode br2) {
	auto result = std::make_shared<parse::Node>(Token(token, StrRange{ br1->token.pos.start, br2->token.pos.finish }));
	result->children = argList->children;
	return result;
}

RawNode Enumeration(const TokenID token, RawNode el1, RawNode el2) {
	auto result = std::make_shared<parse::Node>(Token(token, StrRange{ el1->token.pos.start, el2->token.pos.finish }));
	if (el1->token.id != token) {
		result->children.emplace_back(el1);
	} else {
		result->children = el1->children;
	}
	if (el2->token.id != token) {
		result->children.emplace_back(el2);
	} else {
		result->children.insert(end(result->children), begin(el2->children), end(el2->children));
	}
	return result;
}

RawNode Quantifier(RawNode quant, RawNode declaration, RawNode domain, RawNode predicate) {
	quant->token.pos.finish = predicate->token.pos.finish;
	quant->children.emplace_back(declaration);
	quant->children.emplace_back(domain);
	quant->children.emplace_back(predicate);
	return quant;
}

RawNode FunctionCall(RawNode function, RawNode args, RawNode rs) {
	auto result = std::make_shared<parse::Node>(
		Token{ TokenID::NT_GLOBAL_CALL,	StrRange{ function->token.pos.start, rs->token.pos.finish } });
	result->children.emplace_back(function);
	result->children.insert(end(result->children), begin(args->children), end(args->children));
	return result;
}

RawNode FilterCall(RawNode filter, RawNode params, RawNode argument, RawNode rp) {
	filter->token.pos.finish = rp->token.pos.finish;
	filter->children = params->children;
	filter->children.emplace_back(argument);
	return filter;
}

RawNode TextOperator(RawNode oper, RawNode args, RawNode rp) {
	oper->token.pos.finish = rp->token.pos.finish;
	oper->children.emplace_back(args);
	return oper;
}

RawNode Decartian(RawNode op1, RawNode decart, RawNode op2) {
	if (op1->token.id == TokenID::DECART) {
		auto result = std::move(op1);
		result->token.pos.finish = op2->token.pos.finish;
		result->children.emplace_back(op2);
		return result;
	} else {
		return BinaryOperation(std::move(op1), std::move(decart), std::move(op2));
	}
}

RawNode TermDeclaration(RawNode lc, RawNode declaration, RawNode domain, RawNode predicate, RawNode rc) {
	auto result = std::make_shared<parse::Node>(
		Token{ TokenID::NT_DECLARATIVE_EXPR,	StrRange{ lc->token.pos.start, rc->token.pos.finish } });
	result->children.emplace_back(declaration);
	result->children.emplace_back(domain);
	result->children.emplace_back(predicate);
	return result;
}

RawNode FullRecursion(RawNode rec, RawNode localid, 
											RawNode domain, RawNode condition, RawNode iteration, RawNode rc) {
	auto result = std::make_shared<parse::Node>(
		Token{ TokenID::NT_RECURSIVE_FULL, StrRange{ rec->token.pos.start, rc->token.pos.finish } });
	result->children.emplace_back(localid);
	result->children.emplace_back(domain);
	result->children.emplace_back(condition);
	result->children.emplace_back(iteration);
	return result;
}

RawNode ShortRecursion(RawNode rec, RawNode localid, 
											 RawNode domain, RawNode iteration, RawNode rc) {
	auto result = std::make_shared<parse::Node>(
		Token{ TokenID::NT_RECURSIVE_SHORT, StrRange{ rec->token.pos.start, rc->token.pos.finish } });
	result->children.emplace_back(localid);
	result->children.emplace_back(domain);
	result->children.emplace_back(iteration);
	return result;
}

RawNode Imperative(RawNode imp, RawNode value, RawNode actions, RawNode rc) {
	auto result = std::make_shared<parse::Node>(
		Token{ TokenID::NT_IMPERATIVE_EXPR, StrRange{ imp->token.pos.start, rc->token.pos.finish } });
	result->children.emplace_back(value);
	result->children.insert(end(result->children), begin(actions->children), end(actions->children));
	return result;
}

SyntaxTree::NodePtr CreateNodeRecursive(parse::Node& astNode) {
	if (astNode.token.id == TokenID::PUNC_PL) {
		return CreateNodeRecursive(*astNode.children.at(0));
	} else {
		auto result = std::make_unique<SyntaxTree::Node>(std::move(astNode.token));
		for (const auto& child : astNode.children) {
			result->AdoptChild(CreateNodeRecursive(*child));
		}
		return result;
	}
}

void ParserState::CreateSyntaxTree(RawNode root) {
	parsedTree = std::make_unique<SyntaxTree>(CreateNodeRecursive(*root));
}

void ParserState::FinalizeASTExpression(RawNode expr) {
	CreateSyntaxTree(expr);
}

void ParserState::FinalizeASTCstNoExpr(RawNode cst, RawNode mode) {
	mode->token.pos = StrRange{ cst->token.pos.start, mode->token.pos.finish };
	mode->children.emplace_back(cst);
	CreateSyntaxTree(mode);
}

void ParserState::FinalizeASTCstExpr(RawNode cst, RawNode mode, RawNode data) {
	mode->token.pos = StrRange{ cst->token.pos.start, data->token.pos.finish };
	mode->children.emplace_back(cst);
	mode->children.emplace_back(data);
	CreateSyntaxTree(mode);
}

void ParserState::FinalizeASTFunction(RawNode tf, RawNode mode, RawNode argdecl, RawNode expr) {
	mode->token.pos = StrRange{ tf->token.pos.start, expr->token.pos.finish };
	mode->children.emplace_back(tf);
	mode->children.emplace_back(argdecl);
	mode->children.emplace_back(expr);
	CreateSyntaxTree(mode);
}

} // namespace parse

} // namespace ccl::rslang

#ifdef _MSC_VER
	#pragma warning( pop )
#endif