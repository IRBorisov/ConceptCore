#include "ccl/ops/RSAggregator.h"

#include "ccl/semantic/RSForm.h"

namespace ccl::ops {

void RSAggregator::Clear() noexcept {
  prevOutput = nullptr;
  translation.Clear();
  nameSubstitutes.clear();
  insertedCsts.clear();
}

std::optional<EntityTranslation> RSAggregator::Merge(const semantic::RSForm& previous,
                                                     const EntityTranslation& oldToNew) {
  if (&previous == &output) {
    return previous.Ops().Identity();
  }
  Clear();

  prevOutput = &previous;
  translation = oldToNew;

  if (!PrepareSubstitutes()) {
    return std::nullopt;
  } else {
    TransferNew();
    TransferIheritedData();
    UpdateReferences();
    return translation;
  }
}

bool RSAggregator::PrepareSubstitutes() {
  for (const auto& [key, value] : translation) {
    if (!prevOutput->Contains(key) || !output.Contains(value)) {
      return false;
    }
    const auto& oldCst = prevOutput->GetRS(key);
    const auto& newCst = output.GetRS(value);
    if (IsRSObject(oldCst.type) != IsRSObject(newCst.type) ||
        prevOutput->Mods().IsTracking(key) != output.Mods().IsTracking(value)) {
      return false;
    } else {
      nameSubstitutes.insert({ oldCst.alias, newCst.alias });
    }
  }
  return true;
}

std::optional<EntityUID> RSAggregator::TransferCst(const EntityUID target, const semantic::ListIterator insertWhere) {
  if (translation.ContainsKey(target)) {
    return translation(target);
  } else if (prevOutput->Mods().IsTracking(target)) {
    return std::nullopt;
  } else {
    const auto& oldCst = prevOutput->GetRS(target);
    const auto newUID = output.InsertCopy(target, prevOutput->Core());
    if (insertWhere != std::end(output.List())) {
      output.MoveBefore(newUID, insertWhere);
    }
    output.SetExpressionFor(newUID, oldCst.definition);
    output.SetDefinitionFor(newUID, prevOutput->GetText(target).definition.Raw());
    insertedCsts.emplace_back(newUID);
    translation.Insert(target, newUID);
    nameSubstitutes.insert({ oldCst.alias, output.GetRS(newUID).alias });
    return newUID;
  }
}

void RSAggregator::TransferNew() {
  auto insertWhere = std::end(output.List());
  for (const auto uid : prevOutput->List()) {
    if (const auto newID = TransferCst(uid, insertWhere); newID.has_value()) {
      insertWhere = output.List().Find(newID.value());
      ++insertWhere;
    }
  }
}

void RSAggregator::TransferIheritedData() {
  const auto textTranslator = 
    rslang::TFFactory::GetTransition(nameSubstitutes);
  for (const auto& item : translation) {
    const auto& oldCst = prevOutput->GetText(item.first);
    const auto& newCst = output.GetText(item.second);

    if (prevOutput->Mods().IsTracking(oldCst.uid)) {
      const auto* oldMode = prevOutput->Mods()(oldCst.uid);
      auto* newMode = output.Mods()(newCst.uid);
      newMode->allowEdit = oldMode->allowEdit;
      if (newMode->allowEdit) {
        if (oldMode->definition) {
          const auto flag = oldCst.definition.Str() != newCst.definition.Str();
          output.core.SetDefinitionFor(newCst.uid, oldCst.definition.Raw());
          output.core.TranslateDef(newCst.uid, textTranslator);
          newMode->definition = flag;
        }
        if (oldMode->convention) {
          const auto flag = prevOutput->GetRS(item.first).convention != output.GetRS(item.second).convention;
          output.SetConventionFor(newCst.uid, prevOutput->GetRS(item.first).convention);
          newMode->convention = flag;
        }
        if (oldMode->term) {
          const auto flag = oldCst.term != newCst.term;
          output.core.SetTermFor(newCst.uid, oldCst.term.Text().Raw());
          output.core.TranslateTerm(newCst.uid, textTranslator);
          newMode->term = flag;
        }
      }
    }
  }
}

void RSAggregator::UpdateReferences() {
  const auto translator = rslang::TFFactory::GetTransition(nameSubstitutes);
  for (const auto uid : insertedCsts) {
    output.core.Translate(uid, translator);
  }
}

} // namespace ccl::ops