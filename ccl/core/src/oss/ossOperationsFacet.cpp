#include "ccl/oss/ossOperationsFacet.h"

#include "ccl/oss/OSSchema.h"
#include "ccl/ops/RSOperations.h"
#include "ccl/oss/RSSynthesProcessor.h"

namespace ccl::oss {

void OperationHandle::Reset() noexcept {
  options = nullptr;
  translations = nullptr;
  type = ops::Type::tba;
  broken = false;
  outdated = false;
}

void OperationHandle::InitOperation(ops::Type operation, meta::UniqueCPPtr<ops::Options> newOptions) noexcept {
  Reset();
  type = operation;
  translations = nullptr;
  options = std::move(newOptions);
}

ossOperationsFacet::ossOperationsFacet(OSSchema& oss) 
  : MutatorFacet<OSSchema>(oss) {
  auto& proc = procs.emplace_back(std::make_unique<RSSProcessor>());
  opProcs[ops::Type::rsMerge] = &*proc;
  opProcs[ops::Type::rsSynt] = &*proc;
}

const OperationHandle* ossOperationsFacet::operator()(const PictID pid) const {
  if (operations.contains(pid)) {
    return operations.at(pid).get();
  } else {
    return nullptr;
  }
}

OperationHandle* ossOperationsFacet::Access(const PictID pid) {
  return &*operations.at(pid);
}

bool ossOperationsFacet::IsOperable(const PictID pid) const {
  return operator()(pid) != nullptr;
}

void ossOperationsFacet::Erase(const PictID pid) noexcept {
  operations.erase(pid);
}

ops::Status ossOperationsFacet::StatusOf(const PictID pid) const {
  if (!IsOperable(pid)) {
    return ops::Status::undefined;
  } else if (const auto* opHandle = operator()(pid);
             opHandle->type == ops::Type::tba) {
    return ops::Status::undefined;
  } else if (opHandle->broken) {
    return ops::Status::broken;
  } else if (!std::empty(*core.Src()(pid))) {
    return opHandle->outdated ? ops::Status::outdated : ops::Status::done;
  } else {
    return ops::Status::defined;
  }
}

std::unordered_set<ops::Type> ossOperationsFacet::VariantsFor(const PictID pid) const {
  if (!IsOperable(pid)) {
    return std::unordered_set<ops::Type>{};
  } else {
    const auto signa = SignaFor(pid);
    std::unordered_set<ops::Type> result{ ops::Type::tba };
    for (const auto& [type, processor] : opProcs) {
      if (processor->CanProcess(type, signa.args)) {
        result.insert(type);
      }
    }
    return result;
  }
}

ops::Signature ossOperationsFacet::SignaFor(const PictID pid) const {
  ops::Signature signa{};
  signa.result = core(pid)->dataType;
  for (const auto parent : core.Graph().ParentsOf(pid)) {
    signa.args.emplace_back(core(parent)->dataType);
  }
  return signa;
}

bool ossOperationsFacet::IsExecutable(const PictID pid) {
  if (!IsOperable(pid)) {
    return false;
  } else {
    return CheckOperation(pid);
  }
}

bool ossOperationsFacet::IsTranslatable(PictID pid) {
  if (!IsOperable(pid)) {
    return false;
  } else {
    return CheckTranslations(pid);
  }
}

bool ossOperationsFacet::CheckTranslations(const PictID pid) {
  if (const auto* oldData = core.Src().DataFor(pid); oldData == nullptr) {
    return false;
  } else {
    assert(operations.at(pid)->translations != nullptr);
    return opProcs.at(operations.at(pid)->type)->CheckTranslations(*operations.at(pid)->translations, *oldData);
  }
}

bool ossOperationsFacet::CheckOperation(const PictID pid) {
  const auto call = CallFor(pid);
  operations.at(pid)->broken =
    !opProcs.contains(call.type) || 
    !opProcs.at(call.type)->CheckCall(call);
  return !operations.at(pid)->broken;
}

ops::Call ossOperationsFacet::CallFor(const PictID pid) const {
  const auto* hndl = core.Ops()(pid);
  ops::Call call{};
  call.type = hndl->type;
  call.params = hndl->options.get();
  for (const auto parent : core.Graph().ParentsOf(pid)) {
    core.Src().UpdateSync(parent);
    call.args.emplace_back(core.Src().DataFor(parent));
  }
  return call;
}

bool ossOperationsFacet::InitFor(const PictID pid, ops::Type operation, 
                                 std::unique_ptr<ops::Options> newOptions) {
  if (!IsOperable(pid)) {
    return false;
  } else if (ops::HasOptions(operation) && (newOptions == nullptr)) {
    return false;
  } else if (auto& opHandle = operations.at(pid);
             opHandle->type == operation && ((opHandle->options == nullptr) == (newOptions == nullptr)) &&
             (newOptions == nullptr || opHandle->options->IsEqualTo(*newOptions))) {
    return true;
  } else {
    opHandle->InitOperation(operation, std::move(newOptions));
    core.Src().Discard(pid);
    CheckOperation(pid);
    core.NotifyModification();
    return true;
  }
}

bool ossOperationsFacet::PrepareParents(const PictID pid) {
  for (const auto parent : core.Graph().ParentsOf(pid)) {
    if (const auto* parentOp = operator()(parent); parentOp == nullptr) {
      continue;
    } else if (parentOp->broken) {
      return false;
    } else if (parentOp->outdated) {
      if (!Execute(parent)) {
        return false;
      }
    }
  }
  return true;
}

bool ossOperationsFacet::Execute(const PictID pid, bool autoDiscard) {
  if (!IsOperable(pid)) {
    return false;
  } else if (!PrepareParents(pid)) {
    return false;
  } else if (!CheckOperation(pid)) {
    return false;
  } else if (!RunOperation(pid, autoDiscard)) {
    return false;
  } else {
    core.NotifyModification();
    return true;
  }
}

void ossOperationsFacet::ExecuteAll() {
  for (const auto pid : core.Graph().ExecuteOrder()) {
    if (IsOperable(pid)) {
      Execute(pid, true);
    }
  }
}

bool ossOperationsFacet::RunOperation(const PictID pid, bool autoDiscard) {
  auto opResult = CreateNewResult(pid);
  if (auto old2New = AggregateVersions(pid, opResult); old2New.has_value()) {
    return SaveOperationResult(pid, old2New.value(), std::move(opResult));
  } else if (!autoDiscard) {
    return false;
  } else {
    core.Src().Discard(pid);
    return SaveOperationResult(pid, EntityTranslation{}, std::move(opResult));
  }
}

ops::Result ossOperationsFacet::CreateNewResult(const PictID pid) {
  const auto call = CallFor(pid);
  ops::Result opResult = opProcs[call.type]->Execute(call).value(); // NOLINT(bugprone-unchecked-optional-access)
  assert(opResult.value != nullptr);

  auto& data = *opResult.value;
  ossSourceFacet::EnableTracking(data);

  const auto& pict = *core.Access(pid);
  ossSourceFacet::WriteAttribute({ src::DataStream::Attribute::title, pict.title }, data);
  ossSourceFacet::WriteAttribute({ src::DataStream::Attribute::alias, pict.alias }, data);
  ossSourceFacet::WriteAttribute({ src::DataStream::Attribute::comment, pict.comment }, data);
  return opResult;
}

// NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
bool ossOperationsFacet::SaveOperationResult(
  const PictID pid,
  const EntityTranslation& old2New,
  ops::Result&& opResult
) {
  auto& opHandle = operations.at(pid);
  assert(opHandle != nullptr);
  const auto guard = core.DndGuard();
  if (!core.Src().InputData(pid, std::move(opResult.value))) {
    opHandle->broken = true;
    return false;
  } else {
    opHandle->translations = std::move(opResult.translation);
    opHandle->broken = false;
    opHandle->outdated = false;
    for (const auto& child : core.Graph().ChildrenOf(pid)) {
      const auto index = core.Graph().ParentIndex(pid, child).value(); // NOLINT(bugprone-unchecked-optional-access)
      UpdateChild(child, index, old2New);
    }
    return true;
  }
}

void ossOperationsFacet::UpdateChild(const PictID child,
                                     const size_t childIndex,
                                     const EntityTranslation& oldToNew) {
  UpdateTranslations(child, childIndex, oldToNew);
  UpdateOptions(child, childIndex, oldToNew);
  CheckOperation(child);
}

void ossOperationsFacet::UpdateTranslations(const PictID child, 
                                            const size_t childIndex,
                                            const EntityTranslation& oldToNew) {
  if (auto* translations = operations.at(child)->translations.get();
      translations != nullptr && size(*translations) > childIndex) {
    EntityTranslation newTranlation{};
    for (const auto& [key, value] : translations->at(childIndex)) {
      if (oldToNew.ContainsKey(key)) {
        newTranlation.Insert(oldToNew(key), value);
      }
    }
    translations->at(childIndex) = newTranlation;
  }
}

void ossOperationsFacet::UpdateOptions(const PictID child, const size_t childIndex,
                                       const EntityTranslation& oldToNew) {
  using ops::EquationOptions;
  auto& opHandle = *operations.at(child);
  if (opHandle.options == nullptr) {
    return;
  }
  auto* options = dynamic_cast<EquationOptions*>(opHandle.options.get());
  if (options == nullptr) {
    return;
  }

  EquationOptions newParams{};
  for (const auto& [oldKey, oldValue] : *options) {
    auto key = oldKey;
    auto value = oldValue;
    if (childIndex == 0) {
      if (oldToNew.ContainsKey(key)) {
        key = oldToNew(key);
      }
    } else {
      if (oldToNew.ContainsKey(value)) {
        value = oldToNew(value);
      }
    }
    newParams.Insert(key, value, options->PropsFor(oldKey));
  }
  *options = newParams;
}

std::optional<EntityTranslation> 
ossOperationsFacet::AggregateVersions(const PictID pid, ops::Result& opResult) const {
  const auto* oldData = core.Src().DataFor(pid);
  if (oldData == nullptr) {
    return EntityTranslation{};
  }

  core.Src().UpdateSync(pid);
  const auto& opHandle = operations.at(pid);
  const auto versionTrans =
    opProcs.at(opHandle->type)->CreateVersionTranslation(opHandle->type, *opHandle->translations, *opResult.translation);

  // Note: при переходе к нескольким типам операндов обобщить этот код
  auto* newSchema = dynamic_cast<semantic::RSForm*>(&*opResult.value);
  const auto* oldSchema = dynamic_cast<const semantic::RSForm*>(&*oldData);
  assert(newSchema != nullptr);
  assert(oldSchema != nullptr);
  return newSchema->Ops().ExtrapolateFromPrevious(*oldSchema, versionTrans);
}

change::Usage ossOperationsFacet::QueryEntityUsage(const src::Source& src, const EntityUID cstID) const {
  if (const auto pict = core.Src().Src2PID(src); pict.has_value()) {
    return CstUsage(cstID, pict.value());
  } else {
    return change::Usage::notUsed;
  }
}

change::Usage ossOperationsFacet::CstUsage(const EntityUID cstID, const PictID pid) const {
  using ops::EquationOptions;
  auto result = change::Usage::notUsed;
  for (const auto child : core.Graph().ChildrenOf(pid)) {
    const auto opIndex = core.Graph().ParentIndex(pid, child).value(); // NOLINT(bugprone-unchecked-optional-access)
    const auto* eqs = dynamic_cast<const EquationOptions*>(operations.at(child)->options.get());
    if (eqs != nullptr) {
      if (opIndex == 0 && eqs->ContainsKey(cstID)) {
        return change::Usage::asOption;
      } else if (opIndex == 1 && eqs->ContainsValue(cstID)) {
        return change::Usage::asOption;
      }
    }

    const auto* translations = operations.at(child)->translations.get();
    if (translations != nullptr && size(*translations) > opIndex) {
      if (translations->at(opIndex).ContainsKey(cstID)) {
        result = std::max(change::Usage::asElement, result);
        if (CstUsage(translations->at(opIndex)(cstID), child) == change::Usage::asOption) {
          return change::Usage::asOption;
        }
      }
    }
  }
  return result;
}

} // namespace ccl::oss