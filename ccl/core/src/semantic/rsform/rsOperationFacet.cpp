#include "ccl/semantic/rsOperationFacet.h"

#include "ccl/semantic/RSForm.h"
#include "ccl/ops/RSAggregator.h"
#include "ccl/rslang/RSExpr.h"
#include "ccl/lang/RefsManager.h"

namespace ccl::semantic {

rsOperationFacet::rsOperationFacet(RSForm& core)
  : meta::MutatorFacet<RSForm>(core) {
  equationProcessor = std::make_unique<ops::RSEquationProcessor>(core);
}

// rsOperationFacet::~rsOperationFacet() noexcept = default;

EntityTranslation rsOperationFacet::MergeWith(const RSForm& schema2) {
  VectorOfEntities insertionOrder{};
  insertionOrder.reserve(std::size(schema2.Core()));
  for (const auto entity : schema2.List()) {
    insertionOrder.emplace_back(entity);
  }

  StrSubstitutes contextReplace{};
  EntityTranslation equateParams{};
  SetOfEntities inserted{};
  for (const auto entity : insertionOrder) {
    const auto& etalon = schema2.GetRS(entity);
    const auto& newCst = core.GetRS(core.InsertCopy(entity, schema2.Core()));
    contextReplace.insert({ etalon.alias, newCst.alias });
    inserted.insert(newCst.uid);
    equateParams.Insert(entity, newCst.uid);
  }

  const auto mapping = CreateTranslator(contextReplace);
  for (const auto entity : inserted) {
    core.core.Translate(entity, mapping);
  }
  core.NotifyModification();
  return equateParams;
}

EntityTranslation rsOperationFacet::Identity() const {
  EntityTranslation result{};
  for (const auto uid : core.Core()) {
    result.Insert(uid, uid);
  }
  return result;
}

std::optional<EntityTranslation> rsOperationFacet::Equate(const ops::EquationOptions& equations) {
  if (!equationProcessor->Execute(equations)) {
    return std::nullopt;
  } else {
    const auto& translation = equationProcessor->Translation();
    core.NotifyModification();
    return translation;
  }
}

EntityTranslation rsOperationFacet::DeleteDuplicates() {
  auto result = core.DeleteDuplicatesInternal();
  if (!std::empty(result)) {
    core.UpdateState();
    core.NotifyModification();
  }
  return result;
}

bool rsOperationFacet::IsEquatable(const ops::EquationOptions& equations) const {
  return equationProcessor->Evaluate(equations);
}

std::optional<EntityTranslation> rsOperationFacet::ExtrapolateFromPrevious(const RSForm& oldShema, const EntityTranslation& oldToNew){
  return ops::RSAggregator{ core }.Merge(oldShema, oldToNew);
}

} // namespace ccl::semantic
