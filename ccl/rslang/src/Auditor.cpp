#pragma once

#include "ccl/rslang/Auditor.h"

namespace ccl::rslang {


Auditor::Auditor(const TypeContext& types, ValueClassContext globalClass, SyntaxTreeContext globalAST)
  : typeAuditor{ types, parser.log.SendReporter() },
  valueAuditor{ std::move(globalClass), std::move(globalAST),
  parser.log.SendReporter() } {}

bool Auditor::CheckType(const std::string& expr, const Syntax syntaxHint) {
  isTypeCorrect = false;
  isValueCorrect = false;
  isParsed = parser.Parse(expr, syntaxHint);
  isTypeCorrect = isParsed && typeAuditor.CheckType(parser.AST());
  return isTypeCorrect;
}

bool Auditor::CheckValue() {
  isValueCorrect = isTypeCorrect && valueAuditor.Check(parser.AST());
  return isValueCorrect;
}

const ErrorLogger& Auditor::Errors() const noexcept {
  return parser.log;
}

const FunctionArguments& Auditor::GetDeclarationArgs() const noexcept {
  return typeAuditor.GetDeclarationArgs();
}

const ExpressionType& Auditor::GetType() const noexcept {
  return typeAuditor.GetType();
}

ValueClass Auditor::GetValueClass() const noexcept {
  if (isValueCorrect) {
    return valueAuditor.VType();
  } else {
    return ValueClass::invalid;
  }
}

} // namespace ccl::rslang