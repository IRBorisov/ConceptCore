#pragma once

#include "ccl/rslang/TypeContext.hpp"
#include "ccl/rslang/SyntaxTree.h"

namespace ccl::rslang {

//! Convert syntax
[[nodiscard]] std::string ConvertTo(const std::string& input, Syntax targetSyntax);

//! RS expression generator factory
class Generator {
  const TypeContext& environment;

public:
  explicit Generator(const TypeContext& environment) noexcept
    : environment{ environment } {}

public:
  [[nodiscard]] static std::string GlobalDefinition(std::string globalID, std::string expr, bool isStruct);
  [[nodiscard]] static std::string FromTree(const SyntaxTree& ast, Syntax syntax = Syntax::MATH);

  [[nodiscard]] static std::string CreatePrefix(const FunctionArguments& args);
  [[nodiscard]] static std::string ExtractPrefix(const std::string& declaration);
  [[nodiscard]] static std::string CreateCall(const std::string& funcName,
                                                      const std::vector<std::string>& args);
  using StructureDescription = std::vector<std::pair<std::string, Typification>>;
  [[nodiscard]] static StructureDescription StructureFor(const std::string& globalName, 
                                                         const Typification& type);

public:
  [[nodiscard]] std::string TermFromFunction(const std::string& funcName, const std::string& expression,
                                             const std::vector<std::string>& args) const;
  [[nodiscard]] std::string FunctionFromExpr(const std::vector<std::string>& args, 
                                             const std::string& expression) const;
};

} // namespace ccl::rslang