#pragma once

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"
#include "ccl/rslang/TypeContext.hpp"

namespace ccl::rslang {

namespace details {
//! Environment wrapper
class TypeEnv {
public:
  const TypeContext& context;

public:
  explicit TypeEnv(const TypeContext& context) noexcept 
    : context{ context } {}

public:
  [[nodiscard]] bool CompareTemplated(Typification::Substitutes& substitutes,
                                      const Typification& arg,
                                      const Typification& value);
  [[nodiscard]] bool IsArithmetic(const Typification& type) const;
  [[nodiscard]] bool IsOrdered(const Typification& type) const;
  [[nodiscard]] std::optional<Typification> Merge(const Typification& type1, const Typification& type2) const;
  [[nodiscard]] bool AreCompatible(const Typification& type1, const Typification& type2) const;
  [[nodiscard]] bool AreCompatible(const ExpressionType& type1, const ExpressionType& type2) const;

private:
  [[nodiscard]] const Typification* CommonType(const Typification& type1, const Typification& type2) const;
};

} // namespace details

//! Semantic analysis for AST representation of RS text
class TypeAuditor final : public ASTVisitor<TypeAuditor> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<TypeAuditor>;

  struct LocalData  {
    TypedID arg;
    int32_t level{};
    int32_t useCount{};
    bool enabled{ false };
  };

  details::TypeEnv env;
  std::optional<ErrorReporter> reporter{};

  ExpressionType currentType{};

  std::vector<LocalData> localVars{};
  std::vector<size_t> functionArgsID{};
  FunctionArguments functionArgs{};

  types::GuardableBool isArgDeclaration{ false };
  types::GuardableBool isLocalDeclaration{ false };
  types::GuardableBool isFuncDeclaration{ false };

public:
  explicit TypeAuditor(const TypeContext& context) noexcept
    : env{ context } {}
  explicit TypeAuditor(const TypeContext& context, ErrorReporter reporter) noexcept
    : env{ context }, reporter{ std::move(reporter) } {}

public:
  [[nodiscard]] bool CheckType(const SyntaxTree& tree);
  [[nodiscard]] const FunctionArguments& GetDeclarationArgs() const noexcept { return functionArgs; }
  [[nodiscard]] const ExpressionType& GetType() const noexcept;

protected:
  bool ViGlobalDefinition(Cursor iter);

  bool ViFunctionDefinition(Cursor iter);
  bool ViFunctionCall(Cursor iter);

  bool ViGlobal(Cursor iter);
  bool ViLocal(Cursor iter);
  bool ViInteger(Cursor /*iter*/) { return VisitAndReturn(Typification::Integer()); }
  bool ViIntegerSet(Cursor /*iter*/) { return VisitAndReturn(Typification::Integer().Bool()); }
  bool ViEmptySet(Cursor iter);

  bool ViLocalBind(Cursor iter);
  bool ViLocalEnum(Cursor iter) { return VisitAllAndReturn(iter, LogicT{}); }
  bool ViArgumentsEnum(Cursor iter) { return VisitAllAndReturn(iter, LogicT{}); }
  bool ViArgument(Cursor iter);

  bool ViArithmetic(Cursor iter);
  bool ViCard(Cursor iter);

  bool ViQuantifier(Cursor iter);
  bool ViNegation(Cursor iter) { return VisitAllAndReturn(iter, LogicT{}); }
  bool ViLogicBinary(Cursor iter) { return VisitAllAndReturn(iter, LogicT{}); }
  bool ViEquals(Cursor iter);
  bool ViOrdering(Cursor iter);
  bool ViTypedPredicate(Cursor iter);

  bool ViDecart(Cursor iter);
  bool ViBoolean(Cursor iter);

  bool ViDeclarative(Cursor iter);
  bool ViImperative(Cursor iter);
  bool ViImpDeclare(Cursor iter);
  bool ViImpAssign(Cursor iter);
  bool ViImpCheck(Cursor iter) { return VisitAllAndReturn(iter, LogicT{}); }
  bool ViRecursion(Cursor iter);

  bool ViTuple(Cursor iter);
  bool ViSetEnum(Cursor iter);
  bool ViBool(Cursor iter) { return ViSetEnum(iter); }
  bool ViDebool(Cursor iter);

  bool ViTypedBinary(Cursor iter);
  bool ViProjectSet(Cursor iter);
  bool ViProjectTuple(Cursor iter);
  bool ViFilter(Cursor iter);
  bool ViReduce(Cursor iter);

private:
  void Clear() noexcept;

  [[nodiscard]] bool VisitChildDeclaration(const Cursor& iter, Index index, const Typification& domain);

  [[nodiscard]] bool VisitAndReturn(ExpressionType type) noexcept;
  [[nodiscard]] bool VisitAllAndReturn(Cursor iter, const ExpressionType& type);

  [[nodiscard]] std::optional<ExpressionType> ChildType(Cursor iter, Index index);
  [[nodiscard]] std::optional<Typification> ChildTypeDebool(Cursor iter, Index index,  SemanticEID eid);

  void OnError(SemanticEID eid, StrPos position);
  void OnError(SemanticEID eid, StrPos position, std::string param);
  void OnError(SemanticEID eid, StrPos position, std::vector<std::string> params);
  void OnError(SemanticEID eid, StrPos position, const ExpressionType& expected, const ExpressionType& actual);

  [[nodiscard]] std::optional<Typification::Substitutes> 
    CheckFuncArguments(Cursor iter, const std::string& funcName);

  [[nodiscard]] const Typification* GetLocalTypification(const std::string& name, StrPos pos);
  [[nodiscard]] bool AddLocalVar(const std::string& name, const Typification& type, StrPos pos);
  void StartScope() noexcept;
  void EndScope(StrPos pos);
};

} // namespace ccl::rslang