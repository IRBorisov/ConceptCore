#pragma once

#include "ccl/rslang/Typification.h"
#include "ccl/rslang/StructuredData.h"

#include <vector>

namespace ccl::object {

//! Compact representation of StructuredData
struct SDCompact {
  using Header = std::vector<std::string>;
  using Data = std::vector<std::vector<int32_t>>;

  Header header{};
  Data data{};

  static constexpr int32_t unknownCount{ 10000000 };

  SDCompact(Header header, Data data) noexcept
    : header{ std::move(header) }, data{ std::move(data) } {}
  SDCompact(const rslang::Typification& type, Data data) 
    : SDCompact(CreateHeader(type), data) {}

  [[nodiscard]] static SDCompact FromSData(const StructuredData& sdata, const rslang::Typification& type);
  [[nodiscard]] static Header CreateHeader(const rslang::Typification& type);

  [[nodiscard]] std::optional<StructuredData> Unpack(const rslang::Typification& type) const;
  [[nodiscard]] static std::optional<StructuredData> Unpack(const Data& packedData, 
                                                            const rslang::Typification& type);
};

} // namespace ccl::object