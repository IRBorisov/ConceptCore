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

#include <optional>
#include <unordered_map>

namespace ccl::rslang::detail {

RSParser::RSParser(std::optional<ErrorReporter> reporter)
  : state{ reporter }, impl{ std::make_unique<RSParserImpl>(&state) } {}

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
    *yylval = std::make_shared<Node>(token);
    return static_cast<int>(token.id);
  }
}

void RSParserImpl::error(const std::string& /*msg*/) {}

RawNode AddNode(const TokenID token, RawNode son) {
  auto result = std::make_shared<Node>(token, son->token.pos);
  result->children.emplace_back(son);
  return result;
}

RawNode AddNode(const TokenID token, RawNode son1, RawNode son2) {
  auto result = std::make_shared<Node>(token, StrRange{ son1->token.pos.start, son2->token.pos.finish });
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
  auto bracketNode = std::make_shared<Node>(TokenID::PUNC_PL, operand->token.pos);
  bracketNode->children.emplace_back(operand);
  return bracketNode;
}

RawNode ReplaceBrackets(const TokenID token, RawNode br1, RawNode argList, RawNode br2) {
  auto result = std::make_shared<Node>(token, StrRange{ br1->token.pos.start, br2->token.pos.finish });
  result->children = argList->children;
  return result;
}

RawNode Enumeration(const TokenID token, RawNode el1, RawNode el2) {
  auto result = std::make_shared<Node>(token, StrRange{ el1->token.pos.start, el2->token.pos.finish });
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

RawNode FunctionDeclaration(RawNode start, RawNode argumentsDeclaration, RawNode expr) {
  auto result = std::make_shared<Node>(
    TokenID::NT_FUNC_DEFINITION,
    StrRange{ start->token.pos.start, expr->token.pos.finish }
  );
  result->children.emplace_back(argumentsDeclaration);
  result->children.emplace_back(expr);
  return result;
}

RawNode FunctionCall(RawNode function, RawNode args, RawNode rs) {
  auto result = std::make_shared<Node>(
    TokenID::NT_FUNC_CALL,
    StrRange{ function->token.pos.start, rs->token.pos.finish }
  );
  result->children.emplace_back(function);
  result->children.insert(
    end(result->children),
    begin(args->children),
    end(args->children)
  );
  return result;
}

RawNode FilterCall(RawNode filter, RawNode params, RawNode argument, RawNode rp) {
  filter->token.pos.finish = rp->token.pos.finish;
  filter->children = params->children;
  filter->children.emplace_back(argument);
  return filter;
}

RawNode TextOperator(RawNode operatorName, RawNode args, RawNode rp) {
  operatorName->token.pos.finish = rp->token.pos.finish;
  operatorName->children.emplace_back(args);
  return operatorName;
}

RawNode Decartian(RawNode op1, RawNode decartian, RawNode op2) {
  if (op1->token.id == TokenID::DECART) {
    auto result = std::move(op1);
    result->token.pos.finish = op2->token.pos.finish;
    result->children.emplace_back(op2);
    return result;
  } else {
    return BinaryOperation(std::move(op1), std::move(decartian), std::move(op2));
  }
}

RawNode TermDeclaration(
  RawNode lc,
  RawNode declaration,
  RawNode domain,
  RawNode predicate,
  RawNode rc
) {
  auto result = std::make_shared<Node>(
    TokenID::NT_DECLARATIVE_EXPR,
    StrRange{ lc->token.pos.start, rc->token.pos.finish }
  );
  result->children.emplace_back(declaration);
  result->children.emplace_back(domain);
  result->children.emplace_back(predicate);
  return result;
}

RawNode TupleDeclaration(ParserState* state, RawNode tuple) {
  // TODO: check tuple contains only local variables, ParseEID::expectedLocal
  std::vector<Node*> stack{ tuple.get()};
  while (!stack.empty()) {
    auto node = stack.back();
    const auto id = node->token.id;
    stack.pop_back();
    if (id == TokenID::NT_TUPLE) {
      node->token.id = TokenID::NT_TUPLE_DECL;
    } else if (id != TokenID::ID_LOCAL) {
      state->OnError(ParseEID::expectedLocal, node->token.pos.start);
      return nullptr;
    }
    for (const auto& child: node->children) {
      stack.emplace_back(child.get());
    }
  }
  return tuple;
}

RawNode FullRecursion(
  RawNode rec,
  RawNode declaration, 
  RawNode domain,
  RawNode condition,
  RawNode iteration,
  RawNode rc
) {
  auto result = std::make_shared<Node>(
    TokenID::NT_RECURSIVE_FULL,
    StrRange{ rec->token.pos.start, rc->token.pos.finish }
  );
  result->children.emplace_back(declaration);
  result->children.emplace_back(domain);
  result->children.emplace_back(condition);
  result->children.emplace_back(iteration);
  return result;
}

RawNode ShortRecursion(
  RawNode rec,
  RawNode declaration, 
  RawNode domain,
  RawNode iteration,
  RawNode rc
) {
  auto result = std::make_shared<Node>(
    TokenID::NT_RECURSIVE_SHORT,
    StrRange{ rec->token.pos.start, rc->token.pos.finish }
  );
  result->children.emplace_back(declaration);
  result->children.emplace_back(domain);
  result->children.emplace_back(iteration);
  return result;
}

RawNode Imperative(RawNode imp, RawNode value, RawNode actions, RawNode rc) {
  auto result = std::make_shared<Node>(
    TokenID::NT_IMPERATIVE_EXPR,
    StrRange{ imp->token.pos.start, rc->token.pos.finish }
  );
  result->children.emplace_back(value);
  result->children.insert(end(result->children), begin(actions->children), end(actions->children));
  return result;
}

bool SemanticCheck(ParserState* state, RawNode root) {
  std::vector<Node*> stack{ root.get() };
  std::vector<Node*> parents{ nullptr };
  while (!stack.empty()) {
    const auto node = stack.back();
    stack.pop_back();
    const auto parent = parents.back();
    parents.pop_back();

    const auto id = node->token.id;
    if (id == TokenID::ASSIGN || id == TokenID::ITERATE) {
      if (parent == nullptr || parent->token.id != TokenID::NT_IMPERATIVE_EXPR) {
        state->OnError(ParseEID::invalidImperative, node->token.pos.start);
        return false;
      }
    }
    for (const auto& child: node->children) {
      stack.emplace_back(child.get());
      parents.emplace_back(node);
    }
  }
  return true;
}

SyntaxTree::RawNode CreateNodeRecursive(Node& astNode) {
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

bool ParserState::CreateSyntaxTree(RawNode root) {
  if (!SemanticCheck(this, root)) {
    return false;
  }
  parsedTree = std::make_unique<SyntaxTree>(CreateNodeRecursive(*root));
  return true;
}

bool ParserState::FinalizeExpression(RawNode expr) {
  return CreateSyntaxTree(expr);
}

bool ParserState::FinalizeCstEmpty(RawNode cst, RawNode mode) {
  mode->token.pos = StrRange{ cst->token.pos.start, mode->token.pos.finish };
  mode->children.emplace_back(cst);
  return CreateSyntaxTree(mode);
}

bool ParserState::FinalizeCstExpression(RawNode cst, RawNode mode, RawNode data) {
  mode->token.pos = StrRange{ cst->token.pos.start, data->token.pos.finish };
  mode->children.emplace_back(cst);
  mode->children.emplace_back(data);
  return CreateSyntaxTree(mode);
}

} // namespace ccl::rslang::detail

#ifdef _MSC_VER
  #pragma warning( pop )
#endif