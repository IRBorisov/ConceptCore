#pragma once

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"
#include "ccl/rslang/ValueClass.hpp"

namespace ccl::rslang {

//! Semantic analysis for AST representation of RS text
class ValueAuditor final : public ASTVisitor<ValueAuditor> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<ValueAuditor>;

  ValueClassContext globalClass;
  SyntaxTreeContext globalAST;
  std::optional<ErrorReporter> reporter{};

  ValueClass current{ ValueClass::invalid };
  std::vector<std::string> localProps{};

public:
  explicit ValueAuditor(ValueClassContext globalClass, 
                        SyntaxTreeContext globalAST) noexcept
    : globalClass{ std::move(globalClass) }, globalAST{ std::move(globalAST) } {}
  explicit ValueAuditor(ValueClassContext globalClass, 
                        SyntaxTreeContext globalAST, 
                        ErrorReporter reporter) noexcept
    : globalClass{ std::move(globalClass) }, globalAST{ std::move(globalAST) }, reporter{ std::move(reporter) } {}

public:
  [[nodiscard]] bool Check(const SyntaxTree& tree);
  [[nodiscard]] ValueClass VType() const noexcept { return current; }

protected:
  bool ViGlobalDefinition(Cursor iter);

  bool ViFunctionDefinition(Cursor iter) { return VisitAllChildren(iter); }
  bool ViFunctionCall(Cursor iter);

  bool ViGlobal(Cursor iter);
  bool ViLocal(Cursor iter);
  bool ViInteger(Cursor /*iter*/) noexcept { return VisitAndReturn(ValueClass::value); }
  bool ViIntegerSet(Cursor /*iter*/) noexcept { return VisitAndReturn(ValueClass::props); }
  bool ViEmptySet(Cursor /*iter*/) noexcept { return VisitAndReturn(ValueClass::value); }

  bool ViLocalBind(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViLocalEnum(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViArgumentsEnum(Cursor iter) { return VisitAllChildren(iter); }
  bool ViArgument(Cursor iter) { return VisitAllChildren(iter); }

  bool ViArithmetic(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViCard(Cursor iter) { return AssertChildIsValue(iter, 0); }

  bool ViQuantifier(Cursor iter);
  bool ViNegation(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViLogicBinary(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViEquals(Cursor iter) { return AssertAllValues(iter); }
  bool ViOrdering(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViTypedPredicate(Cursor iter);

  bool ViDecart(Cursor iter);
  bool ViBoolean(Cursor iter);

  bool ViDeclarative(Cursor iter);
  bool ViImperative(Cursor iter);
  bool ViImpDeclare(Cursor iter) { return AssertChildIsValue(iter, 1); }
  bool ViImpAssign(Cursor iter) { return AssertChildIsValue(iter, 1); }
  bool ViImpCheck(Cursor iter) { return VisitAllAndReturn(iter, ValueClass::value); }
  bool ViRecursion(Cursor iter) { return AssertAllValues(iter); }

  bool ViTuple(Cursor iter) { return AssertAllValues(iter); }
  bool ViSetEnum(Cursor iter) { return AssertAllValues(iter); }
  bool ViBool(Cursor iter) { return AssertChildIsValue(iter, 0); }
  bool ViDebool(Cursor iter) { return AssertChildIsValue(iter, 0); }

  bool ViTypedBinary(Cursor iter);
  bool ViProjectSet(Cursor iter) { return AssertChildIsValue(iter, 0); }
  bool ViProjectTuple(Cursor iter) { return AssertChildIsValue(iter, 0); }
  bool ViFilter(Cursor iter) { return VisitAllChildren(iter); }
  bool ViReduce(Cursor iter) { return AssertChildIsValue(iter, 0); }

private:
  void Clear() noexcept;
  [[nodiscard]] bool VisitAndReturn(ValueClass type) noexcept;
  [[nodiscard]] bool VisitAllAndReturn(Cursor iter, ValueClass type);

  [[nodiscard]] bool AssertAllValues(Cursor iter);
  [[nodiscard]] bool AssertChildIsValue(Cursor iter, Index index);

  [[nodiscard]] bool RunCheckOnFunc(Cursor iter, 
                                    const std::string& funcName, 
                                    const std::vector<ValueClass>& argumentVals);

  void OnError(SemanticEID eid, StrPos position);
  void OnError(SemanticEID eid, StrPos position, std::string param);
};

} // namespace ccl::rslang