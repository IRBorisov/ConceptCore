#include "ccl/rslang/StructuredData.h"

#include "SDImplementation.h"

namespace ccl::object {

Comparison SDBasicElement::Compare(const SDBasicElement &rhs) const noexcept {
  if (value == rhs.value) {
    return Comparison::EQUAL;
  } else if (value < rhs.value) {
    return Comparison::LESS;
  } else {
    return Comparison::GREATER;
  }
}

std::string SDBasicElement::ToString() const {
  return std::to_string(value);
}

SDTuple::SDTuple(const std::vector<StructuredData>& input) {
  auto index = rslang::Typification::PR_START;
  for (const auto& component : input) {
    components.emplace(index, component);
    ++index;
  }
}

rslang::Index SDTuple::Arity() const noexcept {
  return static_cast<rslang::Index>(ssize(components));
}

StructuredData SDTuple::Component(const rslang::Index index) const {
  assert(components.contains(index));
  return components.at(index);
}

Comparison SDTuple::Compare(const SDTuple& rhs) const {
  if (Arity() != rhs.Arity()) {
    return Comparison::INCOMPARABLE;
  } else {
    for (auto index = rslang::Typification::PR_START; index < Arity() + rslang::Typification::PR_START; ++index) {
      if (const auto res = components.at(index).Compare(rhs.Component(index)); res != Comparison::EQUAL) {
        return res;
      }
    }
    return Comparison::EQUAL;
  }
}

std::string SDTuple::ToString() const {
  std::string result{ '(' };
  const auto size = Arity();
  for (rslang::Index i = 0; i < size; ++i) {
    if (i > 0) {
      result += R"(, )";
    }
    const auto index = static_cast<rslang::Index>(rslang::Typification::PR_START + i);
    result += components.at(index).ToString();
  }
  result += ')';
  return result;
}

SDSet::~SDSet() noexcept = default;

SDSet::SDSet(const SetImpl& impl) 
  : impl{ impl.Clone() } {}

SDSet::SDSet(const SDSet& rhs) {
  *this = rhs;
}

SDSet& SDSet::operator=(const SDSet& rhs) {
  if (this != &rhs) {
    impl = rhs.impl->Clone();
  }
  return *this;
}

SDIterator SDSet::begin() const {
  return impl->begin();
}

SDIterator SDSet::end() const {
  return impl->end();
}

bool SDSet::Contains(const StructuredData& element) const {
  return impl->Contains(element);
}

Size SDSet::Cardinality() const noexcept {
  return impl->Cardinality();
}

bool SDSet::AddElement(StructuredData newElement) {
  return impl->AddElement(newElement);
}

bool SDSet::IsEmpty() const noexcept {
  return Cardinality() == 0;
}

Comparison SDSet::Compare(const SDSet& rhs) const {
  if (Cardinality() > rhs.Cardinality()) {
    return Comparison::GREATER;
  } else if (Cardinality() < rhs.Cardinality()) {
    return Comparison::LESS;
  } else {
    auto iter2 = rhs.begin();
    for (auto iter = begin(); iter != end(); ++iter, ++iter2) {
      if (const auto res = iter->Compare(*iter2);
          res != Comparison::EQUAL) {
        return res;
      }
    }
    return Comparison::EQUAL;
  }
}

std::string SDSet::ToString() const {
  std::string result{ '{' };
  const auto startI = begin();
  for (auto iter = begin(); iter != end(); ++iter) {
    if (iter != startI) {
      result += R"(, )";
    }
    result += iter->ToString();
  }
  result += '}';
  return result;
}

bool SDSet::IsSubsetOrEq(const SDSet& rhs) const {
  return std::all_of(std::begin(*this), std::end(*this),
    [&](const auto& element) { return rhs.Contains(element); });
}

StructuredData SDSet::Union(const SDSet& rhs) const {
  auto result = Factory::EmptySet();
  for (const auto& thisElement : *this) {
    result.ModifyB().AddElement(thisElement);
  }
  for (const auto& secondElement : rhs) {
    result.ModifyB().AddElement(secondElement);
  }
  return result;
}

StructuredData SDSet::Intersect(const SDSet& rhs) const {
  auto result = Factory::EmptySet();
  for (const auto& secondElement : rhs) {
    if (this->Contains(secondElement)) {
      result.ModifyB().AddElement(secondElement);
    }
  }
  return result;
}

StructuredData SDSet::Diff(const SDSet& rhs) const {
  auto result = Factory::EmptySet();
  for (const auto& thisElement : *this) {
    if (!rhs.Contains(thisElement)) {
      result.ModifyB().AddElement(thisElement);
    }
  }
  return result;
}

StructuredData SDSet::SymDiff(const SDSet& rhs) const {
  auto result = Factory::EmptySet();
  for (const auto& thisElement : *this) {
    if (!rhs.Contains(thisElement)) {
      result.ModifyB().AddElement(thisElement);
    }
  }
  for (const auto& secondElement : rhs) {
    if (!Contains(secondElement)) {
      result.ModifyB().AddElement(secondElement);
    }
  }
  return result;
}

StructuredData SDSet::Projection(const std::vector<rslang::Index>& indicies) const {
  auto result = Factory::EmptySet();
  for (const auto& thisElement : *this) {
    std::vector<StructuredData> components{};
    components.reserve(size(indicies));
    for (const auto index : indicies) {
      components.emplace_back(thisElement.T().Component(index));
    }
    result.ModifyB().AddElement(Factory::Tuple(components));
  }
  return result;
}

StructuredData SDSet::Reduce() const {
  auto result = Factory::EmptySet();
  for (const auto& thisElement : *this) {
    assert(thisElement.IsCollection());
    for (const auto& subElement : thisElement.B()) {
      result.ModifyB().AddElement(subElement);
    }
  }
  return result;
}

StructuredData SDSet::Debool() const {
  assert(Cardinality() == 1);
  return *begin();
}

StructuredData Factory::EmptySet() noexcept {
  return StructuredData::Impl{ SDEnumSet{} };
}

StructuredData::StructuredData() noexcept {
  *this = Factory::EmptySet();
}

StructuredData::StructuredData(const Impl& impl)
  : data(std::make_shared<Impl>(impl)) {}

StructuredData Factory::Val(const DataID value) {
  return StructuredData::Impl{ SDBasicElement{ value } };
}

std::vector<StructuredData> Factory::Vals(const std::vector<DataID>& vals) {
  std::vector<StructuredData> result{};
  result.reserve(size(vals));
  for (const auto& value : vals) {
    result.emplace_back(Val(value));
  }
  return result;
}

StructuredData Factory::Tuple(const std::vector<StructuredData>& components) {
  assert(size(components) > 0);
  if (size(components) > 1) {
    return StructuredData::Impl{ SDTuple(components) };
  } else {
    return *begin(components);
  }
}

StructuredData Factory::TupleV(const std::vector<DataID>& components) {
  return Tuple(Vals(components));
}

StructuredData Factory::Set(const std::vector<StructuredData>& elements) {
  auto result = EmptySet();
  for (const auto& element : elements) {
    result.ModifyB().AddElement(element);
  }
  return result;
}

StructuredData Factory::SetV(const std::vector<DataID>& vals) {
  return Set(Vals(vals));
}

StructuredData Factory::Singleton(StructuredData base) {
  auto result = EmptySet();
  result.ModifyB().AddElement(base);
  return result;
}

StructuredData Factory::Decartian(const std::vector<StructuredData>& factors) {
  if (std::any_of(begin(factors), end(factors),
    [](const auto& factor) noexcept { return factor.B().IsEmpty(); })) {
    return EmptySet();
  } else {
    return StructuredData::Impl{ SDDecartian(factors) };
  }
}

StructuredData Factory::Boolean(StructuredData base) {
  assert(base.IsCollection());
  return StructuredData::Impl{ SDPowerSet(base) };
}

rslang::StructureType StructuredData::Structure() const noexcept {
  return data->Structure();
}

bool StructuredData::IsElement() const noexcept {
  return data->IsElement();
}

bool StructuredData::IsTuple() const noexcept  {
  return data->IsTuple(); }

bool StructuredData::IsCollection() const noexcept {
  return data->IsCollection();
}

const SDBasicElement& StructuredData::E() const noexcept {
  return data->E();
}

const SDTuple& StructuredData::T() const noexcept {
  return data->T();
}

const SDSet& StructuredData::B() const noexcept {
  return data->B();
}

SDSet& StructuredData::ModifyB() {
  return UniqueData().B();
}

StructuredData::Impl& StructuredData::UniqueData() {
  if (data.use_count() > 1) {
    data = std::make_shared<Impl>(data->B()); // Note: used only for Set items
  }
  return *data;
}

bool StructuredData::operator==(const StructuredData& rhs) const {
  return Compare(rhs) == Comparison::EQUAL;
}

bool StructuredData::operator<(const StructuredData& rhs) const {
  return data != rhs.data && Compare(rhs) == Comparison::LESS;
}

Comparison StructuredData::Compare(const StructuredData& rhs) const {
  if (data == rhs.data) {
    return Comparison::EQUAL;
  } else if (data->Structure() != rhs.Structure()) {
    return Comparison::INCOMPARABLE;
  } else {
    switch (data->Structure()) {
    default:
    case rslang::StructureType::basic:
      return data->E().Compare(rhs.E());
    case rslang::StructureType::tuple:
      return data->T().Compare(rhs.T());
    case rslang::StructureType::collection:
      return data->B().Compare(rhs.B());
    }
  }
}

std::string StructuredData::ToString() const {
  switch (data->Structure()) {
  default:
  case rslang::StructureType::basic:
    return data->E().ToString();
  case rslang::StructureType::tuple:
    return data->T().ToString();
  case rslang::StructureType::collection:
    return data->B().ToString();
  }
}

bool CheckCompatible(const StructuredData& data, const rslang::Typification& type) {
  if (type.Structure() != data.Structure()) {
    return false;
  } else {
    switch (type.Structure()) {
    default:
    case rslang::StructureType::basic: {
      return true;
    }
    case rslang::StructureType::collection: {
      if (data.B().IsEmpty()) {
        return true;
      } else {
        return CheckCompatible(*std::begin(data.B()), type.B().Base());
      }
    }
    case rslang::StructureType::tuple:
      if (data.T().Arity() != type.T().Arity()) {
        return false;
      } else {
        for (auto i = rslang::Typification::PR_START; i < data.T().Arity() + rslang::Typification::PR_START; ++i) {
          if (!CheckCompatible(data.T().Component(i), type.T().Component(i))) {
            return false;
          }
        }
        return true;
      }
    }
  }
}

} // namespace ccl::object