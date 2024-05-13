#pragma once

#include "ccl/rslang/SyntaxTree.h"

#include <unordered_map>

namespace ccl::rslang {
//! Converter for AST into standard form
class Normalizer {
public:
  using TupleSubstitutes = std::unordered_map<std::string, std::vector<Index>>;
  using NodeSubstitutes = std::unordered_map<std::string, const SyntaxTree::Node*>;
  using NameSubstitutes = std::unordered_map<std::string, std::string>;

private:
  SyntaxTreeContext termFuncs;

  TupleSubstitutes tupleSubstitutes{};
  NodeSubstitutes nodeSubstitutes{};
  NameSubstitutes nameSubstitutes{};
  uint32_t localVarBase{ 0 };

public:
  explicit Normalizer(SyntaxTreeContext termFuncs)
    : termFuncs{ std::move(termFuncs) } {}

public:
  void Normalize(SyntaxTree::Node& root);

private:
  void Quantifier(SyntaxTree::Node& quant);
  void Imperative(SyntaxTree::Node& root);
  void Recursion(SyntaxTree::Node& root);
  void Declarative(SyntaxTree::Node& root);
  void Function(SyntaxTree::Node& func);

  static void EnumDeclaration(SyntaxTree::Node& quant);
  void TupleDeclaration(SyntaxTree::Node& declaration, SyntaxTree::Node& predicate);

  [[nodiscard]] std::string ProcessTupleDeclaration(SyntaxTree::Node& root);
  void SubstituteTupleVariables(SyntaxTree::Node& target, const std::string& newName);
  
  [[nodiscard]] static std::vector<std::string> ArgNames(const SyntaxTree::Node& declaration);
  void SubstituteArgs(SyntaxTree::Node& target, StrRange pos);
};

} // namespace ccl::rslang