#include "ccl/semantic/rsCalculationFacet.h"

#include "ccl/semantic/RSModel.h"
#include "ccl/rslang/RSGenerator.h"

namespace ccl::semantic {

rsCalculationFacet::rsCalculationFacet(RSModel& core)
  : meta::MutatorFacet<RSModel>(core) {
  calculator = std::make_unique<rslang::Interpreter>(core.RSLang(), core.RSLang().ASTContext(), Context());
}

rslang::DataContext rsCalculationFacet::Context() const {
  return [&](const std::string& name) -> std::optional<object::StructuredData> {
    // Note: Do not check name because semantic analyzer garantees name will be resolved
    return core.Values().SDataFor(core.Core().FindAlias(name).value());
  };
}

void rsCalculationFacet::Erase(const EntityUID target) noexcept {
  ResetFor(target);
}

void rsCalculationFacet::ResetFor(const EntityUID target) noexcept {
  calculatedEntities.erase(target);
}

EvalStatus rsCalculationFacet::operator()(const EntityUID entity) const {
  if (!core.Contains(entity)) {
    return EvalStatus::UNKNOWN;
  } else if (const auto type = core.GetRS(entity).type; IsCallable(type)) {
    return EvalStatus::UNKNOWN;
  } else if (!IsBaseNotion(type) && !WasCalculated(entity)) {
    return EvalStatus::NEVER_CALCULATED;
  } else if (IsRSObject(type)) {
    if (const auto rsValue = core.Values().SDataFor(entity); !rsValue.has_value()) {
      if (IsBaseNotion(type)) {
        return EvalStatus::UNKNOWN;
      } else {
        return EvalStatus::INCALCULABLE;
      }
    } else if (!rsValue->IsCollection() || !rsValue->B().IsEmpty()) {
      return EvalStatus::HAS_DATA;
    } else {
      return EvalStatus::EMPTY;
    }
  } else if (const auto logicValue = core.Values().StatementFor(entity); logicValue.has_value()) {
    return !*logicValue && type == CstType::axiom ? EvalStatus::AXIOM_FAIL : EvalStatus::HAS_DATA;
  } else {
    return EvalStatus::INCALCULABLE;
  }
}

bool rsCalculationFacet::WasCalculated(const EntityUID entity) const {
  return calculatedEntities.contains(entity);
}

size_t rsCalculationFacet::Count(const EvalStatus status) const {
  return static_cast<size_t>(
    std::count_if(std::begin(core.Core()), std::end(core.Core()),
    [&calc = *this, &status](const auto uid) { return calc(uid) == status; })
  );
}

bool rsCalculationFacet::Calculate(const EntityUID target) {
  if (!core.Contains(target)) {
    return false;
  } else if (!IsCalculable(core.GetRS(target).type)) {
    return false;
  } else {
    const auto result = CalculateCstInternal(target);
    core.ResetDependants(target);
    core.NotifyModification();
    return result;
  }
}

bool rsCalculationFacet::CalculateCstInternal(const EntityUID target) {
  using rslang::Generator;
  if (core.GetParse(target).status != ParsingStatus::VERIFIED) {
    return false;
  } 
  const auto& cst = core.GetRS(target);
  const auto expression = Generator::GlobalDefinition(cst.alias, cst.definition,
                                                      cst.type == CstType::structured);
  calculatedEntities.emplace(target);
  if (const auto calcResult = calculator->Evaluate(expression, rslang::Syntax::MATH);
      !calcResult.has_value()) {
    return false;
  } else {
    if (IsRSObject(cst.type)) {
      const auto& newRSValue = std::get<object::StructuredData>(calcResult.value());
      core.Values().SetRSInternal(target, newRSValue);
    } else {
      const auto& newLogicValue = std::get<bool>(calcResult.value());
      core.Values().SetStatementInternal(target, newLogicValue);
    }
    return true;
  }
}

void rsCalculationFacet::RecalculateAll() {
  calculatedEntities.clear();
  core.Values().ResetAllExceptCore();
  for (const auto entity : core.RSLang().Graph().TopologicalOrder()) {
    if (IsCalculable(core.GetRS(entity).type)) {
      CalculateCstInternal(entity);
    }
  }
  core.NotifyModification();
}

} // namespace ccl::semantic
