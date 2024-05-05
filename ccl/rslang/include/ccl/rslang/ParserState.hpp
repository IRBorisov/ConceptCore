#pragma once

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"

#include <functional>
#include <optional>

namespace ccl::rslang::detail {

struct Node;
using RawNode = std::shared_ptr<Node>;

struct Node {
  Token token{};
  std::vector<RawNode> children{};

public:
  explicit Node(Token token) noexcept
    : token{ std::move(token) } {}
  explicit Node(TokenID id, StrRange pos, TokenData val = {}) noexcept 
    : Node(Token(id, pos, std::move(val))) {}
};

struct ParserState {
  meta::UniqueCPPtr<SyntaxTree> parsedTree{ nullptr };
  int32_t currentPosition{ 0 };
  int32_t countCriticalErrors{ 0 };

  std::optional<ErrorReporter> reporter{};
  TokenStream* nextTokenCall{ nullptr };

public:
  explicit ParserState(std::optional<ErrorReporter> reporter) noexcept
    : reporter{ std::move(reporter) } {}

public:
  void NewInput(TokenStream* input) noexcept {
    parsedTree = nullptr;
    currentPosition = 0;
    countCriticalErrors = 0;
    nextTokenCall = input;
  }

  void OnError(ParseEID errorCode) {
    OnError(errorCode, currentPosition);
  }

  void OnError(ParseEID errorCode, StrPos pos) {
    Error err{ static_cast<uint32_t>(errorCode), pos };
    if (err.IsCritical()) {
      ++countCriticalErrors;
    }
    if (reporter.has_value()) {
      reporter.value()(err);
    }
  }

  void CreateSyntaxTree(RawNode root);
  void FinalizeExpression(RawNode expr);
  void FinalizeCstEmpty(RawNode cst, RawNode mode);
  void FinalizeCstExpression(RawNode cst, RawNode mode, RawNode data);
};

} // namespace ccl::rslang::detail