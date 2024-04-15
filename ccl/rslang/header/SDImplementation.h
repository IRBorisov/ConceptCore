#pragma once

#include "ccl/cclMeta.hpp"
#include "ccl/rslang/StructuredData.h"

#include <map>
#include <set>
#include <vector>
#include <unordered_map>

namespace ccl::object {

using SDSetPtr = std::unique_ptr<SDSet::SetImpl>;

//! Structured data implementation
class StructuredData::Impl : public rslang::Structured<SDBasicElement, SDTuple, SDSet> {
public:
  using Structured::Structured;
};

//! Abstract data set implementation
class SDSet::SetImpl {
public:
  virtual ~SetImpl() noexcept = default;

protected:
  SetImpl() = default;
  SetImpl(const SetImpl&) = default;
  SetImpl& operator=(const SetImpl&) = default;
  SetImpl(SetImpl&&) noexcept = default;
  SetImpl& operator=(SetImpl&&) noexcept = default;

public:
  [[nodiscard]] virtual SDSetPtr Clone() const = 0;
  [[nodiscard]] virtual SDIterator begin() const = 0;
  [[nodiscard]] virtual SDIterator end() const = 0;

  [[nodiscard]] virtual bool Contains(StructuredData element) const = 0;
  [[nodiscard]] virtual Size Cardinality() const noexcept = 0;

  virtual bool AddElement(StructuredData newElement) = 0;
};

//! Enumeration set implementation
class SDEnumSet : public SDSet::SetImpl {
  std::set<StructuredData> elements{};

public:
  [[nodiscard]] SDSetPtr Clone() const override;
  [[nodiscard]] SDIterator begin() const override { return SDIterator{ elements.begin() }; }
  [[nodiscard]] SDIterator end() const override { return SDIterator{ elements.end() }; }

  [[nodiscard]] bool Contains(StructuredData element) const override;
  [[nodiscard]] Size Cardinality() const noexcept override { return static_cast<Size>(ssize(elements)); }

  bool AddElement(StructuredData newElement) override;
};

//! Abstract cached set
class CachedSD : public SDSet::SetImpl {
  using CacheIndex = uint32_t;
  using Container = std::unordered_map<CacheIndex, StructuredData>;

  mutable  Container  cachedElements{};

protected:
  CachedSD() = default;

protected:
  [[nodiscard]] bool IsCached(CacheIndex index) const;
  [[nodiscard]] const StructuredData& GetCache(CacheIndex index) const;
  const StructuredData& SaveCache(CacheIndex index, StructuredData value) const;
};

//! Powerset data implementation
class SDPowerSet final : public CachedSD {
  StructuredData base;
  Size size{ 0 };

public:
  explicit SDPowerSet(StructuredData base);

public:
  [[nodiscard]] SDSetPtr Clone() const override;
  [[nodiscard]] SDIterator begin() const override;
  [[nodiscard]] SDIterator end() const override;

  [[nodiscard]] bool Contains(StructuredData element) const override;
  [[nodiscard]] Size Cardinality() const noexcept override { return size; }

  bool AddElement(StructuredData /*newElement*/) noexcept override { return false; }

private:
  void UpdateSize() noexcept;

private:
  class Iterator {
    const SDPowerSet* boolean;
    std::vector<SDIterator> itemIterators{};
    bool isCompleted{ true };
    uint32_t counter{ 0 };

  public:
    explicit Iterator(const SDPowerSet& boolean, bool isCompleted = false) noexcept;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = StructuredData;
    using difference_type = std::ptrdiff_t;
    using pointer = const StructuredData*;
    using reference = const StructuredData&;

    [[nodiscard]] bool operator==(const Iterator& rhs) const noexcept;
    [[nodiscard]] bool operator!=(const Iterator& rhs) const noexcept { return !(*this == rhs); }

    Iterator& operator++();

    [[nodiscard]] pointer operator->() const { return &this->operator*(); }
    [[nodiscard]] reference operator*() const;

  private:
    bool Increment();
    bool IncrementLastItem(Size offset);
  };
};

//! Decartian data implementation
class SDDecartian final : public CachedSD {
  Size count{ 0 };
  std::vector<StructuredData> factors{};

public:
  // Note: size(factors) >= 2!
  explicit SDDecartian(std::vector<StructuredData> factors) noexcept;

public:
  [[nodiscard]] SDSetPtr Clone() const override;
  [[nodiscard]] SDIterator begin() const override;
  [[nodiscard]] SDIterator end() const override;

  [[nodiscard]] bool Contains(StructuredData element) const override;
  [[nodiscard]] Size Cardinality() const noexcept override { return count; }

  bool AddElement(StructuredData /*newElement*/) noexcept override { return false; }

private:
  void UpdateSize() noexcept;

private:
  class Iterator {
    const SDDecartian* decartian{ nullptr };
    std::vector<SDIterator> componentIters{};
    bool isCompleted{ true };
    uint32_t counter{ 0 };

  public:
    explicit Iterator(const SDDecartian& decartaian, bool isCompleted = false);

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = StructuredData;
    using difference_type = std::ptrdiff_t;
    using pointer = const StructuredData*;
    using reference = const StructuredData&;

    [[nodiscard]] bool operator==(const Iterator& rhs) const noexcept;
    [[nodiscard]] bool operator!=(const Iterator& rhs) const noexcept { return !(*this == rhs); }

    Iterator& operator++();

    [[nodiscard]] pointer operator->() const { return &this->operator*(); }
    [[nodiscard]] reference operator*() const;

  private:
    bool IncrementComponent(SDIterator& component, size_t index);
  };
};

} // namespace ccl::object