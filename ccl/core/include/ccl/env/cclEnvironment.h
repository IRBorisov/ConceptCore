#pragma once

#include "ccl/env/SourceManager.hpp"

#include <random>

namespace ccl {

//! Enivroment configuration - !Global state!
class Environment final {
  std::unique_ptr<SourceManager> sources{ std::make_unique<SourceManager>() };

public:
  [[nodiscard]] static Environment& Instance() noexcept;

  [[nodiscard]] static SourceManager& Sources() noexcept { return *Instance().sources; }
  [[nodiscard]] static std::random_device& RNG();

  [[nodiscard]] static std::string_view VersionInfo() noexcept;

  void SetSourceManager(std::unique_ptr<SourceManager> newSources);
};

} // namespace ccl