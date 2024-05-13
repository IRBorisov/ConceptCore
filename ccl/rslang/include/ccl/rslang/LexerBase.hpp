#pragma once

#include "ccl/rslang/RSToken.h"
#include "ccl/rslang/Error.hpp"
#include "ccl/rslang/RSErrorCodes.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::rslang::detail {

//! Abstract Lexer
template<typename ConcreteLexer>
class LexerBase : public meta::crtp<ConcreteLexer, LexerBase> {
protected:
  std::optional<ErrorReporter> reporter{};
  TokenID lastRead{ TokenID::INTERRUPT };

protected:
  LexerBase() = default;
  explicit LexerBase(ErrorReporter reporter) noexcept
    : reporter{ std::move(reporter) } {}

public:
  TokenID lex() {
    lastRead = this->BaseT().DoLex();
    return lastRead;
  }

  [[nodiscard]] TokenStream Stream() {
    return[lex = this]() {
      lex->lex();
      auto token = lex->MakeToken();
      if (token.id == TokenID::INTERRUPT && lex->reporter.has_value()) {
        lex->reporter.value()(Error{
          static_cast<uint32_t>(LexerEID::unknownSymbol),
          token.pos.start,
          { lex->Text() }
        });
      }
      return token;
    };
  }

  [[nodiscard]] std::string Text() const {
    return this->BaseT().GetText();
  }
  [[nodiscard]] TokenID LastID() const noexcept { return lastRead; }
  [[nodiscard]] Token MakeToken() const {
    return Token{ lastRead, this->BaseT().Range(), this->BaseT().ParseData() };
  }
  [[nodiscard]] TokenData ParseData() const {
    switch (lastRead) {
    default: return TokenData{};
    case TokenID::SMALLPR:
    case TokenID::BIGPR:
    case TokenID::FILTER:
      return ToTuple(2);

    case TokenID::LIT_INTEGER:
      return ToInt();

    case TokenID::ID_GLOBAL:
    case TokenID::ID_FUNCTION:
    case TokenID::ID_PREDICATE:
    case TokenID::ID_RADICAL:
    case TokenID::ID_LOCAL:
      return TokenData{ Text() };
    }
  }

private:
  [[nodiscard]] TokenData ToInt() const {
    return TokenData{ static_cast<int32_t>(std::atol(Text().c_str())) }; // TODO: strtol
  }
  [[nodiscard]] TokenData ToTuple(const size_t prefixLen) const {
    return TokenData::FromIndexSequence(Text().erase(0, prefixLen)); // TODO: is it correct? Refactor code to be more clear
  }
};

} // namespace ccl::rslang::detail