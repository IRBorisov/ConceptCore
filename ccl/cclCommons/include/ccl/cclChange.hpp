#pragma once

#include "ccl/cclTypes.hpp"

#include <string>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <numeric>

namespace ccl::change {
using Hash = uint32_t;

inline Hash HashString(const std::string& string) {
  static constexpr auto BERNSTEIN_CONSTANT = 33U;
  return std::accumulate(begin(string), end(string), Hash{ 0 },
                         [&](Hash hash, char symbol) noexcept
                          { return hash * BERNSTEIN_CONSTANT + static_cast<uint32_t>(symbol); });
}

//! Usage types enumeration
enum class Usage : uint8_t {
  notUsed = 0, // не используется
  asElement = 1, // Наследуется
  asOption = 2, // Используется в таблице отождествлений
};

//! Abstract modification info
struct Modification {};

//! Abstract modification message
class ModMessage : public types::Message {
  const Modification* modInfo{ nullptr };

public:
  ModMessage() = default;
  explicit ModMessage(const Modification& modInfo) noexcept 
    : modInfo{ &modInfo } {}

  [[nodiscard]] const Modification* GetModInfo() const noexcept { return modInfo; }
  [[nodiscard]] uint32_t Type() const noexcept override { return modificationCode; }  
};

//! Observable modifications base
class ObservableMods : public types::Observable {
protected:
  void NotifyModification(const Modification& mod = {}) {
    Notify(ModMessage{ mod });
  }
};

} // namespace ccl::change
