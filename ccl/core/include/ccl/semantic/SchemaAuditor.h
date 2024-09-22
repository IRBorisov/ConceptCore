#pragma once

#include "ccl/semantic/CstType.hpp"
#include "ccl/rslang/Auditor.h"

#include <optional>

namespace ccl::semantic {

//! CstType semantic errors enumeration
enum class CstTypeEID : uint32_t {
  cstNonemptyBase = 0x8860,       // Ѕазисное или константное множество не пусто
  cstEmptyDerived = 0x8861,       // ќпределение пон€ти€ пусто
  cstCallableNoArgs = 0x8862,     // ѕараметризованное выражение без аргументов
  cstNonCallableHasArgs = 0x8863, // Ќепараметризованное выражение с аргументами
  cstExpectedLogical = 0x8864,    // ќжидалось логическое выражение
  cstExpectedTyped = 0x8865,      // ќжидалось теоретико-множественное выражение
};

//! Auditor for Schema - wrapper around rsLang::Auditor
class SchemaAuditor {
public:
  StrPos prefixLen;

private:
  rslang::Auditor auditor;

public:
  explicit SchemaAuditor(
    const rslang::TypeContext& types,
    rslang::ValueClassContext globalClass,
    rslang::SyntaxTreeContext globalAST
  );

public:
  bool CheckConstituenta(const std::string& alias, const std::string& definition, CstType targetType);
  bool CheckExpression(const std::string& expr, rslang::Syntax syntax = rslang::Syntax::MATH);
  bool CheckValue();

  [[nodiscard]] bool IsParsed() const noexcept;
  [[nodiscard]] bool IsTypeCorrect() const noexcept;
  [[nodiscard]] bool IsValueCorrect() const noexcept;

  [[nodiscard]] const rslang::Syntax& GetSyntax() const noexcept;
  [[nodiscard]] const rslang::SyntaxTree& AST() const noexcept;
  [[nodiscard]] const rslang::ErrorLogger& Errors() const noexcept;
  [[nodiscard]] const rslang::FunctionArguments& GetDeclarationArgs() const noexcept;
  [[nodiscard]] const rslang::ExpressionType& GetType() const noexcept;
  [[nodiscard]] rslang::ValueClass GetValueClass() const noexcept;
  [[nodiscard]] meta::UniqueCPPtr<rslang::SyntaxTree> ExtractAST() noexcept;

private:
  void OnError(CstTypeEID errorID);
};

} // namespace ccl::semantic
