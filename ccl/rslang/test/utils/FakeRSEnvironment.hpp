#pragma once

#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/TypeContext.hpp"
#include "ccl/rslang/ValueClass.hpp"
#include "ccl/rslang/DataContext.hpp"
#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/Interpreter.h"
#include "ccl/rslang/Literals.h"

using ccl::rslang::operator""_rs;
using ccl::rslang::operator""_t;

class RSEnvironment final : public ccl::rslang::TypeContext {
  using DataContext = ccl::rslang::DataContext;
  using SyntaxTree = ccl::rslang::SyntaxTree;
  using StructuredData = ccl::object::StructuredData;
  using AsciiLexer = ccl::rslang::detail::AsciiLexer;
  using RSParser = ccl::rslang::detail::RSParser;
  using ValueClass = ccl::rslang::ValueClass;
  using ValueClassContext = ccl::rslang::ValueClassContext;
  using SyntaxTreeContext = ccl::rslang::SyntaxTreeContext;
  using TypeTraits = ccl::rslang::TypeTraits;
  using FunctionArguments = ccl::rslang::FunctionArguments;
  using ExpressionType = ccl::rslang::ExpressionType;
  using Typification = ccl::rslang::Typification;

private:
  AsciiLexer lexer{};
  RSParser parser{};

public:
  struct Element {
    std::optional<ExpressionType> type{};
    std::optional<TypeTraits> traits{};
    std::optional<ValueClass> valueClass{};
    std::optional<FunctionArguments> arguments{};
    std::optional<SyntaxTree> ast{};
    std::optional<StructuredData> objects{};
  };

  std::unordered_map<std::string, Element> data{};

public:
  void Insert(const std::string& globalName, const ExpressionType type, const ValueClass valueClass = ValueClass::value) {
    data[globalName].type = type;
    data[globalName].valueClass = valueClass;
  }
  void InsertBase(const std::string& globalName) {
    Insert(globalName, Typification(globalName).Bool());
  }

  bool AddAST(const std::string& globalName, const std::string& expr) {
    if (data.contains(globalName) && data[globalName].ast.has_value()) {
      return false;
    } else if (!parser.Parse(lexer(expr).Stream())) {
      return false;
    } else {
      data[globalName].ast = parser.AST();
      return true;
    }
  }

  [[nodiscard]] const ExpressionType* TypeFor(const std::string& globalName) const final {
    if (!data.contains(globalName)) {
      return nullptr;
    } else if (const auto& result = data.at(globalName).type; !result.has_value()) {
      return nullptr;
    } else {
      return &result.value();
    }
  }
  [[nodiscard]] const FunctionArguments* FunctionArgsFor(const std::string& globalName) const final {
    if (!data.contains(globalName)) {
      return nullptr;
    } else if (const auto& result = data.at(globalName).arguments; !result.has_value()) {
      return nullptr;
    } else {
      return &result.value();
    }
  }
  [[nodiscard]] std::optional<TypeTraits> TraitsFor(const Typification& type) const final {
    if (type == Typification::Integer()) {
      return ccl::rslang::TraitsIntegral;
    } else if (const auto& globalName = type.E().baseID; !data.contains(globalName)) {
      return std::nullopt;
    } else {
      return data.at(globalName).traits;
    }
  }

  DataContext GetDataContext() {
    return [&data = this->data](const std::string& globalName) -> std::optional<StructuredData> {
      if (data.contains(globalName)) {
        return data.at(globalName).objects;
      } else {
        return std::nullopt;
      }
    };
  }
  
  SyntaxTreeContext GetAST() const {
    return [&data = this->data](const std::string& globalName) -> const SyntaxTree* {
      if (!data.contains(globalName)) {
        return nullptr;
      } else if (const auto& result = data.at(globalName).ast; !result.has_value()) {
        return nullptr;
      } else {
        return &result.value();
      }
    };
  }

  ValueClassContext GetValueContext() const {
    return[&data = this->data](const std::string& globalName) -> ValueClass {
      if (!data.contains(globalName)) {
        return ValueClass::invalid;
      } else if (const auto& result = data.at(globalName).valueClass; !result.has_value()) {
        return ValueClass::invalid;
      } else {
        return result.value();
      }
    };
  }
};