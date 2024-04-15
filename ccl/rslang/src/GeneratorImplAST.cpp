#include "GeneratorImplAST.h"

namespace ccl::rslang {

void GeneratorImplAST::Clear() noexcept {
  rsText.clear();
}

std::string GeneratorImplAST::FromTree(const SyntaxTree& ast, Syntax syntax) {
  static GeneratorImplAST generator{};
  generator.SetSyntax(syntax);
  generator.Clear();
  ast.Root().DispatchVisit(generator);
  return generator.rsText;
}

bool GeneratorImplAST::VisitDefault(Cursor iter) {
  rsText += iter->ToString(syntax);
  return true;
}

bool GeneratorImplAST::ViGlobalDefinition(Cursor iter) {
  OutputChild(iter, 0);
  rsText += iter->ToString(syntax);
  if (iter.ChildrenCount() > 1) {
    OutputChild(iter, 1);
  }
  return true;
}

bool GeneratorImplAST::ViFunctionDefinition(Cursor iter) {
  OutputChild(iter, 0);
  rsText += ' ';
  OutputChild(iter, 1);
  return true;
}

bool GeneratorImplAST::ViFunctionCall(Cursor iter) {
  OutputChild(iter, 0);
  rsText += '[';
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    if (child > 1) {
      rsText += R"(, )";
    }
    OutputChild(iter, child);
  }
  rsText += ']';
  return true;
}

bool GeneratorImplAST::ViLocalBind(Cursor iter) {
  EnumChildren(iter, '(', ')', std::string(R"(, )"));
  return true;
}

bool GeneratorImplAST::ViLocalEnum(Cursor iter) {
  EnumChildren(iter, R"(, )");
  return true;
}

bool GeneratorImplAST::ViArgumentsEnum(Cursor iter) {
  EnumChildren(iter, '[', ']', R"(, )");
  return true;
}

bool GeneratorImplAST::ViArgument(Cursor iter) {
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::IN, syntax);
  OutputChild(iter, 1);
  return true;
}

bool GeneratorImplAST::ViArithmetic(Cursor iter) {
  const auto parent = iter->id;
  const auto left = iter(0).id;
  const auto right = iter(1).id;
  const auto needLeftBrackets = 
    parent != left && 
    Token::CompareOperations(parent, left) == Comparison::GREATER; // NOLINT(readability-suspicious-call-argument)
  const auto needRightBrackets = 
    parent == right || 
    Token::CompareOperations(parent, right) == Comparison::EQUAL ||
    Token::CompareOperations(parent, right) == Comparison::GREATER;

  OutputChild(iter, 0, needLeftBrackets);
  rsText += iter->ToString(syntax);
  OutputChild(iter, 1, needRightBrackets);
  return true;
}

bool GeneratorImplAST::ViCard(Cursor iter) {
  static constexpr bool surroundWithBrackets = true;
  rsText += iter->ToString(syntax);
  OutputChild(iter, 0, surroundWithBrackets);
  return true;
}

bool GeneratorImplAST::ViFilter(Cursor iter) {
  rsText += iter->ToString(syntax);
  rsText += '[';
  Index child = 0;
  for (; child < iter.ChildrenCount() - 1; ++child) {
    if (child > 0) {
      rsText += R"(, )";
    }
    OutputChild(iter, child);
  }
  rsText += ']';
  rsText += '(';
  OutputChild(iter, child);
  rsText += ')';
  return true;
}

bool GeneratorImplAST::ViQuantifier(Cursor iter) {
  rsText += iter->ToString(syntax);
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::IN, syntax);
  OutputChild(iter, 1);
  rsText += ' ';
  OutputChild(iter, 2, Token::CompareOperations(iter->id, iter(2).id) == Comparison::GREATER);
  return true;
}

bool GeneratorImplAST::ViNegation(Cursor iter) {
  rsText += iter->ToString(syntax);
  OutputChild(iter, 0, Token::CompareOperations(iter->id, iter(0).id) == Comparison::GREATER);
  return true;
}

bool GeneratorImplAST::ViLogicBinary(Cursor iter) {
  const auto parent = iter->id;
  const auto left = iter(0).id;
  const auto right = iter(1).id;
  const auto needLeftBrackets = 
    parent != left && 
    Token::CompareOperations(parent, left) == Comparison::GREATER; // NOLINT(readability-suspicious-call-argument)
  const auto needRightBrackets = 
    parent == right ||
    Token::CompareOperations(parent, right) == Comparison::GREATER; // NOLINT(readability-suspicious-call-argument)

  OutputChild(iter, 0, needLeftBrackets);
  rsText += ' ';
  rsText += iter->ToString(syntax);
  rsText += ' ';
  OutputChild(iter, 1, needRightBrackets);
  return true;
}

bool GeneratorImplAST::ViEquals(Cursor iter) {
  OutputChild(iter, 0);
  rsText += iter->ToString(syntax);
  OutputChild(iter, 1);
  return true;
}

bool GeneratorImplAST::ViDeclarative(Cursor iter) {
  rsText += Token::Str(TokenID::DECLARATIVE, syntax);
  rsText += '{';
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::IN, syntax);
  OutputChild(iter, 1);
  rsText += R"( | )";
  OutputChild(iter, 2);
  rsText += '}';
  return true;
}

bool GeneratorImplAST::ViImperative(Cursor iter) {
  rsText += Token::Str(TokenID::IMPERATIVE, syntax);
  rsText += '{';
  OutputChild(iter, 0);
  rsText += R"( | )";
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    if (child > 1) {
      rsText += R"(; )";
    }
    OutputChild(iter, child);
  }
  rsText += '}';
  return true;
}

bool GeneratorImplAST::ViImpDeclare(Cursor iter) {
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::PUNC_ITERATE, syntax);
  OutputChild(iter, 1);
  return true;
}

bool GeneratorImplAST::ViImpAssign(Cursor iter) {
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::PUNC_ASSIGN, syntax);
  OutputChild(iter, 1);
  return true;
}

bool GeneratorImplAST::ViImpCheck(Cursor iter) {
  OutputChild(iter, 0);
  return true;
}

bool GeneratorImplAST::ViRecursion(Cursor iter) {
  rsText += Token::Str(TokenID::RECURSIVE, syntax);
  rsText += '{';
  OutputChild(iter, 0);
  rsText += Token::Str(TokenID::PUNC_ASSIGN);
  OutputChild(iter, 1);
  rsText += R"( | )";
  OutputChild(iter, 2);
  if (iter->id == TokenID::NT_RECURSIVE_FULL) {
    rsText += R"( | )";
    OutputChild(iter, 3);
  }
  rsText += '}';
  return true;
}

bool GeneratorImplAST::ViDecart(Cursor iter) {
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (child > 0) {
      rsText += Token::Str(TokenID::DECART, syntax);
    }
    OutputChild(iter, child, iter(child).id == TokenID::DECART);
  }
  return true;
}

bool GeneratorImplAST::ViBoolean(Cursor iter) {
  rsText += Token::Str(TokenID::BOOLEAN, syntax);
  OutputChild(iter, 0, iter(0).id != TokenID::BOOLEAN);
  return true;
}

bool GeneratorImplAST::ViTuple(Cursor iter) {
  EnumChildren(iter, '(', ')', R"(, )");
  return true;
}

bool GeneratorImplAST::ViSetEnum(Cursor iter) {
  EnumChildren(iter, '{', '}', R"(, )");
  return true;
}

void GeneratorImplAST::OutputChild(const Cursor& iter, const Index index, const bool addBrackets) {
  if (addBrackets) {
    rsText += '(';
  }
  VisitChild(iter, index);
  if (addBrackets) {
    rsText += ')';
  }
}

void GeneratorImplAST::EnumChildren(const Cursor& iter, const std::string& separator) {
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (child > 0) {
      rsText += separator;
    }
    OutputChild(iter, child);
  }
}

void GeneratorImplAST::EnumChildren(const Cursor& iter, const char left, const char right, const std::string& separator) {
  rsText += left;
  EnumChildren(iter, separator);
  rsText += right;
}

} // namespace ccl::rslang