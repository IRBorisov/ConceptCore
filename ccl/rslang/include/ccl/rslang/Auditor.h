#pragma once

#include "ccl/rslang/Parser.h"
#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/ValueAuditor.h"

namespace ccl::rslang {

//! Auditor for RS expressions
class Auditor {
public:
  bool isParsed{ false };
  bool isTypeCorrect{ false };
  bool isValueCorrect{ false };

  Parser parser{};

private:
  TypeAuditor typeAuditor;
  ValueAuditor valueAuditor;

public:
  explicit Auditor(const TypeContext& types,
                   ValueClassContext globalClass,
                   SyntaxTreeContext globalAST);

public:
  bool CheckType(const std::string& expr, Syntax syntaxHint = Syntax::UNDEF);
  bool CheckValue();

  [[nodiscard]] const ErrorLogger& Errors() const noexcept;
  [[nodiscard]] const FunctionArguments& GetDeclarationArgs() const noexcept;
  [[nodiscard]] const ExpressionType& GetType() const noexcept;
  [[nodiscard]] ValueClass GetValueClass() const noexcept;
};

} // namespace ccl::rslang