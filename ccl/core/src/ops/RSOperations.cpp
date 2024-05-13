#include "ccl/ops/RSOperations.h"

// TODO: Use format library for GCC when available (GCC13+)
#if !defined(__GNUC__) && !defined(__clang__)
#include <format>
#endif

#include "ccl/rslang/MathLexer.h"

namespace ccl::ops {

using semantic::CstType;
using semantic::RSForm;
using semantic::ConceptRecord;
using rslang::Token;
using rslang::TokenID;
using rslang::Typification;

namespace {

bool NeedsBracketsAfterQuantor(const std::string& expr) {
  const auto firstID = rslang::detail::MathLexer{ expr }.lex(); // TODO: do not create lexer for each quantor
  return firstID != TokenID::FORALL
    && firstID != TokenID::EXISTS
    && firstID != TokenID::NOT;
}

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 26446 ) // Disable warnings about using operator[]
#endif

std::optional<StrRange> FindArgPrefix(const std::string_view funcExpr) noexcept {
  auto bracketCount = 0;
  StrPos start = -1;
  for (auto pos = 0U; pos < size(funcExpr); ++pos) {
    if (funcExpr[pos] == ']') {
      --bracketCount;
    } else if (funcExpr[pos] == '[') {
      if (start == -1) {
        start = static_cast<StrPos>(pos);
      }
      ++bracketCount;
    }
    if (bracketCount < 0) {
      return std::nullopt;
    } else if (bracketCount == 0) {
      return StrRange{ start, static_cast<StrPos>(pos + 1) };
    }
  }
  return std::nullopt;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

} // namespace

BinarySynthes::BinarySynthes(const RSForm& ks1, const RSForm& ks2, EquationOptions equations)
  : operand1{ ks1 }, operand2{ ks2 }, equations{ std::move(equations) } {
  ResetResult();
}

void BinarySynthes::ResetResult() {
  PrecreateResult();
  if (std::empty(equations)) {
    isCorrect = true;
    return;
  } else {
    for (const auto& [key, value] : equations) {
      if (!operand1.Contains(key) || !operand2.Contains(value)) {
        isCorrect = false;
        return;
      }
    }
    isCorrect = resultSchema->Ops().IsEquatable(TranslateEquations());
  }
}

void BinarySynthes::PrecreateResult() {
  resultSchema = std::make_unique<RSForm>(operand1);
  translations = { resultSchema->Ops().Identity(), resultSchema->Ops().MergeWith(operand2) };
  resultSchema->UpdateState();
}

const EquationOptions& BinarySynthes::Equations() const noexcept {
  return equations;
}

bool BinarySynthes::IsCorrectlyDefined() const noexcept {
  return isCorrect;
}

std::unique_ptr<semantic::RSForm> BinarySynthes::Execute() {
  if (!IsCorrectlyDefined()) {
    return nullptr;
  }

  EntityTranslation equatedTranslations{};
  if (std::empty(equations)) {
    equatedTranslations = resultSchema->Ops().DeleteDuplicates();
  } else {
    equatedTranslations = resultSchema->Ops().Equate(TranslateEquations()).value(); // NOLINT(bugprone-unchecked-optional-access)
  }

  resultSchema->ResetAliases();
  translations.at(1).SubstituteValues(equatedTranslations);
  translations.at(0).SubstituteValues(equatedTranslations);
  return std::move(resultSchema);
}

EquationOptions BinarySynthes::TranslateEquations() const {
  EquationOptions result = equations;
  result.SubstituteValues(translations.at(1));
  VectorOfEntities swaps{};
  for (const auto& [key, value] : result) {
    if (resultSchema->Contains(key) && resultSchema->Contains(value)) {
      const auto keyType = resultSchema->GetRS(key).type;
      const auto valueType = resultSchema->GetRS(value).type;
      if (keyType != valueType && !IsBaseSet(keyType) && IsBaseNotion(valueType)) {
        swaps.emplace_back(key);
      }
    }
  }
  for (const auto swapKey : swaps) {
    result.SwapKeyVal(swapKey);
  }
  return result;
}

bool OpMaxPart::IsCorrectlyDefined() const {
  for (const auto entity : arguments) {
    if (!schema.Contains(entity)) {
      return false;
    } else if (!IsBaseSet(schema.GetRS(entity).type) && !CheckCst(entity, arguments)) {
      return false;
    }
  }
  return !std::empty(arguments);
}

std::unique_ptr<semantic::RSForm> OpMaxPart::Execute() {
  if (!IsCorrectlyDefined()) {
    return nullptr;
  }
  auto res = std::make_unique<RSForm>();
  const auto cstList = GetAllCstMaxPart();
  res->InsertCopy(cstList, schema.Core());
  res->ResetAliases();
  return res;
}

VectorOfEntities OpMaxPart::GetAllCstMaxPart() const {
  SetOfEntities selList = arguments;
  for (const auto entity : schema.List()) {
    if (!selList.contains(entity) && CheckCst(entity, selList)) {
      selList.emplace(entity);
    }
  }
  return schema.List().SortSubset(selList);
}

bool OpMaxPart::CheckCst(const EntityUID target, const SetOfEntities& selList) const {
  if (std::empty(schema.GetRS(target).definition)) {
    return selList.contains(target);
  } else {
    auto result = true;
    auto cstFWList = schema.RSLang().Graph().InputsFor(target);
    for (const auto entity : cstFWList) {
      if (!selList.contains(entity)) {
        result = false;
        break;
      }
    }
    return result;
  }
}

bool OpExtractBasis::IsCorrectlyDefined() const {
  if (std::empty(arguments)) {
    return false;
  } else {
    return std::all_of(begin(arguments), end(arguments),
                       [&core = schema.Core()](const auto& entity) { return core.Contains(entity); });
  }
}

std::unique_ptr<semantic::RSForm> OpExtractBasis::Execute() {
  if (!IsCorrectlyDefined()) {
    return nullptr;
  }
  auto res = std::make_unique<RSForm>();
  auto basis = schema.RSLang().Graph().ExpandInputs(arguments);
  res->InsertCopy(schema.List().SortSubset(basis), schema.Core());
  res->ResetAliases();
  return res;
}

OpRelativation::OpRelativation(const RSForm& ks, const EntityUID target, const SetOfEntities& ignore)
  : operand{ ks }, ignoreOperand{ ignore }, targetID{ target },
  isApplicable{ OpRelativation::IsCorrectlyDefined(ks, target, ignore) }, baseCst{ target } {}

bool OpRelativation::IsCorrectlyDefined(const RSForm& schema,
                                        const EntityUID target,
                                        const SetOfEntities& ignore) {
  if (!schema.Core().Contains(target)) {
    return false;
  } else if (!schema.GetParse(target).exprType.has_value()) {
    return false;
  } else if (ignore.contains(target)) {
    return false;
  } else if(!std::all_of(begin(ignore), end(ignore),
            [&core = schema.Core()](const auto& entity) 
            { return core.Contains(entity); })) {
    return false;
  } else {
    return
      schema.GetRS(target).type == CstType::structured ||
      schema.GetRS(target).type == CstType::base;
  }
}

bool OpRelativation::IsCorrectlyDefined() const noexcept {
  return isApplicable;
}

std::unique_ptr<semantic::RSForm> OpRelativation::Execute() {
  if (!IsCorrectlyDefined()) {
    return nullptr;
  }

  ResetState();
  resultSchema = std::make_unique<RSForm>(operand);

  SetupCore();
  CreateUniqueIDs();
  CreateFunctions();
  ModifyExpressions();
  resultSchema->ResetAliases();

  return std::move(resultSchema);
}

void OpRelativation::ResetState() noexcept {
  resultSchema = nullptr;
  processOrder.clear();
  types.clear();
  moifiedFuncs.clear();
  substitutes.clear();
}

SetOfEntities OpRelativation::ExtractCore() {
  SetOfEntities result{};
  for (const auto entity : operand.List()) {
    const auto type = operand.GetRS(entity).type;
    if (ignoreOperand.contains(entity) || entity == targetID) {
      continue;
    } else if (IsBaseSet(type)) {
      result.insert(PromoteBaseToStruct(entity));
    } else if (type == CstType::structured) {
      result.insert(entity);
    }
  }
  return result;
}

void OpRelativation::SetupCore() {
  baseCst = targetID;
  ignoreResult = ignoreOperand;
  if (operand.GetRS(targetID).type == CstType::base) {
    baseCst = PromoteBaseToStruct(targetID);
  }

  auto core = ExtractCore();
  core.insert(baseCst);
  core = resultSchema->RSLang().Graph().ExpandOutputs(core);
  core.erase(baseCst);

  CreateProcessingOrder(core);
  // NOLINTNEXTLINE(bugprone-unchecked-optional-access)
  baseStr = std::get<Typification>(resultSchema->GetParse(baseCst).exprType.value()).ToString();
}

EntityUID OpRelativation::PromoteBaseToStruct(const EntityUID target) {
  const auto newID = resultSchema->InsertCopy(target, resultSchema->Core());  
  resultSchema->MoveBefore(newID, resultSchema->List().Find(target));
  const auto newStruct = CreateStructFrom(target, resultSchema->GetRS(newID).alias);
  const auto structID = resultSchema->InsertCopy(newStruct);
  ignoreResult.erase(target);
  ignoreResult.insert(newID);
  resultSchema->Ops().Equate(EquationOptions{ target, structID });
  return structID;
}

ConceptRecord OpRelativation::CreateStructFrom(const EntityUID base, const std::string& newName) {
  auto result = resultSchema->Core().AsRecord(base);
  result.type = CstType::structured;
  result.rs = Token::Str(TokenID::BOOLEAN) + '(' + newName + ')';
  return result;
}

void OpRelativation::CreateProcessingOrder(const SetOfEntities& core) {
  for (const auto entity : resultSchema->List().SortSubset(core)) {
    if (ignoreResult.contains(entity)) {
      continue;
    } else if (resultSchema->GetParse(entity).status == semantic::ParsingStatus::VERIFIED) {
      const auto& cst = resultSchema->GetRS(entity);
      processOrder.emplace_back(entity);
      if (const auto* typif = resultSchema->GetParse(entity).Typification(); typif != nullptr) {
        types.insert({ entity, typif->ToString() });
      }
      if (semantic::IsCallable(cst.type)) {
        moifiedFuncs.insert(cst.alias);
      }
    }
  }
}

void OpRelativation::CreateUniqueIDs() {
  static const std::string_view basePrefix = "__base";
  static const std::string_view termPrefix = "__rt";
  constexpr auto locals_max = 10000;

  termID = termPrefix;
  baseID = basePrefix;
  std::unordered_set<std::string> localNames{};
  for (const auto entity : processOrder) {
    for (const auto& name : rslang::ExtractULocals(resultSchema->GetRS(entity).definition)) {
      localNames.emplace(name);
    }
  }
  for (auto i = 1; i < locals_max; ++i) {
    if (!localNames.contains(termID + std::to_string(i)) &&
        !localNames.contains(baseID + std::to_string(i))) {
      termID = termPrefix;
      termID += std::to_string(i);
      baseID = basePrefix;
      baseID +=std::to_string(i);
      break;
    }
  }
}

void OpRelativation::CreateFunctions() {
  std::string funcName{};
  for (const auto entity : processOrder) {
    if (!types.contains(entity)) {
      continue;
    } else if (IsRSObject(resultSchema->GetRS(entity).type)) {
      if (empty(funcName)) {
        funcName = resultSchema->GetRS(resultSchema->Emplace(CstType::function, GenerateTermFunc())).alias;
      }
      const auto& cstName = resultSchema->GetRS(entity).alias;
#if !defined(__GNUC__) && !defined(__clang__)
      substitutes.insert({ cstName, std::format("{}[{}, {}]", funcName, baseID, cstName) });
#else
      substitutes.insert({ cstName, funcName + "[" + baseID + ", " + cstName + "]" });
#endif
    }
  }
}

std::string OpRelativation::GenerateTermFunc() const {
#if !defined(__GNUC__) && !defined(__clang__)
  return std::format("[{0}{2}{1}, __term{2}{3}({1}{4}R1)] debool(Pr2(Fi1[{{{0}}}](__term)))",
                     baseID,                        // 0
                     baseStr,                        // 1
                     Token::Str(TokenID::IN),        // 2
                     Token::Str(TokenID::BOOLEAN),  // 3
                     Token::Str(TokenID::DECART));  // 4
#else
  std::string result = "[";
  result += baseID + Token::Str(TokenID::IN) + baseStr;
  result += ", __term";
  result += Token::Str(TokenID::IN);
  result += Token::Str(TokenID::BOOLEAN) + "(" + baseStr;
  result += Token::Str(TokenID::DECART) + "R1)]";
  result += " debool(Pr2(Fi1[{" + baseID + "}](__term)))";
  return result;
#endif
}

void OpRelativation::ModifyExpressions() {
  resultSchema->SetExpressionFor(baseCst, Token::Str(TokenID::BOOLEAN) + baseStr);
  const auto& baseName = resultSchema->GetRS(baseCst).alias;
  substitutes.insert({ baseName, baseID });
  
  for (const auto entity : processOrder) {
    const auto& cst = resultSchema->GetRS(entity);
    switch (cst.type) {
    default:
    case CstType::term: {
      const auto updated = UpdatedExpression(cst.definition);
#if !defined(__GNUC__) && !defined(__clang__)
      std::string newExpression = std::format("I{{({0}, {1}) | {0}{4}{2}; {1}{5}{3}}}",
                                       baseID,        // 0
                                       termID,        // 1
                                       baseName,      // 2
                                       updated,        // 3
                                       Token::Str(TokenID::ITERATE), // 4
                                       Token::Str(TokenID::ASSIGN)); // 5
#else
      std::string newExpression = "I{(";
      newExpression += baseID + ", " + termID;
      newExpression += ") | ";
      newExpression += baseID + Token::Str(TokenID::ITERATE) + baseName;
      newExpression += "; ";
      newExpression += termID + Token::Str(TokenID::ASSIGN) + updated + "}";
#endif
      resultSchema->SetExpressionFor(entity, newExpression);
      break;
    }
    case CstType::structured: {
      resultSchema->SetExpressionFor(entity, Token::Str(TokenID::BOOLEAN) + '(' + baseName
                                     + Token::Str(TokenID::DECART) + cst.definition + ')');
      break;
    }
    case CstType::axiom:
    case CstType::theorem: {
      auto newExpr = UpdatedExpression(cst.definition);
      if (NeedsBracketsAfterQuantor(newExpr)) {
        newExpr = '(' + newExpr + ')';
      }
      newExpr = Token::Str(TokenID::FORALL) 
        + baseID  + Token::Str(TokenID::IN) + baseName
        + ' ' + newExpr;
      resultSchema->SetExpressionFor(entity, newExpr);
      break;
    }
    case CstType::predicate:
    case CstType::function: {
      resultSchema->SetExpressionFor(entity, UpdatedFunction(cst.definition));
      break;
    }
    }
  }
}

std::string OpRelativation::UpdatedExpression(const std::string_view input) const {
  std::string result{ input };
  rslang::SubstituteGlobals(result, substitutes);
  UpdatedFunctionCalls(result);
  return result;
}

void OpRelativation::UpdatedFunctionCalls(std::string& input) const {
  const std::string addStr{ baseID + ", " };
  for (const auto& funcName : moifiedFuncs) {
    std::vector<size_t> insertPositions{};
    auto findPos = input.find(funcName);
    while (findPos != std::string::npos) {
      insertPositions.push_back(findPos + size(funcName) + 1);
      findPos = input.find(funcName, findPos + 1);
    }
    size_t difLen{ 0 };
    for (auto pos : insertPositions) {
      input.insert(pos + difLen, addStr);
      difLen += addStr.length();
    }
  }
}

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wuseless-cast"
#endif

std::string OpRelativation::UpdatedFunction(const std::string_view input) const {
  auto result = UpdatedExpression(input);
  const auto prefixPos = FindArgPrefix(result);
  if (!prefixPos.has_value()) {
    return result;
  }
  const auto argEnum = result.substr(static_cast<size_t>(prefixPos->start) + 1, 
                    static_cast<size_t>(prefixPos->length()) - 2);
  const auto expression = result.substr(static_cast<size_t>(prefixPos->finish), 
                    size(result) + 1U - static_cast<size_t>(prefixPos->finish));
#if !defined(__GNUC__) && !defined(__clang__)
  return std::format("[{}{}{}, {}]{}", baseID, Token::Str(TokenID::IN), baseStr, argEnum, expression);
#else
  return "[" + baseID + Token::Str(TokenID::IN) + baseStr + ", " + argEnum + "]" + expression;
#endif
}

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif

} // namespace ccl::ops