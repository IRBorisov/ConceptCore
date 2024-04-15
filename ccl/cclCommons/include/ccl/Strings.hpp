#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <algorithm>
#include <optional>
#include <functional>
#include <cassert>
#include <utility>

namespace ccl {

inline std::string operator""_c17(const char8_t* input, size_t /*size*/) {
// TODO: C++20 use constexpr version from http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1423r0.html
  return std::string{ reinterpret_cast<const char*>(input) };
}

template<typename T>
std::u8string to_u8string(const T& obj) {
  auto str = std::to_string(obj);
  return std::u8string{ begin(str), end(str) }; // TODO: do not copy!
}

template<>
inline std::u8string to_u8string<std::string>(const std::string& obj) {
  return std::u8string{ begin(obj), end(obj) };
}

inline std::string u8to_string(const std::u8string& obj) {
  return std::string{ begin(obj), end(obj) };
}

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26481 ) // disable pointer arithmetic warning - it is ok to use it on string_view
#endif

//! Split string_view by specific symbol
inline std::vector<std::string_view> SplitBySymbol(std::string_view text, char delim = ',') {
  std::vector<std::string_view> items{};
  int32_t leftComma = 0;
  int32_t rightComma = -1;
  int32_t index = -1;
  for (const auto& symbol : text) {
    ++index;
    if (symbol != delim) {
      continue;
    }
    leftComma = rightComma;
    rightComma = index;
    const auto start = leftComma + 1;
    const auto length = rightComma - start;
    items.emplace_back(text.data() + start, length);
  }
  items.emplace_back(text.data() + rightComma + 1, static_cast<int32_t>(text.size()) - rightComma - 1);
  return items;
}

//! Trim whitespace
inline std::string_view TrimWhitespace(std::string_view text) {
  if (empty(text)) {
    return text;
  }
  size_t start = 0U;
  size_t end = text.length() - 1;
  while (std::isspace(text.at(start)) && ++start < end) {}
  while (std::isspace(text.at(end)) && end != 0 && --end >= start) {}
  const size_t length = start > end ? 0 : end - start + 1;
  return { text.data() + start, length };
}

//! Check if text is integer (including negative integers)
inline bool IsInteger(std::string_view text) {
  if (empty(text)) {
    return false;
  }
  size_t pos = 0;
  if (text.at(pos) == '-') {
    ++pos;
    if (text.length() == 1) {
      return false;
    }
  }
  while (pos < text.length()) {
    if (!std::isdigit(text.at(pos))) {
      return false;
    }
    ++pos;
  }
  return true;
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

//! String position corresponds to CodePoint position
/*
  Warning: codepoint position != array index in char* UTF8 string
*/
using StrPos = int32_t;

//! String range based on interval algebra
/*
  Warning: lightweight class, doesnt provide any bounds checks!
  For reference see https://en.wikipedia.org/wiki/Allen%27s_interval_algebra
  Pre: start <= finish
*/
struct StrRange {
  constexpr StrRange() noexcept = default;
  explicit constexpr StrRange(StrPos start, StrPos end) noexcept
    : start{ start }, finish{ end } {}

  [[nodiscard]] static constexpr StrRange FromLength(StrPos start, StrPos len) noexcept {
    return StrRange{ start, start + len };
  }

  StrPos start{ 0 };
  StrPos finish{ 0 };

  [[nodiscard]] constexpr StrPos length() const noexcept { return finish - start; }
  [[nodiscard]] constexpr bool empty() const noexcept { return finish == start; }

  [[nodiscard]] constexpr bool operator==(const StrRange& rhs) const noexcept { 
    return start == rhs.start && finish == rhs.finish;
  }
  [[nodiscard]] constexpr bool operator!=(const StrRange& rhs) const noexcept {
    return !(*this == rhs);
  }
  [[nodiscard]] constexpr bool Contains(const StrPos pos) const noexcept {
    return start <= pos && finish > pos;
  }
  [[nodiscard]] constexpr bool Contains(const StrRange& rhs) const noexcept {
    if (std::empty(rhs)) {
      return Contains(rhs.finish);
    } else {
      return start <= rhs.start && finish >= rhs.finish;
    }
  }
  [[nodiscard]] constexpr bool SharesBorder(const StrRange& rhs) const noexcept {
    return Meets(rhs) || rhs.Meets(*this);
  }
  
  [[nodiscard]] constexpr bool IsBefore(const StrRange& rhs) const noexcept {
    return finish < rhs.start;
  }
  [[nodiscard]] constexpr bool IsAfter(const StrRange& rhs) const noexcept {
    return start > rhs.finish;
  }
  [[nodiscard]] constexpr bool Meets(const StrRange& rhs) const noexcept {
    return finish == rhs.start;
  }
  [[nodiscard]] constexpr bool Overlaps(const StrRange& rhs) const noexcept {
    if (start == rhs.start) {
      return true;
    } else if (start < rhs.start) {
      return finish > rhs.start;
    } else {
      return rhs.finish > start;
    }
  }
  [[nodiscard]] constexpr bool Starts(const StrRange& rhs) const noexcept {
    return start == rhs.start && finish < rhs.finish;
  }
  [[nodiscard]] constexpr bool Finishes(const StrRange& rhs) const noexcept {
    return finish == rhs.finish && start > rhs.start;
  }
  [[nodiscard]] constexpr bool IsDuring(const StrRange& rhs) const noexcept {
    return start > rhs.start && finish < rhs.finish;
  }

  //! Pre: assert(ammount >= 0)
  constexpr StrRange& SetLength(StrPos ammount) noexcept {
    finish = start + ammount;
    return *this;
  }
  constexpr StrRange& Shift(StrPos ammount) noexcept {
    start = start + ammount;
    finish = finish + ammount;
    return *this;
  }
  constexpr StrRange& CollapseEnd() noexcept {
    start = finish;
    return *this;
  }
  constexpr StrRange& CollapseStart() noexcept {
    finish = start;
    return *this;
  }

  [[nodiscard]] constexpr std::optional<StrRange> Intersect(const StrRange& rhs) const {
    if (IsBefore(rhs) || IsAfter(rhs)) {
      return std::nullopt;
    } else {
      return StrRange{ std::max(start, rhs.start), std::min(finish, rhs.finish) };
    }
  }

  static constexpr StrRange Merge(const std::vector<StrRange>& base) {
    if (std::empty(base)) {
      return StrRange{};
    } else {
      return std::accumulate(next(begin(base)), end(base), *begin(base),
                             [](StrRange bounds, const StrRange& rng) noexcept {
        bounds.start = std::min(rng.start, bounds.start);
        bounds.finish = std::max(rng.finish, bounds.finish);
        return bounds;
      });
    }
  }
};

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26446 ) // Note: do not warn about indexes
#endif

//! Evaluate codepoint size from first byte of UTF8 string
/*
  Requires UTF8 conforming input
*/
[[nodiscard]] constexpr int8_t UTF8CharSize(unsigned const char firstByte) noexcept {
  constexpr unsigned char kFirstBitMask = 128; // 1000000
  constexpr unsigned char kThirdBitMask = 32; // 0010000
  constexpr unsigned char kFourthBitMask = 16; // 0001000
  if ((firstByte & kFirstBitMask) == 0) {
    return 1;
  } else if ((firstByte & kThirdBitMask) == 0) {
    return 2;
  } else if ((firstByte & kFourthBitMask) == 0) {
    return 3;
  } else {
    return 4;
  }
}

//! iterator for UTF8 string
/*
  Warning: iterator returns first byte of each codepoint.
  To access UTF8 symbol one should use BytePosition and SymbolSize functions
*/
class UTF8Iterator {
public:
  // stl compatibility
  using iterator_category = std::forward_iterator_tag;
  using value_type = char;
  using difference_type = std::ptrdiff_t;
  using pointer = const char*;
  using reference = const char&;

public:
  constexpr explicit UTF8Iterator(const std::string_view data, const StrPos position = 0) noexcept
    : data{ data } {
    if (position != endPos) {
      GotoCodepoint(position);
    }
  }

  static constexpr StrPos endPos = -1;

private:
  StrPos current{ endPos };
  size_t bytePosition{ 0 };
  std::string_view data;

public:
  // Note: comparing iterators from different strings is unspecified behavior!
  [[nodiscard]] bool operator==(const UTF8Iterator& rhs) const noexcept {
    return current == rhs.current;
  }
  [[nodiscard]] bool operator!=(const UTF8Iterator& rhs) const noexcept { 
    return !(*this == rhs);
  }

  UTF8Iterator& operator++() noexcept {
    bytePosition += SymbolSize(); // Note: range check not needed
    ++current;
    if (size(data) <= bytePosition) {
      current = endPos;
    }
    return *this;
  }

  [[nodiscard]] reference operator*() const noexcept { return data[bytePosition]; }
  [[nodiscard]] size_t BytePosition() const noexcept { return bytePosition; }
  [[nodiscard]] StrPos Position() const noexcept { return current; }
  [[nodiscard]] size_t SymbolSize() const noexcept { 
    return static_cast<size_t>(UTF8CharSize(static_cast<unsigned char>(data[bytePosition])));
  }

private:
  void GotoCodepoint(const StrPos cpPosition) noexcept {
    assert(cpPosition >= 0);
    for (current = 0; current != cpPosition && size(data) > bytePosition; ++current) {
      bytePosition += SymbolSize(); // Note: range check not needed
    }
    if (size(data) <= bytePosition) {
      current = endPos;
    }
  };
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

[[nodiscard]] constexpr UTF8Iterator UTF8Begin(const std::string_view source) {
  return UTF8Iterator{ source };
}

[[nodiscard]] constexpr UTF8Iterator UTF8End(const std::string_view source) {
  return UTF8Iterator{ source, UTF8Iterator::endPos };
}

[[nodiscard]] inline std::string_view Substr(const std::string_view source, const StrRange range) noexcept {
  // Note: add constexpr in C++20
  const auto start = UTF8Iterator(source, range.start);
  auto finish = UTF8Iterator(source, range.finish - 1);
  if (start == UTF8End(source) || finish == UTF8End(source)) {
    return {};
  } else if (++finish == UTF8End(source)) {
    return source.substr(start.BytePosition(), size(source) - start.BytePosition());
  } else {
    return source.substr(start.BytePosition(), finish.BytePosition() - start.BytePosition());
  }
}

[[nodiscard]] inline StrPos SizeInCodePoints(const std::string_view utf8string) noexcept {
  auto size = 0;
  for (auto it = UTF8Begin(utf8string); it != UTF8End(utf8string); ++it, ++size) {}
  return size;
}

} // namespace ccl