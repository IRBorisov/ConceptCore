#pragma once

#include "ccl/cclTypes.hpp"

#include <cstdint>
#include <list>
#include <memory>

struct FakeObserver : ccl::types::Observer {
  std::vector<uint32_t> events{};

  void OnObserve(const ccl::types::Message& msg) override {
    events.emplace_back(msg.Type());
  }

  auto DndGuard() { return ccl::types::Observer::DndGuard(); }
  [[nodiscard]] bool DndStatus() const noexcept { return Observer::DndStatus(); }
};

struct FakeObservable : ccl::types::Observable {
  void Notify(const uint32_t msgID) { ccl::types::Observable::Notify(msgID); }
  void Notify(const ccl::types::Message& msg) { ccl::types::Observable::Notify(msg); }
};