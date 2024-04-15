#include "ccl/ops/RSEquationProcessor.h"

#include "ccl/semantic/RSForm.h"
#include "ccl/rslang/RSGenerator.h"

namespace ccl::ops {

RSEquationProcessor::RSEquationProcessor(semantic::RSForm& schema) 
  : schema{ schema } {}

void RSEquationProcessor::Clear() noexcept {
  ClearMutable();
  translation.Clear();
}

void RSEquationProcessor::ClearMutable() const noexcept {
  nameSubstitutes.clear();
  equations = nullptr;
}

bool RSEquationProcessor::Execute(const EquationOptions& options) {
  Clear();
  if (!Evaluate(options)) {
    return false;
  } else {
    ChangeEquatedCsts();
    UpdateExpressions();
    RemoveEquatedCsts();
    schema.UpdateState();

    translation.SuperposeWith(schema.DeleteDuplicatesInternal());
    schema.UpdateState();
    return true;
  }
}

bool RSEquationProcessor::Evaluate(const EquationOptions& options) const {
  if (std::empty(options)) {
    return false;
  } else {
    ClearMutable();
    equations = &options;
    if (!ResolveCstAndPrecheck()) {
      return false;
    } else {
      return CheckNonBasicEquations();
    }
  }
}

bool RSEquationProcessor::ResolveCstAndPrecheck() const {
  for (const auto& [key, value] : *equations) {
    if (!PrecheckFor(key, value)) {
      return false;
    } else if (equations->ContainsKey(value)) {
      return false;
    } else {
      nameSubstitutes.emplace(schema.GetRS(key).alias, schema.GetRS(value).alias);
    }
  }
  return true;
}

bool RSEquationProcessor::PrecheckFor(const EntityUID key, const EntityUID value) const {
  if (key == value) {
    return false;
  } else if (!schema.Contains(key) || !schema.Contains(value)) {
    return false;
  } 
  const auto keyType = schema.GetRS(key).type;
  const auto valueType = schema.GetRS(value).type;
  if (!IsRSObject(keyType) || !IsRSObject(valueType)) {
    return false;
  } else if (!IsBaseSet(keyType) && IsBaseSet(valueType)) {
    return false;
  } else if (!IsBaseNotion(keyType) && IsBaseNotion(valueType)) {
    return false;
  } else if (schema.RSLang().Graph().IsReachableFrom(value, key)) {
    return false;
  } else if (schema.Texts().TermGraph().IsReachableFrom(value, key)) {
    return false;
  } else if (!schema.GetParse(key).exprType.has_value() || !schema.GetParse(value).exprType.has_value()) {
    return false;
  } else {
    return true;
  }
}

rslang::ExpressionType RSEquationProcessor::Evaluate(const EntityUID uid) const {
  const auto* type = schema.GetParse(uid).TypePtr();
  if (!std::holds_alternative<rslang::Typification>(*type)) {
    return *type;
  } else {
    auto typificationText = std::get<rslang::Typification>(*type).ToString();
    while (rslang::SubstituteGlobals(typificationText, nameSubstitutes) > 0) {
      const auto fixedType = schema.RSLang().Evaluate(typificationText);
      assert(fixedType.has_value());
      // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
      typificationText = std::get<rslang::Typification>(fixedType.value()).B().Base().ToString();
    }
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
    return std::get<rslang::Typification>(schema.RSLang().Evaluate(typificationText).value());
  }
}

bool RSEquationProcessor::CheckNonBasicEquations() const {
  for (const auto& [key, value] : *equations) {
    if (!IsBaseSet(schema.GetRS(key).type) && 
        !IsBaseSet(schema.GetRS(value).type) &&
        Evaluate(key) != Evaluate(value)) {
      return false;
    }
  }
  return true;
}

void RSEquationProcessor::ChangeEquatedCsts() {
  for (const auto& [keyID, valueID] : *equations) {
    const auto& props = equations->PropsFor(keyID);
    translation.Insert(keyID, valueID);
    schema.EquateTextsOf(keyID, valueID, props);
  }
}

void RSEquationProcessor::RemoveEquatedCsts() {
  for (const auto& pos : *equations) {
    schema.Mods().StopTracking(pos.first);
    schema.Erase(pos.first);
  }
  schema.UpdateState();
}

void RSEquationProcessor::UpdateExpressions() {
  const auto mapping = CreateTranslator(nameSubstitutes);
  for (const auto& uid : schema.RSLang().Graph().InverseTopologicalOrder()) {
    schema.core.Translate(uid, mapping);
  }
  schema.UpdateState();
}

} // namespace ccl::ops