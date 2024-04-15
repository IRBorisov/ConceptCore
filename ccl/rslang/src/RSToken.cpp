#include "ccl/rslang/RSToken.h"

#include <set>
#include <cwctype>
#include <algorithm>
#include <unordered_set>

namespace ccl::rslang {

namespace {

// NOLINTBEGIN: ignore magic numbers
std::string ConvertID(std::string_view id, const Syntax syntax) {
  static constexpr std::string_view substitutes = "abgdezhviklmnxoprsstqfcjw";
  static constexpr auto piPosition = 0xBF - 0xB1 + 1;
  if (syntax == Syntax::MATH) {
    return std::string{ id };
  }
  std::string result{};
  for (auto iter = UTF8Iterator(id); iter != UTF8End(id); ++iter) {
    if (iter.SymbolSize() == 1) {
      result += *iter;
    } else {
      const auto& firstByte = static_cast<const unsigned char&>(id.at(iter.BytePosition()));
      const auto& secondByte = static_cast<const unsigned char&>(id.at(iter.BytePosition() + 1));
      if (firstByte == 0xCE && secondByte >= 0xB1 && secondByte <= 0xBF) { // alpha to omikron
        result += substitutes.at(static_cast<size_t>(secondByte) - 0xB1);
      } else if (firstByte == 0xCF && secondByte >= 0x80 && secondByte <= 0x89) { // pi to omega
        result += substitutes.at(static_cast<size_t>(piPosition) + secondByte - 0x80);
      } else {
        result += 'q';
        for (auto i = 0U; i < iter.SymbolSize(); ++i) {
          result += "u?";
        }
        result += 'p';
      }
    }
  }
  return result;
}
// NOLINTEND

[[nodiscard]] std::string SharedStr(const TokenID id) {
  switch (id) {
  default: return "UNKNOWN TOKEN";

  case TokenID::INTERRUPT: return "INTERRUPT";
  case TokenID::END: return "END";

  case TokenID::ID_LOCAL: return "LOCAL";
  case TokenID::ID_GLOBAL: return "GLOBAL";
  case TokenID::ID_FUNCTION: return "FUNCTION";
  case TokenID::ID_PREDICATE: return "PREDICATE";
  case TokenID::ID_RADICAL: return "RADICAL";

  case TokenID::LIT_INTEGER: return "INT";
  case TokenID::LIT_INTSET: return "Z";

  case TokenID::BIGPR: return "Pr";
  case TokenID::SMALLPR: return "pr";

  case TokenID::FILTER: return "Fi";
  case TokenID::CARD: return "card";
  case TokenID::BOOL: return "bool";
  case TokenID::DEBOOL: return "debool";
  case TokenID::REDUCE: return "red";

  case TokenID::DECLARATIVE: return "D";
  case TokenID::RECURSIVE: return "R";
  case TokenID::IMPERATIVE: return "I";

  case TokenID::PUNC_PL: return "(";
  case TokenID::PUNC_PR: return ")";
  case TokenID::PUNC_CL: return "{";
  case TokenID::PUNC_CR: return "}";
  case TokenID::PUNC_SL: return "[";
  case TokenID::PUNC_SR: return "]";
  case TokenID::PUNC_BAR: return "|";
  case TokenID::PUNC_COMMA: return ",";
  case TokenID::PUNC_SEMICOLON: return ";";

  case TokenID::NT_TUPLE: return "TUPLE";
  case TokenID::NT_ENUMERATION: return "SET";

  case TokenID::NT_ENUM_DECL: return "ENUM_DECLARATION";
  case TokenID::NT_TUPLE_DECL: return "TUPLE_DECLARATION";
  case TokenID::NT_ARG_DECL: return "ARG";
  case TokenID::NT_FUNC_CALL: return "CALL";
  case TokenID::NT_ARGUMENTS: return "ARGS";
  case TokenID::NT_FUNC_DEFINITION: return "FUNCTION_DEFINITION";

  case TokenID::NT_DECLARATIVE_EXPR: return "DECLARATIVE";
  case TokenID::NT_IMPERATIVE_EXPR: return "IMPERATIVE";
  case TokenID::NT_RECURSIVE_FULL: return "REC_FULL";
  case TokenID::NT_RECURSIVE_SHORT: return "REC_SHORT";

  case TokenID::NT_IMP_DECLARE: return "IDECLARE";
  case TokenID::NT_IMP_ASSIGN: return "IASSIGN";
  case TokenID::NT_IMP_LOGIC: return "ICHECK";
  }
}

[[nodiscard]] std::string AsciiStr(const TokenID id) {
  switch (id) {
  default: return SharedStr(id);

  case TokenID::LIT_EMPTYSET: return "{}";
  

  case TokenID::PLUS: return R"( \plus )";
  case TokenID::MINUS: return R"( \minus )";
  case TokenID::MULTIPLY: return R"( \multiply )";

  case TokenID::GREATER: return R"( \gr )";
  case TokenID::LESSER: return R"( \less )";
  case TokenID::GREATER_OR_EQ: return R"( \ge )";
  case TokenID::LESSER_OR_EQ: return R"( \le )";

  case TokenID::EQUAL: return R"( \eq )";
  case TokenID::NOTEQUAL: return R"( \noteq )";

  case TokenID::FORALL: return R"( \A )";
  case TokenID::EXISTS: return R"( \E )";
  case TokenID::NOT: return R"( \neg )";
  case TokenID::AND: return R"( \and )";
  case TokenID::OR: return R"( \or )";
  case TokenID::IMPLICATION: return R"( \impl )";
  case TokenID::EQUIVALENT: return R"( \equiv )";

  case TokenID::IN: return R"( \in )";
  case TokenID::NOTIN: return R"( \notin )";
  case TokenID::SUBSET: return R"( \subset )";
  case TokenID::SUBSET_OR_EQ: return R"( \subseteq )";
  case TokenID::NOTSUBSET: return R"( \notsubset )";

  case TokenID::UNION: return R"( \union )";
  case TokenID::INTERSECTION: return R"( \intersect )";
  case TokenID::SET_MINUS: return R"( \setminus )";
  case TokenID::SYMMINUS: return R"( \symmdiff )";
  case TokenID::DECART: return "*";

  case TokenID::BOOLEAN: return "B";

  case TokenID::PUNC_DEFINE: return R"( \defexpr )";
  case TokenID::PUNC_STRUCT: return R"( \deftype )";
  case TokenID::PUNC_ASSIGN: return R"( \assign )";
  case TokenID::PUNC_ITERATE: return R"( \from )";
  }
}

[[nodiscard]] std::string RSStr(const TokenID id) {
  switch (id) {
  default: return SharedStr(id);

  case TokenID::LIT_EMPTYSET: return "\xE2\x88\x85"; // \u2205

  case TokenID::PLUS: return "+";
  case TokenID::MINUS: return "-";
  case TokenID::MULTIPLY: return "*";

  case TokenID::GREATER: return ">";
  case TokenID::LESSER: return "<";
  case TokenID::GREATER_OR_EQ: return "\xE2\x89\xA5"; // \u2265
  case TokenID::LESSER_OR_EQ: return "\xE2\x89\xA4"; // \u2264

  case TokenID::EQUAL: return "=";
  case TokenID::NOTEQUAL: return "\xE2\x89\xA0"; // \u2260

  case TokenID::FORALL: return "\xE2\x88\x80"; // \u2200
  case TokenID::EXISTS: return "\xE2\x88\x83"; // \u2203
  case TokenID::NOT: return "\xC2\xAC"; // \u00AC
  case TokenID::AND: return "&";
  case TokenID::OR: return "\xE2\x88\xA8"; // \u2228
  case TokenID::IMPLICATION: return "\xE2\x87\x92"; // \u21D2
  case TokenID::EQUIVALENT: return "\xE2\x87\x94"; // \u21D4

  case TokenID::PUNC_DEFINE: return ":==";
  case TokenID::PUNC_STRUCT: return "::=";
  case TokenID::PUNC_ASSIGN: return ":=";
  case TokenID::PUNC_ITERATE: return ":\xE2\x88\x88"; // \u2208

  case TokenID::IN: return "\xE2\x88\x88"; // \u2208
  case TokenID::NOTIN: return "\xE2\x88\x89"; // \u2209
  case TokenID::SUBSET: return "\xE2\x8A\x82"; // \u2282
  case TokenID::SUBSET_OR_EQ: return "\xE2\x8A\x86"; // \u2286
  case TokenID::NOTSUBSET: return "\xE2\x8A\x84"; // \u2284

  case TokenID::UNION: return "\xE2\x88\xAA"; // \u222A
  case TokenID::INTERSECTION: return "\xE2\x88\xA9"; // \u2229
  case TokenID::SET_MINUS: return R"(\)";
  case TokenID::SYMMINUS: return "\xE2\x88\x86"; // \u2206
  case TokenID::DECART: return "\xC3\x97"; // \u00D7

  case TokenID::BOOLEAN: return "\xE2\x84\xAC"; // \u212C
  }
}

} // namespace

bool TokenData::operator==(const TokenData& rhs) const {
  if (!HasValue()) {
    return !rhs.HasValue();
  } else if (!rhs.HasValue()) {
    return false;
  } else if (data.type() != rhs.data.type()) {
    return false;
  } else {
    return (IsInt() && ToInt() == rhs.ToInt()) ||
      (IsText() && ToText() == rhs.ToText()) ||
      (IsTuple() && ToTuple() == rhs.ToTuple());
  }
}

TokenData TokenData::FromIndexSequence(const std::string& sequence) {
  std::vector<Index> indicies{};
  Index index = 0;
  for (const auto ch : sequence) {
    if (std::isdigit(static_cast<unsigned char>(ch))) {
      index *= 10; // NOLINT: ignore magic number
      index += static_cast<Index>(ch - '0'); // NOLINT: ignore narrowing conversion
    } else {
      if (index != 0) {
        indicies.push_back(index);
      }
      index = 0;
    }
  }
  if (index != 0) {
    indicies.push_back(index);
  }
  return TokenData{ indicies };
}

bool Token::operator==(const Token& rhs) const {
  return id == rhs.id &&
    pos == rhs.pos &&
    data == rhs.data;
}

std::string Token::ToString(const Syntax syntax) const {
  switch (id) {
  default: {
    return Str(id, syntax);
  }
  case TokenID::ID_LOCAL: {
    return ConvertID(data.ToText(), syntax);
  }
  case TokenID::ID_GLOBAL:
  case TokenID::ID_FUNCTION:
  case TokenID::ID_PREDICATE:
  case TokenID::ID_RADICAL: {
    return data.ToText();
  }
  case TokenID::LIT_INTEGER: {
    return std::to_string(data.ToInt());
  }
  case TokenID::BIGPR:
  case TokenID::SMALLPR:
  case TokenID::FILTER: {
    const auto& indicies = data.ToTuple();
    std::string result = Str(id) + std::to_string(*begin(indicies));
    result += std::accumulate(next(begin(indicies)), end(indicies), std::string{},
                             [](std::string text, const Index index) -> decltype(auto) {
                               text += ',';
                               text += std::to_string(index);
                               return text;
                             });
    return result;
  }
  }
}

std::string Token::Str(const TokenID id, const Syntax syntax) {
  if (syntax == Syntax::MATH) {
    return RSStr(id);
  } else {
    return AsciiStr(id);
  }
}

Comparison Token::CompareOperations(const TokenID left, const TokenID right) {
  static const std::unordered_set<TokenID> operations = {
    TokenID::PLUS,
    TokenID::MINUS,
    TokenID::MULTIPLY,

    TokenID::FORALL,
    TokenID::EXISTS,
    TokenID::NOT,
    TokenID::EQUIVALENT,
    TokenID::IMPLICATION,
    TokenID::OR,
    TokenID::AND,

    TokenID::UNION,
    TokenID::INTERSECTION,
    TokenID::SET_MINUS,
    TokenID::SYMMINUS,
    TokenID::DECART,
  };
  static const std::set<std::pair<TokenID, TokenID>> precedences = {
    { TokenID::PLUS, TokenID::MULTIPLY },
    { TokenID::MINUS, TokenID::MULTIPLY },

    { TokenID::EQUIVALENT, TokenID::IMPLICATION },
    { TokenID::EQUIVALENT, TokenID::OR },
    { TokenID::EQUIVALENT, TokenID::AND },
    { TokenID::EQUIVALENT, TokenID::NOT },
    { TokenID::EQUIVALENT, TokenID::EXISTS },
    { TokenID::EQUIVALENT, TokenID::FORALL },

    { TokenID::IMPLICATION, TokenID::OR },
    { TokenID::IMPLICATION, TokenID::AND },
    { TokenID::IMPLICATION, TokenID::NOT },
    { TokenID::IMPLICATION, TokenID::EXISTS },
    { TokenID::IMPLICATION, TokenID::FORALL },

    { TokenID::OR, TokenID::AND },
    { TokenID::OR, TokenID::NOT },
    { TokenID::OR, TokenID::EXISTS },
    { TokenID::OR, TokenID::FORALL },

    { TokenID::AND, TokenID::NOT },
    { TokenID::AND, TokenID::EXISTS },
    { TokenID::AND, TokenID::FORALL },
  };
  if (!operations.contains(left) || !operations.contains(right)) {
    return Comparison::INCOMPARABLE;
  }
  if (precedences.contains({ left, right })) {
    return Comparison::LESS;
  } else if (precedences.contains({ right, left })) {
    return Comparison::GREATER;
  } else {
    return Comparison::EQUAL;
  }
}

} // namespace ccl::rslang