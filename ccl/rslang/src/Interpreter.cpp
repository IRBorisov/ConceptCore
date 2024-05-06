#include "ccl/rslang/Interpreter.h"

#include <optional>

namespace ccl::rslang {

Interpreter::Interpreter(const TypeContext& types, SyntaxTreeContext ast, DataContext data)
  : auditor{ types, parser.log.SendReporter() }, 
    interpreter{ std::move(data), parser.log.SendReporter() },
    astContext{ std::move(ast) } {}

std::optional<ExpressionValue>
Interpreter::Evaluate(const std::string& expr, const Syntax syntaxHint) {
  parser.log.Clear();
  ast = nullptr;
  if (std::empty(expr)) {
    return std::nullopt;
  }
  
  if (!parser.Parse(expr, syntaxHint)) {
    return std::nullopt;
  } else {
    ast = parser.ExtractAST();
  }
    
  if (!auditor.CheckType(*ast)) {
    return std::nullopt;
  } else {
    ast->Normalize(astContext);
    return interpreter.Evaluate(*ast);
  }
}

const ErrorLogger& Interpreter::Errors() const noexcept {
  return parser.log;
}

const SyntaxTree& Interpreter::NormalizedParseTree() const noexcept {
  return *ast;
}

int32_t Interpreter::Iterations() const noexcept {
  return interpreter.Iterations();
}

} // namespace ccl::rslang