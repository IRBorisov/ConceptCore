#pragma once

#include <list>

#include "ccl/Entity.hpp"
#include "ccl/semantic//CstType.hpp"

namespace ccl::semantic {

class ListIterator;

//! Constituents ordering facet for RSCore
class CstList {
  friend class ListIterator;

public:
  using TypeSource = std::function<CstType(EntityUID)>;

private:
  using Container = std::list<EntityUID>;
  Container order{};
  TypeSource types;

public:
  explicit CstList(TypeSource types)
    : types{ std::move(types) } {}

  CstList(const CstList& rhs);
  CstList& operator=(const CstList& rhs);
  CstList(CstList&& rhs) noexcept;
  CstList& operator=(CstList&& rhs) noexcept;

public:
  using value_type = Container::value_type;
  using size_type = Container::size_type;

  [[nodiscard]] ListIterator begin() const noexcept;
  [[nodiscard]] ListIterator end() const noexcept;
  [[nodiscard]] size_type size() const noexcept;
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }

  [[nodiscard]] ListIterator Find(EntityUID entity) const;

  void Insert(EntityUID newID);
  void Erase(EntityUID target);
  bool MoveBefore(EntityUID what, ListIterator iWhere);

  [[nodiscard]] VectorOfEntities SortSubset(const SetOfEntities& subset) const;

private:
  [[nodiscard]] ListIterator InsertPositionFor(CstType insertType) const;
  [[nodiscard]] bool CanMoveBefore(ListIterator what, ListIterator iWhere) const;
};

//! Constituents list iterator
class ListIterator {
  friend class CstList;
  using Container = CstList::Container;

  const CstList* container{ nullptr };
  Container::const_iterator it{};

private:
  explicit ListIterator(const CstList* container, Container::const_iterator iter) noexcept;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = EntityUID;
  using difference_type = std::ptrdiff_t;
  using pointer = const EntityUID*;
  using reference = const EntityUID&;

  [[nodiscard]] bool operator==(const ListIterator& iter2) const noexcept;
  [[nodiscard]] bool operator!=(const ListIterator& iter2) const noexcept { return !(*this == iter2); }

  ListIterator& operator++() noexcept;
  ListIterator& operator--() noexcept;

  [[nodiscard]] pointer operator->() const noexcept;
  [[nodiscard]] reference operator*() const noexcept;
};

} // namespace ccl::semantic
