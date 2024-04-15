#include "ccl/rslang/ErrorLogger.h"

#include <algorithm>

namespace ccl::rslang {

bool Error::IsWarning() const noexcept {
  return ResolveErrorType(eid) == ErrorStatus::WARNING;
}

bool Error::IsCritical() const noexcept {
  return ResolveErrorType(eid) == ErrorStatus::CRITICAL;
}

void ErrorLogger::Clear() noexcept {
  errors.clear();
}

void ErrorLogger::LogError(const Error& error) {
  errors.emplace_back(error);
}

int32_t ErrorLogger::Count(const ErrorStatus status) const {
  return static_cast<int32_t>(std::count_if(begin(errors), end(errors), 
                   [&](const Error& error) noexcept 
                   { return ResolveErrorType(error.eid) == status; }));
}

ErrorReporter ErrorLogger::SendReporter() {
  return [log = this](const Error& err) { log->LogError(err); };
}

StrPos ErrorLogger::FirstErrorPos() const {
  if (std::empty(errors)) {
    return 0;
  } else {
    return std::accumulate(next(begin(errors)), end(errors), begin(errors)->position,
                           [](StrPos minimum, const Error& error) 
                           { return std::min(error.position, minimum); });
  }
}

bool ErrorLogger::HasCriticalErrors() const {
  return std::any_of(begin(errors), end(errors), 
                     [](const Error& error) noexcept 
                     { return error.IsCritical(); });
}

} // namespace ccl::rslang