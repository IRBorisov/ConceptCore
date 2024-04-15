#pragma once

#include "ccl/rslang/ASTInterpreter.h"

namespace ccl::rslang {

//! Name collector for ASTInterpreter
/// First pass before evaluating expression
class ASTInterpreter::NameCollector : public ASTVisitor<NameCollector> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<NameCollector>;

  ASTInterpreter& parent;

public:
  explicit NameCollector(ASTInterpreter& parent) noexcept 
    : parent{ parent } {}

public:
  bool Visit(Cursor iter);

protected:
  bool VisitDefault(Cursor iter) { return MergeChildren(iter); }

  bool ViGlobalDefinition(Cursor iter);

  bool ViGlobal(Cursor iter);
  bool ViLocal(Cursor iter);

  bool ViQuantifier(Cursor iter);

  bool ViDeclarative(Cursor iter) { return ViQuantifier(iter); }
  bool ViImperative(Cursor iter);
  bool ViRecursion(Cursor iter) { return ViQuantifier(iter); }

private:
  bool MergeChildren(Cursor iter);
};

} // namespace ccl::rslang