#include "ccl/env/cclEnvironment.h"

namespace ccl {

void Environment::SetSourceManager(std::unique_ptr<SourceManager> newSources) {
  newSources->ImportObserversFrom(*sources);
  newSources->ImportCallbacksFrom(*sources);
  sources = std::move(newSources);
}

Environment& Environment::Instance() noexcept {
  static Environment instance{};
  return instance; 
}

std::string_view Environment::VersionInfo() noexcept {
  static const std::string_view version = R"(CCL 3.0.0)";
  return version;
}

std::random_device& Environment::RNG() {
  static std::random_device rd{};
  return rd;
}

} // namespace ccl
