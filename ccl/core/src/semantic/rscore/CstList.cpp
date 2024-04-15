#include "ccl/semantic/CstList.h"

#include "ccl/semantic/RSCore.h"

namespace ccl::semantic {

namespace {

[[nodiscard]] bool HasPriorityOver(const CstType strong, const CstType weak) {
  static const std::array<int8_t, static_cast<size_t>(CstType::size_)>
    priorities{ 0, 4, 3, 0, 2, 1, 1, 1, 1, 1 };
  return priorities.at(static_cast<size_t>(strong)) > priorities.at(static_cast<size_t>(weak));
}

} // anonymous namespace

CstList::CstList(const CstList& rhs) = default;

CstList& CstList::operator=(const CstList& rhs) {
  if (&rhs != this) {
    order = rhs.order;
  }
  return *this;
}

CstList::CstList(CstList&& rhs) noexcept 
  : order{ std::move(rhs.order) }, types{ std::move(rhs.types) } {}

CstList& CstList::operator=(CstList&& rhs) noexcept {
  if (&rhs != this) {
    order = std::move(rhs.order);
  }
  return *this;
}

void CstList::Erase(const EntityUID target) {
  for (auto it = Find(target); it != end(); it = Find(target)) {
    order.erase(it.it);
  }
}

ListIterator CstList::begin() const noexcept {
  return ListIterator{ this, std::begin(order) };
}

ListIterator CstList::end() const noexcept {
  return ListIterator{ this, std::end(order) };
}

CstList::Container::size_type CstList::size() const noexcept {
  return std::size(order);
}

ListIterator CstList::Find(const EntityUID entity) const {
  return ListIterator{ this, std::find(std::begin(order), std::end(order), entity) };
}

void CstList::Insert(const EntityUID newID) {
  const auto insPos = InsertPositionFor(types(newID));
  order.insert(insPos.it, newID);
}

ListIterator CstList::InsertPositionFor(const CstType insertType) const {
  auto result = end();
  if (IsBasic(insertType)) {
    for (auto it = begin(); it != end(); ++it) {
      if (types(*it) <= insertType) {
        result = it;
      }
    }
    if (result != end()) {
      ++result;
    } else if (!std::empty(order) && types(*begin()) > insertType) {
      return begin();
    }
  }
  return result;
}

bool CstList::CanMoveBefore(const ListIterator what, ListIterator iWhere) const {
  if (what == iWhere) {
    return true;
  } else if (iWhere == end()) {
    return CanMoveBefore(--iWhere, what);
  } else if (iWhere == begin()) {
    return !HasPriorityOver(types(*iWhere), types(*what));
  } else {
    ListIterator prev = iWhere;
    --prev;
    return !HasPriorityOver(types(*iWhere), types(*what)) &&
      !HasPriorityOver(types(*what), types(*prev));
  }
}

bool CstList::MoveBefore(const EntityUID what, const ListIterator iWhere)  {
  if (const auto iWhat = Find(what); iWhat == end()) {
    return false;
  } else if (CanMoveBefore(iWhat, iWhere)) {
    order.splice(iWhere.it, order, iWhat.it);
    return true;
  } else {
    return false;
  }
}

VectorOfEntities CstList::SortSubset(const SetOfEntities& subset) const {
  if (std::empty(subset)) {
    return VectorOfEntities{};
  } else if (ssize(subset) == 1) {
    return { *std::begin(subset) };
  } else {
    VectorOfEntities result{};
    std::for_each(begin(), end(), [&result, &subset](const EntityUID& entity) {
      if (subset.contains(entity)) {
        result.emplace_back(entity);
      }
    });
    return result;
  }
}

ListIterator::ListIterator(const CstList* container, Container::const_iterator iter) noexcept
  : container{ container }, it{ std::move(iter) } {}

ListIterator& ListIterator::operator++() noexcept {
  ++it;
  return *this;
}

ListIterator& ListIterator::operator--() noexcept {
  --it;
  return *this;
}

ListIterator::pointer ListIterator::operator->() const noexcept {
  return &*it;
}

ListIterator::reference ListIterator::operator*() const noexcept {
  return *it;
}

bool ListIterator::operator==(const ListIterator& iter2) const noexcept {
  return container == iter2.container && it == iter2.it;
}

} // namespace ccl::semantic
