#pragma once

#include "ccl/Strings.hpp"

#include <memory>
#include <cstdint>
#include <functional>
#include <vector>

namespace ccl::rslang {

//! Error status enumeration
enum class ErrorStatus : uint32_t {
  WARNING = 0x2000,
  CRITICAL = 0x8000
};

constexpr ErrorStatus ResolveErrorType(const uint32_t eid) noexcept {
  if (eid < static_cast<uint32_t>(ErrorStatus::CRITICAL)) {
    return ErrorStatus::WARNING;
  } else {
    return ErrorStatus::CRITICAL;
  }
}

//! Abstract error
struct Error {
  uint32_t eid{};
  StrPos position{};
  std::vector<std::string> params{};

public:
  explicit Error(uint32_t eid, StrPos position) noexcept : eid{ eid }, position{ position } {};
  explicit Error(uint32_t eid, StrPos position, std::vector<std::string> params) noexcept
    : eid{ eid }, position{ position }, params{ std::move(params) } {};

public:
  [[nodiscard]] bool IsWarning() const noexcept;
  [[nodiscard]] bool IsCritical() const noexcept;
};

//! Callback for reporting errors
using ErrorReporter = std::function<void(const Error&)>;

} // namespace ccl::rslang