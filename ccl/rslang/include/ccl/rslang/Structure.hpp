#pragma once

#include <cstdint>
#include <variant>
#include <stdexcept>

namespace ccl::rslang {
//! Structure types enumeration
enum class StructureType : uint8_t {
  basic = 0,
  tuple,
  collection
};

//! Structured base class
template<
  typename Element,
  typename Tuple,
  typename Collection
>
class Structured {
protected:
  std::variant<Element, Tuple, Collection> state;

public:
  explicit constexpr Structured(Element el) noexcept
    : state{ std::move(el) } {}
  explicit constexpr Structured(Tuple tuple) noexcept
    : state{ std::move(tuple) } {}
  explicit constexpr Structured(Collection collection) noexcept
    : state{ std::move(collection) } {}

public:
  [[nodiscard]] constexpr bool IsElement() const noexcept {
    return std::holds_alternative<Element>(state);
  }
  [[nodiscard]] constexpr bool IsTuple() const noexcept {
    return std::holds_alternative<Tuple>(state);
  }
  [[nodiscard]] constexpr bool IsCollection() const noexcept {
    return std::holds_alternative<Collection>(state);
  }
  [[nodiscard]] constexpr rslang::StructureType Structure() const noexcept {
    if (IsElement()) { 
      return StructureType::basic; 
    } else if (IsTuple()) {
      return StructureType::tuple;
    } else {
      return StructureType::collection;
    }
  }

  [[nodiscard]] Element& E() noexcept {
    return *std::get_if<Element>(&state);
  }
  [[nodiscard]] const Element& E() const noexcept {
    return *std::get_if<Element>(&state);
  }
  [[nodiscard]] Tuple& T() noexcept {
    return *std::get_if<Tuple>(&state);
  }
  [[nodiscard]] const Tuple& T() const noexcept {
    return *std::get_if<Tuple>(&state);
  }
  [[nodiscard]] Collection& B() noexcept {
    return *std::get_if<Collection>(&state);
  }
  [[nodiscard]] const Collection& B() const noexcept {
    return *std::get_if<Collection>(&state);
  }
};

} // namespace ccl::rslang

