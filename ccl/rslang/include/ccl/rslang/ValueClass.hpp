#pragma once

#include <cstdint>
#include <string>
#include <functional>

namespace ccl::rslang {

enum class ValueClass : uint8_t {
  invalid,
  value,
  props
};

//! Entities ValueClass Context
using ValueClassContext = std::function<ValueClass(const std::string&)>;

} // namespace ccl::rslang