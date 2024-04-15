#pragma once

#include "ccl/rslang/DataContext.hpp"
#include "ccl/rslang/Parser.h"
#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/ASTInterpreter.h"

namespace ccl::rslang {

class Interpreter {
public:
  Parser parser{};

private:
  TypeAuditor auditor;
  ASTInterpreter interpreter;
  SyntaxTreeContext astContext;

  meta::UniqueCPPtr<SyntaxTree> ast{ nullptr };

public:
  explicit Interpreter(const TypeContext& types, SyntaxTreeContext ast, DataContext data);

public:
  [[nodiscard]] std::optional<ExpressionValue> Evaluate(const std::string& expr, Syntax syntaxHint = Syntax::UNDEF);

  [[nodiscard]] const ErrorLogger& Errors() const noexcept;
  [[nodiscard]] const SyntaxTree& NormalizedParseTree() const noexcept;
  [[nodiscard]] int32_t Iterations() const noexcept;
};

} // namespace ccl::rslang