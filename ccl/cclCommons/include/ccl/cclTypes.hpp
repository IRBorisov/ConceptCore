#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>
#include <array>
#include <optional>
#include <stdexcept>

namespace ccl {

//! Comparison result for non-linear order
enum class Comparison : uint8_t {
  LESS,
  EQUAL,
  GREATER,
  INCOMPARABLE
};

//! Static fixed size BiMap with linear search access. Use preferably in constexpr context and only for small sizes
template <typename Key, typename Value, std::size_t Size>
struct StaticMap {
  using DataType = std::array<std::pair<Key, Value>, Size>;
  DataType data;

  [[nodiscard]] constexpr bool ContainsKey(const Key& key) const noexcept {
    const auto itr =
      std::find_if(begin(data), end(data),
        [&key](const auto& v) { return v.first == key; });
    if (itr != end(data)) {
      return true;
    } else {
      return false;
    }
  }

  [[nodiscard]] constexpr bool ContainsValue(const Value& value) const noexcept {
    const auto itr =
      std::find_if(begin(data), end(data),
        [&value](const auto& v) { return v.second == value; });
    if (itr != end(data)) {
      return true;
    } else {
      return false;
    }
  }

  [[nodiscard]] constexpr Value AtKey(const Key& key) const {
    const auto itr =
      std::find_if(begin(data), end(data),
        [&key](const auto& v) { return v.first == key; });
    if (itr != end(data)) {
      return itr->second;
    }  else {
      throw std::range_error("Not Found");
    }
  }

  [[nodiscard]] constexpr Key AtValue(const Value& value) const {
    const auto itr =
      std::find_if(begin(data), end(data),
        [&value](const auto& v) { return v.second == value; });
    if (itr != end(data)) {
      return itr->first;
    } else {
      throw std::range_error("Not Found");
    }
  }
};

} // namespace ccl

namespace ccl::types {
//! Automatic counter guard
template<typename T>
class CounterGuard {
  T* counter;

public:
  ~CounterGuard() noexcept {
    if (counter != nullptr) {
      --(*counter);
    }
  }
  constexpr explicit CounterGuard(T& counterRef) noexcept
    : counter(&counterRef) { 
    ++(*counter);
  }
  CounterGuard(const CounterGuard&) = delete;
  CounterGuard& operator=(const CounterGuard&) = delete;
  CounterGuard(CounterGuard&&) noexcept = default;
  CounterGuard& operator=(CounterGuard&&) noexcept = default;

  void Release() {
    if (counter != nullptr) {
      --(*counter);
      counter = nullptr;
    }
  }
};

// Note: possibly not thread-safe. If needed thread-safe - make counter atomic
class GuardableBool {
  bool value;
  uint16_t guardCounter{ 0 };

public:
  constexpr explicit GuardableBool(const bool value)
    : value{ value } {}
  constexpr GuardableBool(const GuardableBool& rhs)
    : value{ rhs.value } {}
  GuardableBool& operator=(const GuardableBool& rhs) noexcept { 
    if (&rhs != this) {
      value = rhs.value;
    }
    return *this;
  }
  GuardableBool(GuardableBool&&) noexcept = default;
  GuardableBool& operator=(GuardableBool&&) noexcept = default;

public:
  using Guard = CounterGuard<uint16_t>;

  explicit(false) constexpr operator bool() const noexcept { return IsGuarded() != value; }

  [[nodiscard]] constexpr bool IsGuarded() const noexcept { return guardCounter > 0; }
  [[nodiscard]] Guard CreateGuard() noexcept { return Guard{ guardCounter }; }
};

//! Abstract message for observer
class Message {
public:
  virtual ~Message() noexcept = default;

protected:
  Message() = default;
  Message(const Message&) = default;
  Message& operator=(const Message&) = default;
  Message(Message&&) noexcept = default;
  Message& operator=(Message&&) noexcept = default;

public:
  static constexpr auto modificationCode = 0x1000U;
  static constexpr auto srcCode = 0x2000U;

  [[nodiscard]] virtual uint32_t Type() const noexcept = 0;
};

//! Message with no payload
struct BasicMsg final : Message {
  uint32_t msgID;

  explicit BasicMsg(uint32_t msgID) noexcept
    : msgID{ msgID } {}
  [[nodiscard]] uint32_t Type() const noexcept override {
    return msgID;
  }
};

//! Observer base class
class Observable;
class Observer {
  friend class Observable;

  types::GuardableBool isObserving{ true };

public:
  virtual ~Observer() noexcept = default;

protected:
  Observer() = default;
  Observer(const Observer&) = default;
  Observer& operator=(const Observer&) = default;
  Observer(Observer&&) noexcept = default;
  Observer& operator=(Observer&&) noexcept = default;

protected:
  [[nodiscard]] bool DndStatus() const noexcept {
    return !isObserving;
  }
  [[nodiscard]] auto DndGuard() noexcept {
    return isObserving.CreateGuard();
  }
  virtual void OnObserve(const Message& msg) = 0;

private:
  void TryInvoking(const Message& msg) {
    if (isObserving) {
      OnObserve(msg);
    }
  }
};

//! Observable base class
class Observable {
  std::vector<Observer*> observers{};

public:
  void AddObserver(Observer& obs) {
    observers.emplace_back(&obs);
  }

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26447 ) // Do not warn about using noexcept
#endif

  void RemoveObserver(const Observer& obs) noexcept {
    if (!empty(observers)) { // Note: After this check shouldnt throw (unless used in threaded environement)
      observers.erase(std::remove(begin(observers), end(observers), &obs), end(observers));
    }
  }

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

  void ImportObserversFrom(Observable& rhs) {
    if (this != &rhs) {
      for (auto& obs : rhs.observers) {
        AddObserver(*obs);
      }
    }
  }

protected:
  // Note: notifying dangling observer is UB! Observer should use move semantic
  void Notify(uint32_t msgID) {
    Notify(BasicMsg{ msgID });
  }
  void Notify(const Message& msg) {
    for (auto& oberver : observers) {
      oberver->TryInvoking(msg);
    }
  }
};

} // namespace ccl::types