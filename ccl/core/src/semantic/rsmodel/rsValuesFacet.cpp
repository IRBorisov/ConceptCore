#include "ccl/semantic/rsValuesFacet.h"

#include "ccl/semantic/RSModel.h"

namespace ccl::semantic {

rsValuesFacet::rsValuesFacet(RSModel& core) 
  : MutatorFacet<RSModel>(core) {
  storage = std::make_unique<InterpretationStorage>();
}

void rsValuesFacet::Erase(const EntityUID target) noexcept  {
  storage->Erase(target);
  statements.erase(target);
}

std::optional<object::StructuredData> rsValuesFacet::SDataFor(const EntityUID entity) const {
  return storage->GetRSIFor(entity);
}

void rsValuesFacet::ResetDataFor(const EntityUID target) {
  if (!core.Contains(target)) {
    return;
  } else if (IsBaseNotion(core.GetRS(target).type)) {
    ResetFor(target);
    core.ResetDependants(target);
    core.NotifyModification();
  }
}

bool rsValuesFacet::SetStructureData(const EntityUID target, const object::StructuredData input) {
  if (!core.Contains(target)) {
    return false;
  } else if (core.GetRS(target).type != CstType::structured) {
    return false;
  } else if (const auto* typif = core.GetParse(target).Typification(); typif == nullptr) {
    return false;
  } else if (!CheckCompatible(input, *typif)) {
    return false;
  } else if (!CheckBasicElements(input, *typif)) {
    return false;
  } else if (!SetRSInternal(target, input)) {
    return false;
  } else {
    core.ResetDependants(target);
    core.NotifyModification();
    return true;
  }
}

bool rsValuesFacet::CheckBasicElements(const object::StructuredData& data, const rslang::Typification& type) const {
  switch (data.Structure()) {
  default:
  case ccl::rslang::StructureType::basic: {
    return type == rslang::Typification::Integer() ||
      TextFor(core.Core().FindAlias(type.E().baseID).value())->HasInterpretantFor(data.E().Value()); // NOLINT(bugprone-unchecked-optional-access)
  }
  case ccl::rslang::StructureType::collection: {
    if (!data.B().IsEmpty()) {
      for (const auto& child : data.B()) {
        if (!CheckBasicElements(child, type.B().Base())) {
          return false;
        }
      }
    }
    return true;
  }
  case ccl::rslang::StructureType::tuple: {
    for (auto i = rslang::Typification::PR_START; i < data.T().Arity() + rslang::Typification::PR_START; ++i) {
      if (!CheckBasicElements(data.T().Component(i), type.T().Component(i))) {
        return false;
      }
    }
    return true;
  }
  }
}

void rsValuesFacet::LoadData(const EntityUID target, object::StructuredData input) {
  SetRSInternal(target, input);
}

void rsValuesFacet::LoadData(const EntityUID target, bool logicVal) {
  SetStatementInternal(target, logicVal);
}

void rsValuesFacet::LoadData(const EntityUID target, const TextInterpretation &newInterp) {
  SetTextInternal(target, newInterp);
}

bool rsValuesFacet::SetRSInternal(const EntityUID target, const object::StructuredData& input) {
  if (const auto oldData = SDataFor(target);
      !oldData.has_value() || oldData.value() != input) {
    storage->SetRSInterpretationFor(target, input);
    return true;
  } else {
    return false;
  }
}

const TextInterpretation* rsValuesFacet::TextFor(const EntityUID entity) const  {
  return storage->GetTextInterpretationFor(entity);
}

bool rsValuesFacet::SetBasicText(const EntityUID target, const TextInterpretation& newInterp) {
  if (!core.Contains(target)) {
    return false;
  } else if (!IsBaseSet(core.GetRS(target).type)) {
    return false;
  } else {
    const auto dataChange = std::ssize(newInterp) != std::ssize(*TextFor(target));
    if (!SetTextInternal(target, newInterp)) {
      return false;
    } else {
      if (dataChange) {
        core.ResetDependants(target);
      }
      core.NotifyModification();
      return true;
    }
  }
}

bool rsValuesFacet::SetTextInternal(const EntityUID target, const TextInterpretation& newInterp) {
  if (newInterp == *TextFor(target)) {
    return false;
  } else {
    storage->SetTextInterpretationFor(target, newInterp);
    return true;
  }
}

std::optional<int32_t> 
rsValuesFacet::AddBasicElement(const EntityUID target, const std::string& name) {
  if (!core.Contains(target)) {
    return std::nullopt;
  } else if (!IsBaseSet(core.GetRS(target).type)) {
    return std::nullopt;
  } else {
    const auto resultID = storage->AddInterpretantFor(target, name);
    core.ResetDependants(target);
    core.NotifyModification();
    return resultID;
  }
}

void rsValuesFacet::ResetFor(const EntityUID target) {
  statements.erase(target);
  storage->Erase(target);
  const auto type = core.GetRS(target).type;
  if (IsBaseSet(type)) {
    storage->SetRSInterpretationFor(target, object::Factory::EmptySet());
    storage->SetTextInterpretationFor(target, TextInterpretation{});
    return;
  } else if (type != CstType::structured) {
    return;
  }
  const auto& parseInfo = core.GetParse(target);
  if (parseInfo.status != ParsingStatus::VERIFIED || !parseInfo.exprType.has_value()) {
    return;
  }
  const auto& typif = std::get<rslang::Typification>(parseInfo.exprType.value());
  if (typif.IsCollection()) {
    storage->SetRSInterpretationFor(target, object::Factory::EmptySet());
  }
}

void rsValuesFacet::PruneStructure(const EntityUID target) {
  const auto oldData = SDataFor(target);
  if (!oldData.has_value()) {
    return;
  } 
  const auto& typeValue = core.GetParse(target).exprType;
  assert(typeValue.has_value());
  const auto& type = std::get<rslang::Typification>(typeValue.value()); // NOLINT(bugprone-exception-escape)
  if (!oldData->IsCollection()) {
    if (!CheckBasicElements(oldData.value(), type)) {
      storage->Erase(target);
    }
  } else {
    auto newData = object::Factory::EmptySet();
    for (const auto& element : oldData->B()) {
      if (CheckBasicElements(element, type.B().Base())) {
        newData.ModifyB().AddElement(element);
      }
    }
    storage->SetRSInterpretationFor(target, newData);
  }
}

void rsValuesFacet::ResetAll() {
  for (const auto uid : core.Core()) {
    ResetFor(uid);
  }
}

void rsValuesFacet::ResetAllExceptCore() {
  for (const auto uid : core.Core()) {
    if (!IsBaseNotion(core.GetRS(uid).type)) {
      ResetFor(uid);
    }
  }
}

std::optional<bool> rsValuesFacet::StatementFor(const EntityUID entity) const {
  if (statements.contains(entity)) {
    return statements.at(entity);
  } else {
    return std::nullopt;
  }
}

bool rsValuesFacet::SetStatementInternal(const EntityUID target, bool logicVal) {
  if (const auto oldVal = StatementFor(target);
      !oldVal.has_value() || oldVal.value() != logicVal) {
    statements[target] = logicVal;
    return true;
  } else {
    return false;
  }
}

} // namespace ccl::semantic
