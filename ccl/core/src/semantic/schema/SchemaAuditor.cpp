#include "ccl/semantic/SchemaAuditor.h"

#include "ccl/rslang/RSGenerator.h"

namespace ccl::semantic {

SchemaAuditor::SchemaAuditor(
  const rslang::TypeContext& types,
  rslang::ValueClassContext globalClass,
  rslang::SyntaxTreeContext globalAST
) : prefixLen{ 0 }, auditor { types, globalClass, globalAST } {}


bool SchemaAuditor::CheckConstituenta(const std::string& alias, const std::string& definition, const CstType targetType) {
  auditor.parser.log.Clear();
  const auto expr = rslang::Generator::GlobalDefinition(alias, definition, targetType == CstType::structured);
  prefixLen = static_cast<StrPos>(std::ssize(expr) - std::ssize(definition));

  const auto isBaseSet = IsBaseSet(targetType);
  if (isBaseSet != empty(definition)) {
    OnError(isBaseSet ? CstTypeEID::cstNonemptyBase : CstTypeEID::cstEmptyDerived);
    return false;
  }

  if (!auditor.CheckType(expr, rslang::Syntax::MATH)) {
    return false;
  }
  
  const auto isCallable = IsCallable(targetType);
  if (isCallable == std::empty(auditor.GetDeclarationArgs())) {
    auditor.isTypeCorrect = false;
    OnError(isCallable ? CstTypeEID::cstCallableNoArgs : CstTypeEID::cstNonCallableHasArgs);
    return false;
  }

  const auto& resultType = auditor.GetType();
  const auto isLogical = IsLogical(targetType);
  if (isLogical == std::holds_alternative<rslang::Typification>(resultType)) {
    auditor.isTypeCorrect = false;
    OnError(isLogical ? CstTypeEID::cstExpectedLogical : CstTypeEID::cstExpectedTyped);
    return false;
  }
  return true;
}

bool SchemaAuditor::CheckExpression(const std::string& expr, const rslang::Syntax syntax) {
  auditor.parser.log.Clear();
  return auditor.CheckType(expr, syntax);
}

bool SchemaAuditor::CheckValue() {
  return auditor.CheckValue();
}

bool SchemaAuditor::IsParsed() const noexcept {
  return auditor.isParsed;
}

bool SchemaAuditor::IsTypeCorrect() const noexcept {
  return auditor.isTypeCorrect;
}

bool SchemaAuditor::IsValueCorrect() const noexcept {
  return auditor.isValueCorrect;
}

const rslang::Syntax& SchemaAuditor::GetSyntax() const noexcept {
  return auditor.parser.syntax;
}

const rslang::SyntaxTree& SchemaAuditor::AST() const noexcept {
  return auditor.parser.AST();
}

const rslang::ErrorLogger& SchemaAuditor::Errors() const noexcept {
  return auditor.Errors();
}

const rslang::FunctionArguments& SchemaAuditor::GetDeclarationArgs() const noexcept {
  return auditor.GetDeclarationArgs();
}

const rslang::ExpressionType& SchemaAuditor::GetType() const noexcept {
  return auditor.GetType();
}

rslang::ValueClass SchemaAuditor::GetValueClass() const noexcept {
  return auditor.GetValueClass();
}

meta::UniqueCPPtr<rslang::SyntaxTree> SchemaAuditor::ExtractAST() noexcept {
  return auditor.parser.ExtractAST();
}

void SchemaAuditor::OnError(const CstTypeEID errorID) {
  auditor.parser.log.LogError(rslang::Error{ static_cast<uint32_t>(errorID), 0 });
}

} // namespace ccl::semantic
