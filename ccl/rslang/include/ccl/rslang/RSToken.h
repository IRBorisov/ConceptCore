#pragma once

#include "ccl/Strings.hpp"
#include "ccl/cclTypes.hpp"

#include <cstdint>
#include <any>

namespace ccl::rslang {

using Index = int16_t;

//! Available syntax for RSLang
enum class Syntax : uint8_t {
  UNDEF,
  MATH,      // Math syntax using UTF8 symbols
  ASCII      // Pure ASCII encoded syntax
};

//! RS language token types enumeration
enum class TokenID : uint32_t {
  // Global, local IDs and literals
  ID_LOCAL = 258,
  ID_GLOBAL,
  ID_FUNCTION,
  ID_PREDICATE,
  ID_RADICAL,
  LIT_INTEGER,
  LIT_INTSET,
  LIT_EMPTYSET,

  // Aithmetic
  PLUS,
  MINUS,
  MULTIPLY,
  
  // Integer predicate symbols
  GREATER,
  LESSER,
  GREATER_OR_EQ,
  LESSER_OR_EQ,

  // Equality comparison
  EQUAL,
  NOTEQUAL,

  // Logic predicate symbols
  FORALL,
  EXISTS,
  NOT,
  EQUIVALENT,
  IMPLICATION,
  OR,
  AND,

  // Set theory predicate symbols
  IN,
  NOTIN,
  SUBSET,
  SUBSET_OR_EQ,
  NOTSUBSET,

  // Set theory operators
  DECART,
  UNION,
  INTERSECTION,
  SET_MINUS,
  SYMMINUS,
  BOOLEAN,

  // Structure operations
  BIGPR,
  SMALLPR,
  FILTER,
  CARD,
  BOOL,
  DEBOOL,
  REDUCE,

  // Term constructions prefixes
  DECLARATIVE,
  RECURSIVE,
  IMPERATIVE,

  // Punctuation
  PUNC_DEFINE,
  PUNC_STRUCT,
  PUNC_ASSIGN,
  PUNC_ITERATE,
  PUNC_PL,
  PUNC_PR,
  PUNC_CL,
  PUNC_CR,
  PUNC_SL,
  PUNC_SR,
  PUNC_BAR,
  PUNC_COMMA,
  PUNC_SEMICOLON,

  // ======= Non-terminal tokens =========
  NT_ENUM_DECL, // Перечисление переменных в кванторной декларации
  NT_TUPLE, // Кортеж (a,b,c), типизация B(T(a)xT(b)xT(c))
  NT_ENUMERATION, // Задание множества перечислением
  NT_TUPLE_DECL,  // Декларация переменных с помощью кортежа
  NT_ARG_DECL, // Объявление аргумента

  NT_FUNC_DEFINITION, // Определение функции
  NT_ARGUMENTS, // Задание аргументов функции
  NT_FUNC_CALL, // Вызов функции
  
  NT_DECLARATIVE_EXPR, // Задание множества с помощью выражения D{x из H | A(x) }
  NT_IMPERATIVE_EXPR, // Императивное определение
  NT_RECURSIVE_FULL, // Полная рекурсия
  NT_RECURSIVE_SHORT, // Сокращенная рекурсия

  NT_IMP_DECLARE, // Блок декларации
  NT_IMP_ASSIGN, // Блок присвоения
  NT_IMP_LOGIC, // Блок проверки
  
  // ======= Helper tokens ========
  INTERRUPT,
  END,
};

//! RS token payload data
class TokenData {
  std::any data{};

public:
  TokenData() = default;
  explicit TokenData(int32_t intVal)
    : data{ intVal } {}
  explicit TokenData(std::string strVal) 
    : data{ std::move(strVal) } {}
  explicit TokenData(std::vector<Index> indicies)
    : data{ std::move(indicies) } {}

  static TokenData FromIndexSequence(const std::string& sequence);

public:
  bool operator==(const TokenData& rhs) const;
  bool operator!=(const TokenData& rhs) const { 
    return !(*this == rhs);
  }

  [[nodiscard]] bool HasValue() const noexcept {
    return data.has_value();
  }
  [[nodiscard]] bool IsInt() const noexcept {
    return data.type() == typeid(int32_t);
  }
  [[nodiscard]] int32_t ToInt() const {
    return std::any_cast<int32_t>(data);
  }
  [[nodiscard]] bool IsText() const noexcept {
    return data.type() == typeid(std::string);
  }
  [[nodiscard]] const std::string& ToText() const { 
    return std::any_cast<const std::string&>(data);
  }
  [[nodiscard]] bool IsTuple() const noexcept {
    return data.type() == typeid(std::vector<Index>);
  }
  [[nodiscard]] const std::vector<Index>& ToTuple() const {
    return std::any_cast<const std::vector<Index>&>(data);
  }
};

//! RS grammar token
struct Token {
  TokenID id{ TokenID::INTERRUPT };
  StrRange pos{};
  TokenData data{};

public:
  Token() = default;
  explicit Token(TokenID id, StrRange pos, TokenData val = {}) noexcept 
    : id{ id }, pos{ pos }, data{ std::move(val) } {}

public:
  [[nodiscard]] bool operator==(const Token& rhs) const;
  [[nodiscard]] bool operator!=(const Token& rhs) const { return !(*this == rhs); }

  [[nodiscard]] std::string ToString(Syntax syntax = Syntax::MATH) const;

  [[nodiscard]] static std::string Str(TokenID id, Syntax syntax = Syntax::MATH);
  [[nodiscard]] static Comparison CompareOperations(TokenID left, TokenID right);
};

using TokenStream = std::function<Token()>;

} // namespace ccl::rslang
