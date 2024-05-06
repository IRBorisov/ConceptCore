#include "ccl/rslang/SDataCompact.h"

#include "ccl/rslang/RSToken.h"
#include "ccl/rslang/Typification.h"
#include "ccl/rslang/StructuredData.h"

#include <optional>

namespace ccl::object {

namespace {

using rslang::Typification;

//! Data packaging helper
class Packer {
  SDCompact::Data compact{ {} };

public:
  SDCompact::Data Pack(const StructuredData& theData, const Typification& type);

private:
  void CreateCompactFrom(const StructuredData& data, const Typification& type);
  void AddElementData(const StructuredData& element);
  void AddTupleData(const StructuredData& tuple, const Typification& type);
  void AddBoolData(const StructuredData& boolData, const Typification& type);
  void AddSet(const Typification& type, const StructuredData& setData);
  void AddEmpty(const Typification& type);
};

//! Data unpackaging helper
class Unpacker {
  const SDCompact::Data& input;

  size_t pos_x{ 0 };
  size_t pos_y{ 0 };

public:
  explicit Unpacker(const SDCompact::Data& input) noexcept
    : input(input) {}

public:
  std::optional<StructuredData> Unpack(const Typification& type);

private:
  std::optional<StructuredData> UnpackFor(const Typification& type);
  StructuredData UnpackBasic();
  std::optional<StructuredData> UnpackBool(const Typification& type);
  void SkipEmpty(const Typification& type);
  std::optional<StructuredData> UnpackTuple(const Typification& type);
  std::optional<StructuredData> UnpackSet(const Typification& type);

  [[nodiscard]] bool ReadElementInto(SDSet& result, const Typification& type);
};

} // anonymous namespace

SDCompact::Header SDCompact::CreateHeader(const Typification& type) {
  Header result{};
  const auto visitor = [&](const Typification& host) {
    switch (host.Structure()) {
    case rslang::StructureType::basic:
      result.push_back(host.E().baseID); break;

    case rslang::StructureType::collection:
      result.emplace_back("B"); break;

    case rslang::StructureType::tuple:
      break;
    }
  };
  type.ConstVisit(visitor);
  return result;
}

SDCompact SDCompact::FromSData(const StructuredData& sdata, const Typification& type) {
  return { CreateHeader(type), Packer{}.Pack(sdata, type) };
}

std::optional<StructuredData> SDCompact::Unpack(const Typification& type) const  {
  Unpacker unpacker(data);
  return unpacker.Unpack(type);
}

std::optional<StructuredData> SDCompact::Unpack(const Data& data, const Typification& type) {
  Unpacker unpacker(data);
  return unpacker.Unpack(type);
}

namespace {

SDCompact::Data Packer::Pack(const StructuredData& theData, const Typification& type) {
  CreateCompactFrom(theData, type);
  return compact;
}

void Packer::CreateCompactFrom(const StructuredData& packedData, const Typification& type) {
  switch (packedData.Structure()) {
  case rslang::StructureType::basic:
    AddElementData(packedData); break;
  case rslang::StructureType::collection:
    AddBoolData(packedData, type); break;
  case rslang::StructureType::tuple:
    AddTupleData(packedData, type); break;
  }
}

void Packer::AddElementData(const StructuredData& element) {
  compact.back().emplace_back(element.E().Value());
}

void Packer::AddTupleData(const StructuredData& tuple, const Typification& type) {
  for (auto index = Typification::PR_START; index < Typification::PR_START + tuple.T().Arity(); ++index) {
    CreateCompactFrom(tuple.T().Component(index), type.T().Component(index));
  }
}

void Packer::AddBoolData(const StructuredData& boolData, const Typification& type) {
  if (boolData.B().IsEmpty()) {
    AddEmpty(type);
  } else {
    AddSet(type, boolData);
  }
}

void Packer::AddSet(const Typification& type, const StructuredData& setData) {
  compact.back().emplace_back(setData.B().Cardinality());
  const std::vector<int32_t> baseRowCopy = compact.back();
  const auto& baseType = type.B().Base();
  for (const auto& element : setData.B()) {
    CreateCompactFrom(element, baseType);
    compact.emplace_back(baseRowCopy);
  }
  compact.pop_back();
}

void Packer::AddEmpty(const Typification& type) {
  assert(type.IsCollection());
  const auto visitor = [&](const Typification& host) {
    switch (host.Structure()) {
    case rslang::StructureType::basic:
    case rslang::StructureType::collection:
      compact.back().emplace_back(0); break;
    case rslang::StructureType::tuple: break;
    }
  };
  type.ConstVisit(visitor);
}

std::optional<StructuredData> Unpacker::Unpack(const Typification& type) {
  if (auto result = UnpackFor(type); !result.has_value()) {
    return std::nullopt;
  } else if (pos_x + 1 != size(input)) {
    return std::nullopt;
  } else {
    return result;
  }
}

std::optional<StructuredData> Unpacker::UnpackFor(const Typification& type) {
  if (size(input) <= pos_x || size(input.at(pos_x)) <= pos_y) {
    return std::nullopt;
  } else {
    switch (type.Structure()) {
    default:
    case rslang::StructureType::basic: return UnpackBasic();
    case rslang::StructureType::collection: return UnpackBool(type);
    case rslang::StructureType::tuple: return UnpackTuple(type);
    }
  }
}

StructuredData Unpacker::UnpackBasic() {
  auto result = Factory::Val(input.at(pos_x).at(pos_y));
  ++pos_y;
  return result;
}

std::optional<StructuredData> Unpacker::UnpackTuple(const Typification& type) {
  std::vector<StructuredData> factorsData{};
  const auto arity = type.T().Arity();
  for (rslang::Index i = 0; i < arity; ++i) {
    const auto& factor = type.T().Component(Typification::PR_START + i); // NOLINT
    if (auto factorVal = UnpackFor(factor); !factorVal.has_value()) {
      return std::nullopt;
    } else {
      factorsData.emplace_back(factorVal.value());
    }
  }
  return Factory::Tuple(factorsData);
}

std::optional<StructuredData> Unpacker::UnpackBool(const Typification& type) {
  if (input.at(pos_x).at(pos_y) == 0) {
    SkipEmpty(type);
    return Factory::EmptySet();
  } else {
    return UnpackSet(type);
  }
}

void Unpacker::SkipEmpty(const Typification& type) {
  const auto visitor = [&](const Typification& host) noexcept {
    switch (host.Structure()) {
    case rslang::StructureType::basic:
    case rslang::StructureType::collection:
      ++pos_y;
      break;
    case rslang::StructureType::tuple:
      break;
    }
  };
  type.ConstVisit(visitor);
}

bool Unpacker::ReadElementInto(SDSet& result, const Typification& type) {
  if (const auto element = UnpackFor(type);  !element.has_value()) {
    return false;
  } else {
    return result.AddElement(element.value());
  }
}

std::optional<StructuredData> Unpacker::UnpackSet(const Typification& type) {
  const auto base_y = pos_y;
  const auto& baseType = type.B().Base();
  auto result = Factory::EmptySet();
  auto& modifiableResult = result.ModifyB();

  if (auto count = input.at(pos_x).at(base_y); count != SDCompact::unknownCount) {
    for (; pos_x < size(input) && count > 0; ++pos_x, --count) {
      pos_y = base_y + 1;
      if (!ReadElementInto(modifiableResult, baseType)) {
        return std::nullopt;
      }
    }
    if (count == 0) {
      --pos_x;
      return result;
    } else {
      return std::nullopt;
    }
  } else {
    for (; pos_x < size(input); ++pos_x) {
      pos_y = base_y + 1;
      if (!ReadElementInto(modifiableResult, baseType)) {
        return std::nullopt;
      }
    }
    --pos_x;
    return result;
  }
}

} // anonymous namespace

} // namespace ccl::object
