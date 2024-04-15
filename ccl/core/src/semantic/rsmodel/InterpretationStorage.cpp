#include "ccl/semantic/InterpretationStorage.h"

namespace ccl::semantic {

void InterpretationStorage::Clear() noexcept {
  textData.clear();
  rsData.clear();
}

void InterpretationStorage::Erase(const EntityUID target) noexcept {
  rsData.erase(target);
  textData.erase(target);
}

const TextInterpretation* InterpretationStorage::GetTextInterpretationFor(const EntityUID uid) const {
  if (!textData.contains(uid)) {
    return nullptr;
  } else {
    return &textData.at(uid);
  }
}

int32_t InterpretationStorage::AddInterpretantFor(const EntityUID uid, const std::string& name) {
  const auto resultIndex = textData[uid].PushBack(name);
  auto newData = !rsData.contains(uid) ? object::Factory::EmptySet() : rsData.at(uid);
  newData.ModifyB().AddElement(object::Factory::Val(resultIndex));
  rsData[uid] = newData;
  return resultIndex;
}

void InterpretationStorage::SetTextInterpretationFor(const EntityUID uid, const TextInterpretation& newInterp) {
  textData[uid] = newInterp;

  auto newData = object::Factory::EmptySet();
  for (const auto& it : newInterp) {
    newData.ModifyB().AddElement(object::Factory::Val(it.first));
  }
  rsData[uid] = newData;
}

std::optional<object::StructuredData> InterpretationStorage::GetRSIFor(const EntityUID uid) const {
  if (!rsData.contains(uid)) {
    return std::nullopt;
  } else {
    return rsData.at(uid);
  }
}

void InterpretationStorage::SetRSInterpretationFor(const EntityUID uid, object::StructuredData val) {
  rsData[uid] = std::move(val);
}

} // namespace ccl::semantic