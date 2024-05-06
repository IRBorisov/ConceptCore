#pragma once

#include "ccl/rslang/SyntaxTree.h"

#include <unordered_map>

namespace ccl::rslang {
//! Converter for AST into standard form
class Normalizer {
public:
  using TupleSubstitutes = std::unordered_map<std::string, std::vector<Index>>;
  using NodeSubstitutes = std::unordered_map<std::string, const SyntaxTree::Node*>;

private:
  SyntaxTreeContext termFuncs;

  TupleSubstitutes tuples{};
  NodeSubstitutes nodes{};

  std::unordered_map<std::string, std::string> nameSubstitutes{};
  uint32_t localVarBase{ 0 };

public:
  explicit Normalizer(SyntaxTreeContext termFuncs)
    : termFuncs{ std::move(termFuncs) } {}

public:
  void Normalize(SyntaxTree::Node& root);

private:
  void Quantifier(SyntaxTree::Node& quant);
  static void EnumDeclaration(SyntaxTree::Node& quant);
  void TupleDeclaration(SyntaxTree::Node& declaration, SyntaxTree::Node& predicate);
  void TupleDeclaration(SyntaxTree::Node& target);

  [[nodiscard]] std::string CreateTupleName(const SyntaxTree::Node& root);
  void SubstituteTupleVariables(SyntaxTree::Node& target, const std::string& newName);

  void Function(SyntaxTree::Node& func);
  [[nodiscard]] static std::vector<std::string> ArgNames(const SyntaxTree::Node& declaration);
  void SubstituteArgs(SyntaxTree::Node& target, StrRange pos);
};

} // namespace ccl::rslang