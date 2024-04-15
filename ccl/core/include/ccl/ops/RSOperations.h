#pragma once

#include "ccl/semantic/RSForm.h"
#include "ccl/Operation.hpp"
#include "ccl/ops/EquationOptions.h"

namespace ccl::ops {

//! Binary synthesis of RSForms
class BinarySynthes {
  const semantic::RSForm& operand1;
  const semantic::RSForm& operand2;

  EquationOptions equations;

  bool isCorrect{ false };
  std::unique_ptr<semantic::RSForm> resultSchema{ nullptr };
  TranslationData translations{};

public:
  BinarySynthes(const semantic::RSForm& ks1, const semantic::RSForm& ks2, EquationOptions equations);

public:
  std::unique_ptr<semantic::RSForm> Execute();
  [[nodiscard]] const TranslationData& Translations() noexcept { return translations; }
  [[nodiscard]] const EquationOptions& Equations() const noexcept;
  [[nodiscard]] bool IsCorrectlyDefined() const noexcept;

private:
  void ResetResult();
  void PrecreateResult();
  [[nodiscard]] EquationOptions TranslateEquations() const;
};

//! Operation: Maximize part of RSForm
class OpMaxPart {
  const semantic::RSForm& schema;
  SetOfEntities arguments;

public:
  OpMaxPart(const semantic::RSForm& ks, SetOfEntities args)
    : schema{ ks }, arguments{ std::move(args) } {}

public:
  std::unique_ptr<semantic::RSForm> Execute();
  [[nodiscard]] bool IsCorrectlyDefined() const;

private:
  [[nodiscard]] VectorOfEntities GetAllCstMaxPart() const;
  [[nodiscard]] bool CheckCst(EntityUID target, const SetOfEntities& selList) const;
};

//! Operation: Extract basis from RSForm
class OpExtractBasis {
  const semantic::RSForm& schema;
  SetOfEntities arguments;

public:
  OpExtractBasis(const semantic::RSForm& ks, SetOfEntities args)
    : schema{ ks }, arguments{ std::move(args) } {}

public:
  std::unique_ptr<semantic::RSForm> Execute();
  [[nodiscard]] bool IsCorrectlyDefined() const;
};

//! Operation: relativation
class OpRelativation {
  const semantic::RSForm& operand;
  const SetOfEntities  ignoreOperand;
  const EntityUID targetID;
  bool isApplicable{ false };

  std::unique_ptr<semantic::RSForm> resultSchema{ nullptr };

  SetOfEntities  ignoreResult{};
  EntityUID baseCst;
  std::string baseStr{};

  VectorOfEntities processOrder{};
  std::string baseID{};
  std::string termID{};
  std::unordered_map<EntityUID, std::string> types{};
  std::unordered_set<std::string> moifiedFuncs{};
  StrSubstitutes substitutes{};

public:
  OpRelativation(const semantic::RSForm& ks, EntityUID target, const SetOfEntities& ignore = {});

public:
  std::unique_ptr<semantic::RSForm> Execute();
  [[nodiscard]] static bool IsCorrectlyDefined(const semantic::RSForm& schema, EntityUID target,
                                 const SetOfEntities& ignore = {});
  [[nodiscard]] bool IsCorrectlyDefined() const noexcept;

private:
  void ResetState() noexcept;

  void SetupCore();
  [[nodiscard]] SetOfEntities ExtractCore();
  void CreateProcessingOrder(const SetOfEntities& core);
  [[nodiscard]] EntityUID PromoteBaseToStruct(EntityUID target);
  [[nodiscard]] semantic::ConceptRecord CreateStructFrom(EntityUID base, const std::string& newName);

  void CreateUniqueIDs();

  void CreateFunctions();
  [[nodiscard]] std::string GenerateTermFunc() const;

  void ModifyExpressions();
  [[nodiscard]] std::string UpdatedExpression(std::string_view input) const;
  void UpdatedFunctionCalls(std::string& input) const;
  [[nodiscard]] std::string UpdatedFunction(std::string_view input) const;
};

} // namespace ccl::ops