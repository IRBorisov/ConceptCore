#pragma once

#include "ccl/rslang/Typification.h"
#include "ccl/cclMeta.hpp"

#include <memory>
#include <cstdint>
#include <vector>
#include <cstddef>
#include <variant>

namespace ccl::object {

class Factory;
class SDBasicElement;
class SDTuple;
class SDSet;

using Size = int32_t;
using DataID = int32_t;

//! Structured data interpretation
class StructuredData {
  friend class Factory;

  class Impl;
  std::shared_ptr<Impl> data{ nullptr };

public:
  static constexpr auto BOOL_INFINITY = 30;
  static constexpr auto SET_INFINITY = 0x0FFFFFFF;

public:
  StructuredData() noexcept;

private:
  explicit(false) StructuredData(const Impl& impl); // NOLINT(google-explicit-constructor, hicpp-explicit-conversions)

public:
  [[nodiscard]] bool operator==(const StructuredData& rhs) const;
  [[nodiscard]] bool operator!=(const StructuredData& rhs) const { return !(*this == rhs); }
  [[nodiscard]] bool operator<(const StructuredData& rhs) const;

  [[nodiscard]] Comparison Compare(const StructuredData& rhs) const;
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] bool IsElement() const noexcept;
  [[nodiscard]] bool IsTuple() const noexcept;
  [[nodiscard]] bool IsCollection() const noexcept;
  [[nodiscard]] rslang::StructureType Structure() const noexcept;

  [[nodiscard]] const SDBasicElement& E() const noexcept;
  [[nodiscard]] const SDTuple& T() const noexcept;
  [[nodiscard]] SDSet& ModifyB();
  [[nodiscard]] const SDSet& B() const noexcept;

private:
  Impl& UniqueData();
};

//! Data factory
class Factory {
public:
  [[nodiscard]] static StructuredData EmptySet() noexcept;
  [[nodiscard]] static StructuredData Val(DataID value);
  [[nodiscard]] static std::vector<StructuredData> Vals(const std::vector<DataID>& vals);
  [[nodiscard]] static StructuredData Tuple(const std::vector<StructuredData>& components);
  [[nodiscard]] static StructuredData TupleV(const std::vector<DataID>& components);
  [[nodiscard]] static StructuredData Set(const std::vector<StructuredData>& elements);
  [[nodiscard]] static StructuredData SetV(const std::vector<DataID>& vals);

  [[nodiscard]] static StructuredData Singleton(StructuredData base);
  [[nodiscard]] static StructuredData Decartian(const std::vector<StructuredData>& factors);
  [[nodiscard]] static StructuredData Boolean(StructuredData base);
};

[[nodiscard]] bool CheckCompatible(const StructuredData& data, const rslang::Typification& type);

//! Basic data element
class SDBasicElement {
  DataID value{};

public:
  explicit SDBasicElement(DataID value) noexcept
    : value{ value } {};

public:
  [[nodiscard]] Comparison Compare(const SDBasicElement& rhs) const noexcept;
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] DataID Value() const noexcept { return value; }
};

//! Tuple data element
class SDTuple {
  std::unordered_map<rslang::Index, StructuredData> components{};

public:
  explicit SDTuple(const std::vector<StructuredData>& input);

public:
  [[nodiscard]] Comparison Compare(const SDTuple& rhs) const;
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] rslang::Index Arity() const noexcept;
  [[nodiscard]] StructuredData Component(rslang::Index index) const;
};

//! Set data element
using SDIterator = meta::PolyFCIterator<StructuredData>;
class SDSet {
public:
  class SetImpl;

private:
  std::unique_ptr<SetImpl> impl;

public:
  ~SDSet() noexcept;
  SDSet(const SDSet& rhs);
  SDSet& operator=(const SDSet& rhs);
  SDSet(SDSet&&) noexcept = default;
  SDSet& operator=(SDSet&&) noexcept = default;

  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  SDSet(const SetImpl& impl); // Note: explicit(false)

public:
  [[nodiscard]] SDIterator begin() const;
  [[nodiscard]] SDIterator end() const;
  [[nodiscard]] Size Cardinality() const noexcept;
  [[nodiscard]] bool IsEmpty() const noexcept;

  [[nodiscard]] bool Contains(const StructuredData& element) const;

  bool AddElement(StructuredData newElement);

  [[nodiscard]] Comparison Compare(const SDSet& rhs) const;
  [[nodiscard]] std::string ToString() const;

  [[nodiscard]] bool IsSubsetOrEq(const SDSet& rhs) const;

  [[nodiscard]] StructuredData Union(const SDSet& rhs) const;
  [[nodiscard]] StructuredData Intersect(const SDSet& rhs) const;
  [[nodiscard]] StructuredData Diff(const SDSet& rhs) const;
  [[nodiscard]] StructuredData SymDiff(const SDSet& rhs) const;

  [[nodiscard]] StructuredData Projection(const std::vector<rslang::Index>& indicies) const;
  [[nodiscard]] StructuredData Reduce() const;
  [[nodiscard]] StructuredData Debool() const;
};

} // namespace ccl::object