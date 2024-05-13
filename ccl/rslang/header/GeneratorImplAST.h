#pragma once

#include "ccl/rslang/SyntaxTree.h"

namespace ccl::rslang {

//! Generate string from AST of RS expression
class GeneratorImplAST final : public ASTVisitor<GeneratorImplAST> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<GeneratorImplAST>;

  std::string rsText{};
  Syntax syntax{ Syntax::MATH };

public:
  static std::string FromTree(const SyntaxTree& ast, Syntax syntax);

private:
  bool VisitDefault(Cursor iter);

  bool ViGlobalDeclaration(Cursor iter);

  bool ViFunctionDefinition(Cursor iter);
  bool ViFunctionCall(Cursor iter);

  // bool ViGlobal(Cursor iter);
  // bool ViRadical(Cursor iter);
  // bool ViLocal(Cursor iter);
  // bool ViInteger(Cursor /*iter*/);
  // bool ViIntegerSet(Cursor iter)
  // bool ViEmptySet(Cursor iter);

  bool ViTupleDeclaration(Cursor iter);
  bool ViEnumDeclaration(Cursor iter);
  bool ViArgumentsEnum(Cursor iter);
  bool ViArgument(Cursor iter);

  bool ViArithmetic(Cursor iter);
  bool ViCard(Cursor iter);

  bool ViQuantifier(Cursor iter);
  bool ViNegation(Cursor iter);
  bool ViLogicBinary(Cursor iter);
  bool ViEquals(Cursor iter) { return OutputBinary(iter); }
  bool ViIntegerPredicate(Cursor iter) { return OutputBinary(iter); }
  bool ViSetexprPredicate(Cursor iter) { return OutputBinary(iter); }

  bool ViDeclarative(Cursor iter);
  bool ViRecursion(Cursor iter);
  bool ViImperative(Cursor iter);
  bool ViIterate(Cursor iter) { return OutputBinary(iter); }
  bool ViAssign(Cursor iter) { return OutputBinary(iter); }

  bool ViDecart(Cursor iter);
  bool ViBoolean(Cursor iter);

  bool ViTuple(Cursor iter);
  bool ViEnumeration(Cursor iter);
  bool ViBool(Cursor iter) { return ViCard(iter); }
  bool ViDebool(Cursor iter) { return ViCard(iter); }

  bool ViSetexprBinary(Cursor iter) { return ViArithmetic(iter); }
  bool ViProjectSet(Cursor iter) { return ViCard(iter); }
  bool ViProjectTuple(Cursor iter) { return ViCard(iter); }
  bool ViFilter(Cursor iter);
  bool ViReduce(Cursor iter) { return ViCard(iter); }

private:
  void SetSyntax(const Syntax newSyntax) noexcept { syntax = newSyntax; }
  void Clear() noexcept;

  void OutputChild(const Cursor& iter, Index index, bool addBrackets = false);
  bool OutputBinary(const Cursor& iter);

  bool EnumChildren(const Cursor& iter, const std::string& separator);
  bool EnumChildren(const Cursor& iter, char left, char right, const std::string& separator);
};

} // namespace ccl::rslang