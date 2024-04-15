#pragma once

#include "ccl/Entity.hpp"
#include "ccl/cclChange.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::src {

//! Data type enumeration
enum class DataType : uint32_t { tba = 0,  rsSchema };

//! Source type enumeration
enum class SrcType : uint32_t {  tba = 0, rsDoc };

//! Abstract data stream
class DataStream {
public:
  virtual ~DataStream() noexcept = default;

  DataStream& operator=(const DataStream&) = delete;
  DataStream& operator=(DataStream&&) = delete;

protected:
  DataStream() = default;
  DataStream(const DataStream&) = default;
  DataStream(DataStream&&) noexcept = default;
  

public:
  enum class Attribute : uint32_t {
    title, alias, comment
  };

  using ValueType = std::string;

  struct AttrValue {
    Attribute type{};
    ValueType value{};
  };

  [[nodiscard]] virtual DataType Type() const noexcept = 0;
};

template<
  typename T,
  typename = std::enable_if_t<std::is_base_of_v<DataStream, T>>
>
const T* CastData(const DataStream* data) noexcept {
  return dynamic_cast<const T*>(data);
}

//! Abstract data source
class Source {
  bool isClaimed{ false };

public:
  virtual ~Source() noexcept = default;

protected:
  Source() = default;
  Source(const Source&) = default;
  Source& operator=(const Source&) noexcept = default;
  Source& operator=(Source&&) noexcept = default;
  Source(Source&&) = default;

public:
  [[nodiscard]] virtual change::Hash CoreHash() const = 0;
  [[nodiscard]] virtual change::Hash FullHash() const = 0;
  [[nodiscard]] virtual SrcType Type() const noexcept = 0;

  virtual bool WriteData(meta::UniqueCPPtr<DataStream> /*data*/) = 0;
  [[nodiscard]] virtual const DataStream* ReadData() const = 0;
  [[nodiscard]] virtual DataStream* AccessData() = 0;

  [[nodiscard]] bool IsClaimed() const noexcept { return isClaimed; }
  void Claim() noexcept { isClaimed = true; }
  void ReleaseClaim() noexcept { isClaimed = false; }
};

//! Source descriptor
struct Descriptor {
  SrcType type{ SrcType::tba };
  std::u8string name{};

  explicit Descriptor(SrcType type = SrcType::tba, std::u8string name = {}) noexcept
    : type{ type }, name{ std::move(name) } {}

  bool operator==(const Descriptor& rhs) const noexcept { return type == rhs.type && name == rhs.name; }
  bool operator!=(const Descriptor& rhs) const noexcept { return !(*this == rhs); }
};

//! Source handle
struct Handle {
  Source* src{ nullptr };
  Descriptor desc{};

  change::Hash coreHash{ 0 };
  change::Hash fullHash{ 0 };

public:
  Handle() = default;

  explicit Handle(SrcType type) noexcept 
    : desc{ type, std::u8string{} } {}

public:
  [[nodiscard]] bool empty() const noexcept { 
    return src == nullptr && std::empty(desc.name);
  }
  void UpdateHashes() {
    coreHash = src == nullptr ? coreHash : src->CoreHash();
    fullHash = src == nullptr ? fullHash : src->FullHash();
  }
  void DiscardSrc() noexcept {
    src = nullptr;
    desc.name.clear();
    coreHash = 0;
    fullHash = 0;
  }
};

//! Abstract source callback
class SourceCallback {
public:
  virtual ~SourceCallback() noexcept = default;

protected:
  SourceCallback() = default;
  SourceCallback(const SourceCallback&) = default;
  SourceCallback& operator=(const SourceCallback&) = default;
  SourceCallback(SourceCallback&&) noexcept = default;
  SourceCallback& operator=(SourceCallback&&) noexcept = default;

public:
  [[nodiscard]] virtual change::Usage QueryEntityIsUsed(const src::Source& /*from*/, EntityUID /*entity*/) const = 0;
  [[nodiscard]] virtual bool IsConnectedWith(const src::Source& /*src*/) const = 0;
};

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

//! Callback manager base class
class CallbackManager {
  std::vector<SourceCallback*> callbacks{};

public:
  bool AddCallback(SourceCallback& newCB) {
    if (std::find(begin(callbacks), end(callbacks), &newCB) != end(callbacks)) {
      return false;
    } else {
      callbacks.emplace_back(&newCB);
      return true;
    }
  }

  void RemoveCallback(const SourceCallback& cb) {
    if (const auto item = std::find(begin(callbacks), end(callbacks), &cb); 
        item != end(callbacks)) {
      callbacks.erase(item);
    }
  }

  void ImportCallbacksFrom(CallbackManager& rhs) {
    if (this != &rhs) {
      for (auto& cb : rhs.callbacks) {
        AddCallback(*cb);
      }
    }
  }
  
  [[nodiscard]] bool QuerySrcUse(const src::Source& src) const {
    return std::any_of(begin(callbacks), end(callbacks),
      [&](const auto& callback) { 
      return callback->IsConnectedWith(src); 
    });
  }

  [[nodiscard]] change::Usage QueryEntityUse(const src::Source& from, EntityUID entity) const {
    change::Usage result = change::Usage::notUsed;
    for (const auto& callback : callbacks) {
      const auto curStatus = callback->QueryEntityIsUsed(from, entity);
      if (static_cast<uint8_t>(result) < static_cast<uint8_t>(curStatus)) {
        result = curStatus;
      }
    }
    return result;
  }
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl::src