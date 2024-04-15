#pragma once

#include "ccl/rslang/Typification.h"

#include <vector>
#include <unordered_map>

namespace ccl::rslang {

//! Type of RS expression
struct LogicT {
  [[nodiscard]] constexpr bool operator==(const LogicT& /* rhs */) const noexcept { return true; }
  [[nodiscard]] constexpr bool operator!=(const LogicT& /* rhs */) const noexcept { return false; }
};

using ExpressionType = std::variant<LogicT, Typification>;

//! Typed identifier
struct TypedID {
  std::string name;
  Typification type;

  explicit TypedID(std::string name, Typification type) noexcept
    : name{ std::move(name) }, type{ std::move(type) } {}
};

//! Templated arguments context
using FunctionArguments = std::vector<TypedID>;

//! Type properties
struct TypeTraits {
  bool isIterable;
  bool isOrdered;
  bool isOperable;
  bool convertsFromInt;
};

static constexpr TypeTraits TraitsNominal{ true, false, false, false };
static constexpr TypeTraits TraitsOrdered{ true, true, false, false };
static constexpr TypeTraits TraitsIntegral{ false, true, true, true };

//! Global types context API
struct TypeContext {
  virtual ~TypeContext() = default;

  [[nodiscard]] virtual const ExpressionType* TypeFor(const std::string& globalName) const = 0;
  [[nodiscard]] virtual const FunctionArguments* FunctionArgsFor(const std::string& globalName) const = 0;
  [[nodiscard]] virtual std::optional<TypeTraits> TraitsFor(const Typification& type) const = 0;
};

} // namespace ccl::rslang