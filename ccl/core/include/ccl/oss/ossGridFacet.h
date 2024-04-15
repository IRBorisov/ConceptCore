#pragma once

#include "ccl/oss/Pict.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::oss {

class OSSchema;

using GridIndex = int32_t; // TODO: change to int16_t after serialization fix

//! Decartian grid position
struct GridPosition {
  GridIndex row{ 0 };
  GridIndex column{ 0 };

  constexpr GridPosition() = default;

  constexpr GridPosition(GridIndex row, GridIndex column) noexcept
    : row{ row }, column{ column } {}

  bool operator==(const GridPosition& other) const noexcept {
    return row == other.row && column == other.column;
  }
  bool operator!=(const GridPosition& other) const noexcept {
    return row != other.row || column != other.column;
  }

  [[nodiscard]] constexpr double EffectiveColumn() const {
    return column + row / 2.0; // NOLINT(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
  }
};

} // namespace ccl::oss

namespace std {
  template <>
  struct hash<ccl::oss::GridPosition> {
    uint32_t operator()(const ccl::oss::GridPosition& k) const noexcept {
      using std::hash;
      return static_cast<uint32_t>(hash<int32_t>()(k.row) ^ (hash<int32_t>()(k.column) << 1U));
    }
  };
} // namespace std

namespace ccl::oss {

using Grid = std::unordered_map<GridPosition, PictID>;

//! OSS grid positions facet
class ossGridFacet final : public meta::MutatorFacet<OSSchema> {
  friend class OSSchema;

  Grid grid{};

public:
  static constexpr GridPosition origin{ 0, 0 };

public:
  explicit ossGridFacet(OSSchema& oss) noexcept // NOLINT(bugprone-exception-escape)
    : MutatorFacet<OSSchema>(oss) {}

public:
  [[nodiscard]] const Grid& data() const noexcept;

  [[nodiscard]] std::optional<PictID> operator()(GridPosition pos) const;
  [[nodiscard]] std::optional<GridPosition> operator()(PictID pid) const;

  [[nodiscard]] GridIndex MaxColumnIn(GridIndex row) const;
  [[nodiscard]] GridIndex CountRows() const;

  bool ShiftPict(PictID pid, int32_t shift);
  void LoadPosition(PictID pid, GridPosition pos);

private:
  [[nodiscard]] GridPosition ChildPosFor(PictID parent1, PictID parent2) const;
  [[nodiscard]] GridPosition ClosestFreePos(GridPosition start = origin) const;

  void SetPosFor(PictID pid, GridPosition pos);
  void Erase(PictID pid);
};

} // namespace ccl::oss