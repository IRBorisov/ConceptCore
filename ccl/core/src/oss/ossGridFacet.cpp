#include "ccl/oss/ossGridFacet.h"

#include "ccl/oss/OSSchema.h"

namespace ccl::oss {

const Grid& ossGridFacet::data() const noexcept { return grid; }

GridPosition ossGridFacet::ClosestFreePos(GridPosition start) const {
  auto searchRight = start;
  auto searchLeft = start;
  while (true) {
    if (!grid.contains(searchLeft)) {
      return searchLeft;
    } else if (!grid.contains(searchRight)) {
      return searchRight;
    } else {
      ++searchRight.column;
      searchLeft.column = searchLeft.column == 0 ? 0 : searchLeft.column + 1;
    }
  }
}

std::optional<PictID> ossGridFacet::operator()(GridPosition pos) const {
  if (!grid.contains(pos)) {
    return std::nullopt;
  } else {
    return grid.at(pos);
  }
}

std::optional<GridPosition> ossGridFacet::operator()(const PictID pid) const {
  for (const auto& [position, pict] : grid) {
    if (pict == pid) {
      return position;
    }
  }
  return std::nullopt;
}

GridPosition ossGridFacet::ChildPosFor(const PictID parent1, const PictID parent2) const {
  const auto pos1 = operator()(parent1);
  const auto pos2 = operator()(parent2);
  assert(pos1.has_value() && pos2.has_value());
  GridPosition newPos{};
  newPos.row = std::max(pos1->row, pos2->row) + 1; // NOLINT
  newPos.column = static_cast<GridIndex>
    (std::max(0L, std::lround((pos1->EffectiveColumn() + pos2->EffectiveColumn() - newPos.row) / 2.0))); // NOLINT
  return ClosestFreePos(newPos);
}

void ossGridFacet::SetPosFor(const PictID pid, GridPosition pos) {
  Erase(pid);
  grid[pos] = pid;
}

bool ossGridFacet::ShiftPict(const PictID pid, int32_t shift) {
  if (!core.Contains(pid)) {
    return false;
  } else if (shift == 0) {
    return true;
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access): false positive
  } else if (const auto curPos = operator()(pid).value(); curPos.column + shift < 0) {
      return false;
  } else {
    auto newPos = curPos;
    newPos.column += shift;
    if (!grid.contains(newPos)) {
      grid.erase(curPos);
      grid[newPos] = pid;
    } else {
      std::swap(grid[curPos], grid[newPos]);
    }
    core.NotifyModification();
    return true;
  }
}

void ossGridFacet::LoadPosition(PictID pid, GridPosition pos) {
  SetPosFor(pid, pos);
}

void ossGridFacet::Erase(const PictID pid) {
  if (auto pos = operator()(pid); pos.has_value()) {
    grid.erase(pos.value());
  }
}

GridIndex ossGridFacet::MaxColumnIn(GridIndex row) const {
  return 1 + std::accumulate(begin(grid), end(grid), -1,
    [&](int32_t maxColumn, const auto& cell) {
      if (cell.first.row == row) {
        return std::max(cell.first.column, maxColumn);
      } else {
        return maxColumn;
      }
    }
  );
}

GridIndex ossGridFacet::CountRows() const {
  return 1 + std::accumulate(begin(grid), end(grid), -1,
    [&](int32_t maxRow, const auto& cell) {
      return std::max(cell.first.row, maxRow);
    }
  );
}

} // namespace ccl::oss