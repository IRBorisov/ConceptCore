#pragma once

#include "ccl/rslang/Error.hpp"

namespace ccl::rslang {

//! Error reporting manager
class ErrorLogger {
  std::vector<Error> errors{};

public:
  void LogError(const Error& error);
  void Clear() noexcept;

  [[nodiscard]] ErrorReporter SendReporter();

  [[nodiscard]] const std::vector<Error>& All() const noexcept { return errors; }
  [[nodiscard]] StrPos FirstErrorPos() const;
  [[nodiscard]] bool HasCriticalErrors() const;
  [[nodiscard]] int32_t Count(ErrorStatus status) const;
};

} // namespace ccl::rslang