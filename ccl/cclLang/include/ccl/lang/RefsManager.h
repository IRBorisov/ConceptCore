#pragma once

#include "ccl/lang/Reference.h"

namespace ccl::lang {

//! References management for a string
/*
  * Represents vector of references for a contiguous text
  * Vector is ordered by start position.
  * Positions do not overlap.
  * Positions are separated by at least 1 symbol.
  * External iterator functor is used to inject position updates.
*/
class RefsManager {
  std::vector<Reference> refs{};
  const EntityTermContext* context{ nullptr };

public:
  RefsManager() noexcept = default;
  explicit RefsManager(const EntityTermContext& cntxt) noexcept;

  [[nodiscard]] const auto& get() const noexcept { return refs; }

  void clear() noexcept {
    refs.clear();
  }

  void SetContext(const EntityTermContext& cntxt) noexcept;

  //! Resolve references from \p text and calculate resolved positions
  //  Pre: context != nullptr
  std::string Resolve(std::string_view text);

  //! Add reference and update positions for other references
  //! //  Pre: context != nullptr
  const Reference* Insert(Reference newRef, StrPos insWhere);

  //! Update data positions using \p RangeIterator
  /* 
    Note: returns false if position count doesnt match data count
    or position length doesnt match data length
  */
  template<typename RangeIterator>
  bool UpdatePositions(RangeIterator nextPos);

  //! Erase references in range or at position
  std::optional<StrRange> EraseIn(StrRange range, bool expandRefs = false);

  //! Get data representing given \p range
  [[nodiscard]] const Reference* FirstIn(StrRange range) const;

  //! Generate text with references from plain text using data positions for replacements
  /*
    Pre: 
    * length of \p normStr should be sufficient to replace all positions
    * subRange.start < size(normstr)
  */
  [[nodiscard]] std::string OutputRefs(const std::string& normStr) const;
  [[nodiscard]] std::string OutputRefs(const std::string& normStr, StrRange subRange) const;

private:
  using RefIter = std::vector<Reference>::iterator;
  void ResolveAll();
  void ResolveIt(RefIter target);
  [[nodiscard]] const Reference* FindMaster(RefIter base, int16_t offset) const;
  std::string GenerateResolved(std::string_view text);
};

template<typename RangeIterator>
bool RefsManager::UpdatePositions(RangeIterator nextPos) {
  auto refIt = std::begin(refs);
  auto position = nextPos(StrRange{ 0, 0 });
  for (; position.has_value(); position = nextPos(*position), ++refIt) {
    if (refIt == std::end(refs)) {
      return false;
    } else {
      refIt->position = position.value();
    }
  }
  return refIt == std::end(refs) && (!position.has_value() || !nextPos(position.value()).has_value());
}

} // namespace ccl::lang