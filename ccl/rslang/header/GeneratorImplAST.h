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

  bool ViGlobalDefinition(Cursor iter);

  bool ViFunctionDefinition(Cursor iter);
  bool ViFunctionCall(Cursor iter);

  // bool ViGlobal(Cursor iter);
  // bool ViLocal(Cursor iter);
  // bool ViInteger(Cursor /*iter*/);
  // bool ViIntegerSet(Cursor iter)
  // bool ViEmptySet(Cursor iter);

  bool ViLocalBind(Cursor iter);
  bool ViLocalEnum(Cursor iter);
  bool ViArgumentsEnum(Cursor iter);
  bool ViArgument(Cursor iter);

  bool ViArithmetic(Cursor iter);
  bool ViCard(Cursor iter);

  bool ViQuantifier(Cursor iter);
  bool ViNegation(Cursor iter);
  bool ViLogicBinary(Cursor iter);
  bool ViEquals(Cursor iter);
  bool ViOrdering(Cursor iter) { return ViEquals(iter); }
  bool ViTypedPredicate(Cursor iter) { return ViEquals(iter); }

  bool ViDeclarative(Cursor iter);
  bool ViImperative(Cursor iter);
  bool ViImpDeclare(Cursor iter);
  bool ViImpAssign(Cursor iter);
  bool ViImpCheck(Cursor iter);
  bool ViRecursion(Cursor iter);

  bool ViDecart(Cursor iter);
  bool ViBoolean(Cursor iter);

  bool ViTuple(Cursor iter);
  bool ViSetEnum(Cursor iter);
  bool ViBool(Cursor iter) { return ViCard(iter); }
  bool ViDebool(Cursor iter) { return ViCard(iter); }

  bool ViTypedBinary(Cursor iter) { return ViArithmetic(iter); }
  bool ViProjectSet(Cursor iter) { return ViCard(iter); }
  bool ViProjectTuple(Cursor iter) { return ViCard(iter); }
  bool ViFilter(Cursor iter);
  bool ViReduce(Cursor iter) { return ViCard(iter); }

private:
  void SetSyntax(const Syntax newSyntax) noexcept { syntax = newSyntax; }
  void Clear() noexcept;

  void OutputChild(const Cursor& iter, Index index, bool addBrackets = false);

  void EnumChildren(const Cursor& iter, const std::string& separator);
  void EnumChildren(const Cursor& iter, char left, char right, const std::string& separator);
};

} // namespace ccl::rslang