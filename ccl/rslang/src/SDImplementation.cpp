#include "SDImplementation.h"

#include "ccl/rslang/StructuredData.h"

#include <sstream>

namespace ccl::object {

SDSetPtr SDEnumSet::Clone() const {
  return std::make_unique<SDEnumSet>(*this);
}

bool SDEnumSet::Contains(StructuredData element) const {
  return elements.contains(element);
}

bool SDEnumSet::AddElement(StructuredData newElement) {
  return elements.insert(newElement).second;
}

bool CachedSD::IsCached(const CacheIndex index) const {
  return cachedElements.contains(index);
}

const StructuredData& CachedSD::GetCache(const CacheIndex index) const  {
  return cachedElements.at(index);
}

const StructuredData& CachedSD::SaveCache(const CacheIndex index, StructuredData value) const {
  static constexpr CacheIndex cacheLimit = 100U;
  if (size(cachedElements) >= cacheLimit) {
    cachedElements.clear();
  }
  cachedElements[index] = std::move(value);
  return cachedElements.at(index);
}

SDPowerSet::Iterator::Iterator(const SDPowerSet& boolean, const bool isCompleted) noexcept
  : boolean{ &boolean }, isCompleted{ isCompleted } {}

SDPowerSet::Iterator::reference SDPowerSet::Iterator::operator*() const {
  if (boolean->IsCached(counter)) {
    return boolean->GetCache(counter);
  } else {
    auto newData = Factory::EmptySet();
    for (const auto& iter : itemIterators) {
      newData.ModifyB().AddElement(*iter);
    }
    return boolean->SaveCache(counter, newData);
  }
}

bool SDPowerSet::Iterator::operator==(const Iterator& rhs) const noexcept {
  if (isCompleted && rhs.isCompleted) {
    return true;
  } else if (rhs.isCompleted) {
    return false;
  } else {
    return counter == rhs.counter;
  }
}

SDPowerSet::Iterator& SDPowerSet::Iterator::operator++() {
  if (!isCompleted) {
    if (Increment()) {
      ++counter;
    } else {
      isCompleted = true;
    }
  }
  return *this;
}

bool SDPowerSet::Iterator::Increment() {
  const auto baseCard = boolean->base.B().Cardinality();
  if (empty(itemIterators)) {
    if (baseCard != 0) {
      itemIterators.emplace_back(std::begin(boolean->base.B()));
    }
    return baseCard != 0;
  } else {
    auto offset = 0;
    while (!empty(itemIterators)) {
      if (IncrementLastItem(offset)) {
        return true;
      } else {
        ++offset;
        itemIterators.pop_back();
      }
    }

    if (offset != baseCard) {
      auto it = std::begin(boolean->base.B());
      for (auto i = 0; i < offset + 1; ++i, ++it) {
        itemIterators.emplace_back(it);
      }
      return true;
    } else {
      return false;
    }
  }
}

bool SDPowerSet::Iterator::IncrementLastItem(const Size offset) {
  SDIterator test = itemIterators.back();
  for (auto testcount = offset + 1; testcount > 0; --testcount) {
    ++test;
    if (test == std::end(boolean->base.B())) {
      return false;
    }
  }
  ++itemIterators.back();
  SDIterator it = itemIterators.back();
  for (auto addCount = offset; addCount > 0; --addCount) {
    ++it;
    itemIterators.emplace_back(it);
  }
  return true;
}

SDPowerSet::SDPowerSet(StructuredData base)  
  : base{ std::move(base) } {
  UpdateSize(); 
}

void SDPowerSet::UpdateSize() noexcept {
  if (const auto baseSize = base.B().Cardinality();  baseSize > StructuredData::BOOL_INFINITY) {
    size = StructuredData::SET_INFINITY;
  } else {
    size = static_cast<Size>(static_cast<size_t>(1U) << static_cast<size_t>(baseSize));
  }
}

SDSetPtr SDPowerSet::Clone() const {
  return std::make_unique<SDPowerSet>(base);
}

SDIterator SDPowerSet::begin() const {
  return SDIterator{ Iterator(*this) };
}

SDIterator SDPowerSet::end() const {
  return SDIterator{ Iterator(*this, true) };
}

bool SDPowerSet::Contains(StructuredData element) const {
  return element.B().IsSubsetOrEq(base.B());
}

SDDecartian::Iterator::Iterator(const SDDecartian& decart, const bool completed)
  : decartian{ &decart }, isCompleted{ completed || decartian->Cardinality() == 0 } {
  if (!isCompleted) {
    componentIters.reserve(size(decartian->factors));
    for (const auto& factor : decartian->factors) {
      componentIters.emplace_back(std::begin(factor.B()));
    }
  }
}

SDDecartian::Iterator::reference SDDecartian::Iterator::operator*() const {
  if (decartian->IsCached(counter)) {
    return decartian->GetCache(counter);
  } else {
    std::vector<StructuredData> components{};
    components.reserve(size(componentIters));
    for (const auto& compIter : componentIters) {
      components.emplace_back(*compIter);
    }
    return decartian->SaveCache(counter, Factory::Tuple(components));
  }
}

bool SDDecartian::Iterator::operator==(const Iterator& rhs) const noexcept {
  if (isCompleted && rhs.isCompleted) {
    return true;
  } else if (rhs.isCompleted) {
    return false;
  } else {
    return counter == rhs.counter;
  }
}

SDDecartian::Iterator& SDDecartian::Iterator::operator++() {
  if (!isCompleted) {
    auto index = size(componentIters) - 1;
    for (auto iter = componentIters.rbegin(); iter != componentIters.rend(); ++iter, --index) {
      if (IncrementComponent(*iter, index)) {
        ++counter;
        return *this;
      }
    }
    isCompleted = true;
  }
  return *this;
}

bool SDDecartian::Iterator::IncrementComponent(SDIterator& component, const size_t index) {
  ++component;
  if (component == std::end(decartian->factors.at(index).B())) {
    component = std::begin(decartian->factors.at(index).B());
    return false;
  } else {
    return true;
  }
}

SDDecartian::SDDecartian(std::vector<StructuredData> factors) noexcept // NOLINT(bugprone-exception-escape)
  : factors{ std::move(factors) } {
  assert(ssize(this->factors) >= 2);
  UpdateSize();
}

void SDDecartian::UpdateSize() noexcept {
  count = 1;
  for (const auto& factor : factors) {
    const auto factorSize = factor.B().Cardinality();
    if (StructuredData::SET_INFINITY / factorSize > count) {
      count *= factorSize;
    } else {
      count = StructuredData::SET_INFINITY;
    }
  }
}

SDIterator SDDecartian::begin() const {
  return SDIterator{ Iterator(*this) };
}

SDIterator SDDecartian::end() const {
  return SDIterator{ Iterator(*this, true) };
}

SDSetPtr SDDecartian::Clone() const {
  return std::make_unique<SDDecartian>(factors);
}

bool SDDecartian::Contains(const StructuredData element) const {
  if (element.T().Arity() != ssize(factors)) {
    return false;
  } else {
    for (size_t i = 0; i < size(factors); ++i) {
      const auto index = static_cast<rslang::Index>(i + rslang::Typification::PR_START);
      if (!factors.at(i).B().Contains(element.T().Component(index))) {
        return false;
      }
    }
    return true;
  }
}

} // namespace ccl::object