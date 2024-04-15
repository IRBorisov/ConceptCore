#pragma once

#include "ccl/Source.hpp"

namespace ccl::oss {

using PictID = uint32_t;

//! Media hyperlink data
struct MediaLink {
  std::string address{};
  std::string subAddr{};

  [[nodiscard]] bool empty() const noexcept {
    return std::empty(address);
  }
  bool operator==(const MediaLink& rhs) const noexcept {
    return address == rhs.address && subAddr == rhs.subAddr;
  }
  bool operator!=(const MediaLink& rhs) const noexcept {
    return address != rhs.address || subAddr != rhs.subAddr;
  }
};

//! Pictogramm data structure
struct Pict {
  PictID uid{ 0 };
  src::DataType dataType{ src::DataType::rsSchema };

  std::string title{};
  std::string alias{};
  std::string comment{};

  MediaLink lnk;

  Pict() = default;
  explicit Pict(const PictID uid) noexcept
    : uid{ uid } {}
};

using PictPtr = const Pict*;

} // namespace ccl::oss