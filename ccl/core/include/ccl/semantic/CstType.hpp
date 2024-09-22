#pragma once

#include "ccl/cclMeta.hpp"

namespace ccl::semantic {

//! Consituent type enumeration
enum class CstType : uint8_t {
  base = 1, // Базисное множество (БМ)
  constant = 2, // Константное множество (КМ)
  structured = 4, // Род структуры
  axiom = 5, // Аксиома
  term = 6, // Терм
  function = 7, // Терм-функция
  theorem = 8, // Теорема
  predicate = 9, // Предикат
  size_
};

[[nodiscard]] constexpr bool IsBasic(CstType type) noexcept;

[[nodiscard]] constexpr bool IsBaseSet(CstType type)  noexcept;
[[nodiscard]] constexpr bool IsBaseNotion(CstType type) noexcept;

[[nodiscard]] constexpr bool IsCalculable(CstType type) noexcept;

[[nodiscard]] constexpr bool IsStatement(CstType type) noexcept;
[[nodiscard]] constexpr bool IsRSObject(CstType type)  noexcept;
[[nodiscard]] constexpr bool IsCallable(CstType type) noexcept;

using CstTypeIter = meta::EnumIter
<
  CstType,
  CstType::base, CstType::constant, CstType::structured,
  CstType::axiom, CstType::term, CstType::function, CstType::theorem, CstType::predicate
>;

constexpr bool IsBasic(const CstType type) noexcept {
  switch (type) {
  default: return false;
  case CstType::base:
  case CstType::constant:
  case CstType::structured:
    return true;
  }
}

constexpr bool IsRSObject(const CstType type) noexcept {
  switch (type) {
  default: return false;
  case CstType::base:
  case CstType::constant:
  case CstType::structured:
  case CstType::term:
    return true;
  }
}

constexpr bool IsBaseSet(const CstType type) noexcept {
  switch (type) {
  default: return false;
  case CstType::base:
  case CstType::constant:
    return true;
  }
}

constexpr bool IsBaseNotion(const CstType type) noexcept {
  switch (type) {
  default: return false;
  case CstType::base:
  case CstType::constant:
  case CstType::structured:
    return true;
  }
}

constexpr bool IsCalculable(const CstType type) noexcept {
  return !IsBaseNotion(type) && !IsCallable(type);
}

constexpr bool IsCallable(const CstType type) noexcept {
  switch (type) {
  default: return false;
  case CstType::function:
  case CstType::predicate:
    return true;
  }
}

constexpr bool IsStatement(const CstType type)  noexcept {
  switch (type) {
  default: return false;
  case CstType::axiom:
  case CstType::theorem:
    return true;
  }
}

constexpr bool IsLogical(const CstType type)  noexcept {
  switch (type) {
  default: return false;
  case CstType::axiom:
  case CstType::theorem:
  case CstType::predicate:
    return true;
  }
}

} // namespace ccl::semantic
