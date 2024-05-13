#pragma once

#include "ccl/rslang/DataContext.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/SyntaxTree.h"

#include <unordered_set>
#include <functional>
#include <optional>

namespace ccl::rslang {

//! Value of RS expression
using ExpressionValue = std::variant<bool, object::StructuredData>;

//! AST evaluator for RS expression
class ASTInterpreter : public ASTVisitor<ASTInterpreter> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<ASTInterpreter>;

  class NameCollector;

public:
  using ASTVars = std::unordered_map<const SyntaxTree::Node*, std::vector<uint32_t>>;
  std::unordered_map<std::string, uint32_t> idsBase{};
  std::vector<object::StructuredData> idsData{};
  ASTVars nodeVars{};

private:
  DataContext context;
  std::optional<ErrorReporter> reporter{};

  ExpressionValue curValue{};
  int32_t iterationCounter{ 0 };
  int32_t countCriticalErrors{ 0 };

  static constexpr auto  MAX_ITERATIONS = 100000;

public:
  explicit ASTInterpreter(DataContext context)
    : context{ std::move(context) } {}
  ASTInterpreter(DataContext context, ErrorReporter reporter)
    : context{ std::move(context) }, reporter{ std::move(reporter) } {}

public:
  [[nodiscard]] std::optional<ExpressionValue> Evaluate(const SyntaxTree& tree);
  [[nodiscard]] int32_t Iterations() const noexcept { return iterationCounter; }

protected:
  bool VisitDefault(Cursor /*iter*/) noexcept { return false; } // NOLINT(readability-convert-member-functions-to-static)

  bool ViGlobalDeclaration(Cursor iter);

  // bool ViFunctionDefinition(Cursor iter)
  // bool ViFunctionCall(Cursor iter);

  bool ViGlobal(Cursor iter) { return ViLocal(iter); }
  // bool ViRadical(Cursor iter);
  bool ViLocal(Cursor iter);
  bool ViInteger(Cursor /*iter*/);
  bool ViIntegerSet(Cursor /*iter*/);
  bool ViEmptySet(Cursor iter);

  //bool ViTupleDeclaration(Cursor iter);
  //bool ViEnumDeclaration(Cursor iter);
  //bool ViArgumentsEnum(Cursor iter);
  //bool ViArgument(Cursor iter);

  bool ViArithmetic(Cursor iter);
  bool ViCard(Cursor iter);

  bool ViQuantifier(Cursor iter);
  bool ViNegation(Cursor iter);
  bool ViLogicBinary(Cursor iter);
  bool ViEquals(Cursor iter);
  bool ViIntegerPredicate(Cursor iter);
  bool ViSetexprPredicate(Cursor iter) { return ViSetexprBinary(iter); }

  bool ViDecart(Cursor iter);
  bool ViBoolean(Cursor iter);

  bool ViDeclarative(Cursor iter);
  bool ViRecursion(Cursor iter);
  bool ViImperative(Cursor iter);
  //bool ViIterate(Cursor iter);
  //bool ViAssign(Cursor iter);

  bool ViTuple(Cursor iter);
  bool ViEnumeration(Cursor iter);
  bool ViBool(Cursor iter);
  bool ViDebool(Cursor iter);

  bool ViSetexprBinary(Cursor iter);
  bool ViProjectSet(Cursor iter);
  bool ViProjectTuple(Cursor iter);
  bool ViFilter(Cursor iter);
  bool ViReduce(Cursor iter);

private:
  class ImpEvaluator;

  void Clear() noexcept;
  void AfterVisit(bool result);

  void OnError(ValueEID eid, StrPos position);
  void OnError(ValueEID eid, StrPos position, std::string param);

  [[nodiscard]] bool SetCurrent(ExpressionValue&& value) noexcept;
  [[nodiscard]] bool SetCurrent(const ExpressionValue& value) noexcept;
  [[nodiscard]] std::optional<ExpressionValue> EvaluateChild(Cursor iter, Index index);

  [[nodiscard]] std::optional<object::StructuredData> ExtractDomain(Cursor iter);
  [[nodiscard]] bool TryEvaluateFromFirstArg(TokenID operation, bool firstArgValue) noexcept;
};

} // namespace ccl::rslang