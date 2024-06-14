#include "ccl/rslang/TypeAuditor.h"

namespace ccl::rslang {

namespace {

// Helper functions
[[nodiscard]] std::string ToString(const ExpressionType& type) noexcept(false);
[[nodiscard]] constexpr bool IsSubset(TokenID token) noexcept;
[[nodiscard]] bool IsEchelon(SyntaxTree::Cursor iter);
[[nodiscard]] bool IsEchelon(SyntaxTree::Cursor iter, Index index);
void MangleRadicals(const std::string& funcName, Typification& type);

std::string ToString(const ExpressionType& type) noexcept(false) {
  return std::visit(
    meta::Overloads{
      [](const Typification& t) { return t.ToString();  },
      [](const LogicT& /*t*/) { return std::string{"LOGIC"}; },
    }
    , type);
}

constexpr bool IsSubset(const TokenID token) noexcept {
  return token == TokenID::SUBSET
    || token == TokenID::SUBSET_OR_EQ
    || token == TokenID::NOTSUBSET;
}

bool IsEchelon(SyntaxTree::Cursor iter) {
  switch (iter->id) {
  default: return false;

  case TokenID::LIT_INTSET:
  case TokenID::ID_GLOBAL:
  case TokenID::BOOLEAN:
  case TokenID::DECART:
    break;
  }
  for (Index i = 0; i < iter.ChildrenCount(); ++i) {
    if (!IsEchelon(iter, i)) {
      return false;
    }
  }
  return true;
}

bool IsEchelon(SyntaxTree::Cursor iter, const Index index) {
  iter.MoveToChild(index);
  return IsEchelon(iter);
}

bool IsRadical(const std::string& alias) {
  return !empty(alias) && alias.at(0) == 'R' && alias.at(1) != '0';
}

void MangleRadicals(const std::string& funcName, Typification& type) {
  switch (type.Structure()) {
  case StructureType::basic: {
    if (IsRadical(type.E().baseID)) {
      type.E().baseID += funcName;
    }
    return;
  }
  case StructureType::collection: {
    MangleRadicals(funcName, type.B().Base());
    return;
  }
  case StructureType::tuple: {
    for (auto index = Typification::PR_START; index < type.T().Arity() + Typification::PR_START; ++index) {
      MangleRadicals(funcName, type.T().Component(index));
    }
    return;
  }
  }
}

} // unnamed namespace

namespace details {

bool TypeEnv::IsArithmetic(const Typification& type) const {
  const auto traits = context.TraitsFor(type);
  return traits.has_value() && traits.value().isOperable;
}

bool TypeEnv::IsOrdered(const Typification& type) const {
  const auto traits = context.TraitsFor(type);
  return traits.has_value() && traits.value().isOrdered;
}

const Typification* TypeEnv::CommonType(const Typification& type1, const Typification& type2) const {
  if (type1 == Typification::Integer()) {
    const auto traits = context.TraitsFor(type2);
    if (!traits.has_value() || !traits.value().convertsFromInt) {
      return nullptr;
    } else {
      return &type2;
    }
  } else if (type2 == Typification::Integer()) {
    const auto traits = context.TraitsFor(type1);
    if (!traits.has_value() || !traits.value().convertsFromInt) {
      return nullptr;
    } else {
      return &type1;
    }
  } else {
    return nullptr;
  }
}

bool TypeEnv::AreCompatible(const ExpressionType& type1, const ExpressionType& type2) const {
  if (std::holds_alternative<LogicT>(type1)) {
    return std::holds_alternative<LogicT>(type2);
  } else {
    return AreCompatible(std::get<Typification>(type1), std::get<Typification>(type2));
  }
}

bool TypeEnv::AreCompatible(const Typification& type1, const Typification& type2) const {
  if (type1 == type2) {
    return true;
  } 
  
  const auto struct1 = type1.Structure();
  if (struct1 == rslang::StructureType::basic && type1.IsAnyType()) {
    return true;
  }
  const auto struct2 = type2.Structure();
  if (struct2 == rslang::StructureType::basic && type2.IsAnyType()) {
    return true;
  }
  if (struct1 != struct2) {
    return false;
  }

  switch (struct1) {
  default:
  case rslang::StructureType::basic: return CommonType(type1, type2) != nullptr;
  case rslang::StructureType::collection: return AreCompatible(type1.B().Base(), type2.B().Base());
  case rslang::StructureType::tuple: {
    if (type1.T().Arity() != type2.T().Arity()) {
      return false;
    } else {
      for (auto index = Typification::PR_START; index < type1.T().Arity() + Typification::PR_START; ++index) {
        if (!AreCompatible(type1.T().Component(index), type2.T().Component(index))) {
          return false;
        }
      }
      return true;
    }
  }
  }
}

std::optional<Typification> TypeEnv::Merge(const Typification& type1, const Typification& type2) const {
  if (type1 == type2) {
    return type1;
  }

  const auto struct1 = type1.Structure();
  if (struct1 == rslang::StructureType::basic && type1.IsAnyType()) {
    return type2;
  }
  const auto struct2 = type2.Structure();
  if (struct2 == rslang::StructureType::basic && type2.IsAnyType()) {
    return type1;
  }
  if (struct1 != struct2) {
    return std::nullopt;
  }

  switch (struct1) {
  default:
    case rslang::StructureType::basic: {
      const auto* type = CommonType(type1, type2);
      if (type == nullptr) {
        return std::nullopt;
      }
      return *type;
    }
    case rslang::StructureType::collection: {
      auto base = Merge(type1.B().Base(), type2.B().Base());
      if (!base.has_value()) {
        return std::nullopt;
      }
      return base.value().ApplyBool();
    }
    case rslang::StructureType::tuple: {
      if (type1.T().Arity() != type2.T().Arity()) {
        return std::nullopt;
      }
      std::vector<Typification> components{};
      const auto maxIndex = Typification::PR_START + type1.T().Arity();
      for (auto index = Typification::PR_START; index < maxIndex; ++index) {
        auto component = Merge(type1.T().Component(index), type2.T().Component(index));
        if (!component.has_value()) {
          return std::nullopt;
        }
        components.emplace_back(std::move(component.value()));
      }
      return Typification::Tuple(components);
    }
  }
}

bool TypeEnv::CompareTemplated(
  Typification::Substitutes& substitutes,
  const Typification& arg,
  const Typification& value
) {
  if (arg == value) {
    return true;
  }
  if (arg.IsElement() && IsRadical(arg.E().baseID)) {
    const auto& base = arg.E().baseID;
    if (!substitutes.contains(base)) {
      substitutes.insert({ base, value });
      return true;
    } else {
      auto mergeType = Merge(substitutes.at(base), value);
      if (!mergeType.has_value()) {
        return false;
      }
      substitutes.at(base) = std::move(mergeType.value());
      return true;
    }
  }

  const auto valueStructure = value.Structure();
  if (valueStructure == rslang::StructureType::basic && value.IsAnyType()) {
    return true;
  }
  const auto argStructure = arg.Structure();
  if (argStructure != valueStructure) {
    return false;
  }
  switch (argStructure) {
    default:
    case rslang::StructureType::basic:
      return CommonType(arg, value) != nullptr;
    case rslang::StructureType::collection:
      return CompareTemplated(substitutes, arg.B().Base(), value.B().Base());
    case rslang::StructureType::tuple: {
      if (arg.T().Arity() != value.T().Arity()) {
        return false;
      }
      const auto maxIndex = Typification::PR_START + arg.T().Arity();
      for (auto index = Typification::PR_START; index < maxIndex; ++index) {
        if (!CompareTemplated(substitutes, arg.T().Component(index), value.T().Component(index))) {
          return false;
        }
      }
      return true;
    }
  }
}

} // namespace details

bool TypeAuditor::CheckType(const rslang::SyntaxTree& tree) {
  Clear();
  return tree.Root().DispatchVisit(*this);
}

const ExpressionType& TypeAuditor::GetType() const noexcept {
  return currentType;
}

void TypeAuditor::SetExepectTypification(const bool value) noexcept {
  isTypification = value;
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position });
  }
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position, std::string param) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position, { std::move(param) } });
  }
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position, std::vector<std::string> params) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position, std::move(params) });
  }
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position, const ExpressionType& expected, const ExpressionType& actual) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position, { ToString(expected), ToString(actual) } });
  }
}

void TypeAuditor::Clear() noexcept {
  localVars.clear();
  functionArgs.clear();
  functionArgsID.clear();
  currentType = {};
}

bool TypeAuditor::ViGlobalDeclaration(Cursor iter) {
  const auto childrenCount = iter.ChildrenCount();
  if (iter->id == TokenID::PUNC_STRUCT) {
    if (childrenCount != 2 || !IsEchelon(iter, 1)) {
      OnError(SemanticEID::globalStructure, iter(0).pos.finish);
      return false;
    }
    const auto maybeType = ChildType(iter, 1);
    if (!maybeType.has_value()) {
      return false;
    } 
    
    const auto& type = std::get<Typification>(maybeType.value());
    if (!type.IsCollection()) {
      return false;
    }
    return SetCurrent(type.B().Base());
  } else {
    assert(iter->id == TokenID::PUNC_DEFINE);
    if (childrenCount == 1) {
      return SetCurrent(Typification{ iter(0).data.ToText() }.ApplyBool());
    } 
    
    const auto type = ChildType(iter, 1);
    if (!type.has_value()) {
        return false;
    }
    return SetCurrent(type.value());
  }
}

bool TypeAuditor::ViFunctionDefinition(Cursor iter) {
  {
    const auto guard = isFuncDeclaration.CreateGuard();
    if (!VisitChild(iter, 0)) {
      return false;
    }
  }

  for (auto n : functionArgsID) {
    functionArgs.emplace_back(localVars.at(n).arg);
  }

  const auto type = ChildType(iter, 1);
  if (!type.has_value()) {
    return false;
  }
  return SetCurrent(type.value());
}

bool TypeAuditor::ViFunctionCall(Cursor iter) {
  const auto& funcName = iter(0).data.ToText();
  const auto* funcType = env.context.TypeFor(funcName);
  if (funcType == nullptr) {
    OnError(
      SemanticEID::globalNotTyped,
      iter->pos.start,
      funcName
    );
    return false;
  }
  const auto substitutes = CheckFuncArguments(iter, funcName);
  if (!substitutes.has_value()) {
    return false;
  }
  if (!std::holds_alternative<Typification>(*funcType)) {
    return SetCurrent(*funcType);
  } else {
    Typification fixedType = std::get<Typification>(*funcType);
    MangleRadicals(funcName, fixedType);
    if (!empty(substitutes.value())) {
      fixedType.SubstituteBase(substitutes.value());
    }
    return SetCurrent(fixedType);
  }
}

std::optional<Typification::Substitutes> TypeAuditor::CheckFuncArguments(Cursor iter, const std::string& funcName) {
  const auto* args = env.context.FunctionArgsFor(funcName);
  if (args == nullptr) {
    OnError(
      SemanticEID::globalFuncMissing,
      iter(0).pos.start,
      iter(0).data.ToText()
    );
    return std::nullopt;
  }
  const auto expectedArguments = iter.ChildrenCount() - 1;
  const auto argCount = static_cast<Index>(ssize(*args));
  if (argCount != expectedArguments) {
    OnError(
      SemanticEID::invalidArgsArity,
      iter(1).pos.start,
      { std::to_string(expectedArguments), std::to_string(argCount) }
    );
    return std::nullopt;
  }

  Typification::Substitutes substitutes{};
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    const auto childType = ChildType(iter, child);
    if (!childType.has_value() || !std::holds_alternative<Typification>(childType.value())) {
      return std::nullopt;
    }
    Typification argType = args->at(static_cast<size_t>(child) - 1).type;
    MangleRadicals(funcName, argType);
    if (!env.CompareTemplated(substitutes, argType, std::get<Typification>(childType.value()))) {
      OnError(
        SemanticEID::invalidArgumentType,
        iter(child).pos.start, argType,
        childType.value()
      );
      return std::nullopt;
    }
  }
  return substitutes;
}

bool TypeAuditor::ViGlobal(Cursor iter) {
  const auto& alias = iter->data.ToText();
  if (env.context.FunctionArgsFor(alias) != nullptr) {
    OnError(
      SemanticEID::globalFuncWithoutArgs,
      iter->pos.start,
      alias
    );
    return false;
  }
  const auto* type = env.context.TypeFor(alias); 
  if (type == nullptr) {
    OnError(
      SemanticEID::globalNotTyped,
      iter->pos.start,
      alias
    );
    return false;
  }
  return SetCurrent(*type);
}

bool TypeAuditor::ViRadical(Cursor iter) {
  const auto& alias = iter->data.ToText();
  if (!isFuncDeclaration && !isTypification) {
    OnError(
      SemanticEID::radicalUsage,
      iter->pos.start,
      alias
    );
    return false;
  }
  return SetCurrent(Typification(alias).ApplyBool());
}

bool TypeAuditor::ViLocal(Cursor iter) {
  const auto& localName = iter->data.ToText();
  if (isLocalDeclaration || isArgDeclaration) {
    return AddLocalVariable(localName, std::get<Typification>(currentType), iter->pos.start);
  } else {
    const auto* local = GetLocalTypification(localName, iter->pos.start);
    if (local == nullptr) {
      return false;
    }
    return SetCurrent(*local);
  }
}

bool TypeAuditor::ViEmptySet(Cursor /*iter*/) {
  return SetCurrent(Typification::EmptySet());
}

bool TypeAuditor::ViTupleDeclaration(Cursor iter) {
  assert(isLocalDeclaration || isFuncDeclaration);
  const Typification type = std::get<Typification>(currentType);
  if (!type.IsTuple() || type.T().Arity() != iter.ChildrenCount()) {
    OnError(SemanticEID::invalidBinding, iter(0).pos.start);
    return false;
  }
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    currentType = type.T().Component(static_cast<Index>(child + Typification::PR_START));
    if (!VisitChild(iter, child)) {
      return false;
    }
  }
  return SetCurrent(type);
}

bool TypeAuditor::ViArgument(Cursor iter) {
  const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  if (!domain.has_value()) {
    return false;
  }
  const auto guard{ isArgDeclaration.CreateGuard() };
  currentType = domain.value();
  return VisitChild(iter, 0) && SetCurrent(LogicT{});
}

bool TypeAuditor::ViCard(Cursor iter) {
  return ChildTypeDebool(iter, 0, SemanticEID::invalidCard).has_value()
    && SetCurrent(Typification::Integer());
}

bool TypeAuditor::ViArithmetic(Cursor iter) {
  const auto test1 = ChildType(iter, 0);
  if (!test1.has_value()) {
    return false;
  } 
  
  const auto& type1 = std::get<Typification>(test1.value());
  if (!env.IsArithmetic(type1)) {
    OnError(
      SemanticEID::arithmeticNotSupported,
      iter(0).pos.start,
      type1.ToString()
    );
    return false;
  } 
  
  const auto test2 = ChildType(iter, 1);
  if (!test2.has_value()) {
    return false;
  } 
  
  const auto& type2 = std::get<Typification>(test2.value());
  if (!env.IsArithmetic(type2)) {
    OnError(
      SemanticEID::arithmeticNotSupported,
      iter(1).pos.start,
      type2.ToString()
    );
    return false;
  } 
  
  const auto result = env.Merge(type1, type2);
  if (!result.has_value()) {
    OnError(
      SemanticEID::typesNotCompatible,
      iter(1).pos.start,
      type1,
      type2
    );
    return false;
  }
  return SetCurrent(result.value());
}

bool TypeAuditor::ViIntegerPredicate(Cursor iter) {
  const auto test1 = ChildType(iter, 0);
  if(!test1.has_value()) {
    return false;
  } 
  
  const auto& type1 = std::get<Typification>(test1.value());
  if (!env.IsOrdered(type1)) {
    OnError(
      SemanticEID::orderingNotSupported,
      iter(0).pos.start,
      type1.ToString()
    );
    return false;
  } 
  
  const auto test2 = ChildType(iter, 1);
  if (!test2.has_value()) {
    return false;
  } 
  
  const auto& type2 = std::get<Typification>(test2.value());
  if (!env.IsOrdered(type2)) {
    OnError(
      SemanticEID::orderingNotSupported,
      iter(1).pos.start,
      type2.ToString()
    );
    return false;
  } 
  
  if (!env.AreCompatible(type1, type2)) {
    OnError(
      SemanticEID::typesNotCompatible,
      iter(1).pos.start,
      type1,
      type2
    );
    return false;
  }
  return SetCurrent(LogicT{});
}

bool TypeAuditor::ViQuantifier(Cursor iter) {
  StartScope();

  const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  if (!domain.has_value()) {
    return false;
  } else if (!VisitChildDeclaration(iter, 0, domain.value())) {
    return false;
  } else if (!VisitChild(iter, 2)) {
    return false;
  }

  EndScope(iter->pos.start);
  return SetCurrent(LogicT{});
}

bool TypeAuditor::ViEquals(Cursor iter) {
  const auto test1 = ChildType(iter, 0);
  if (!test1.has_value()) {
    return false;
  }
  const auto& type1 = std::get<Typification>(test1.value());

  const auto test2 = ChildType(iter, 1);
  if (!test2.has_value()) {
    return false;
  }
  const auto& type2 = std::get<Typification>(test2.value());

  if (!env.AreCompatible(type1, type2)) {
    OnError(
      SemanticEID::typesNotCompatible,
      iter(1).pos.start,
      type1,
      type2
    );
    return false;
  } 
  return SetCurrent(LogicT{});
}

bool TypeAuditor::ViSetexprPredicate(Cursor iter) {
  auto type2 = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  if (!type2.has_value()) {
    return false;
  }
  const auto isSubset = IsSubset(iter->id);
  if (isSubset) {
    type2.value().ApplyBool();
  }
  const auto type1 = ChildType(iter, 0);
  if (!type1.has_value()) {
    return false;
  }

  if (!env.AreCompatible(type1.value(), type2.value())) {
    if (isSubset) {
      OnError(
        SemanticEID::typesNotEqual,
        iter(1).pos.start,
        type1.value(),
        type2.value()
      );
    } else {
      OnError(
        SemanticEID::invalidElementPredicate,
        iter(1).pos.start,
        { ToString(type1.value()), iter->ToString(), type2->Bool().ToString() }
      );
    }
    return false;
  }
  return SetCurrent(LogicT{});
}

bool TypeAuditor::ViDeclarative(Cursor iter) {
  StartScope();

  auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  if (!domain.has_value()) {
    return false;
  } else if (!VisitChildDeclaration(iter, 0, domain.value())) {
    return false;
  } else if (!VisitChild(iter, 2)) {
    return false;
  }
  
  EndScope(iter->pos.start);
  return SetCurrent(domain->ApplyBool());
}

bool TypeAuditor::ViImperative(Cursor iter) {
  StartScope();

  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    if (!VisitChild(iter, child)) {
      return false;
    }
  }

  auto type = ChildType(iter, 0); 
  if (!type.has_value()) {
    return false;
  }

  EndScope(iter->pos.start);
  return SetCurrent(std::get<Typification>(type.value()).Bool());
}

bool TypeAuditor::ViIterate(Cursor iter) {
  const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  return domain.has_value() 
    && VisitChildDeclaration(iter, 0, domain.value());
}

bool TypeAuditor::ViAssign(Cursor iter) {
  const auto domain = ChildType(iter, 1);
  return domain.has_value()
    && VisitChildDeclaration(iter, 0, std::get<Typification>(domain.value()));
}

bool TypeAuditor::ViRecursion(Cursor iter) {
  StartScope();

  auto initType = ChildType(iter, 1);
  if (!initType.has_value()) {
    return false;
  } 
  if (!VisitChildDeclaration(iter, 0, std::get<Typification>(initType.value()))) {
    return false;
  }

  const bool isFull = iter->id == TokenID::NT_RECURSIVE_FULL;
  const auto iterationIndex = static_cast<Index>(isFull ? 3 : 2);

  auto iterationValue = ChildType(iter, iterationIndex);
  if (!iterationValue.has_value()) {
    return false;
  } 
  if (!env.AreCompatible(iterationValue.value(), initType.value())) {
    OnError(
      SemanticEID::typesNotEqual,
      iter(iterationIndex).pos.start, 
      iterationValue.value(),
      initType.value()
    );
    return false;
  }

  { 
    const auto guard = noWarnings.CreateGuard();
    for (auto retries = typeDeductionDepth; retries > 0; --retries) {
      ClearLocalVariables();
      if (!VisitChildDeclaration(iter, 0, std::get<Typification>(iterationValue.value()))) {
        return false;
      }
      auto newIteration = ChildType(iter, iterationIndex);
      if (!newIteration.has_value()) {
        return false;
      }
      if (std::get<Typification>(newIteration.value()) == std::get<Typification>(iterationValue.value())) {
        break;
      }
      iterationValue = newIteration;
    }
  }

  if (isFull) {
    if (!VisitChild(iter, 2)) {
      return false;
    }
  }

  EndScope(iter->pos.start);
  return SetCurrent(iterationValue.value());
}

bool TypeAuditor::ViDecart(Cursor iter) {
  std::vector<Typification> factors{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    const auto type = ChildTypeDebool(iter, child, SemanticEID::invalidDecart);
    if (!type.has_value()) {
      return false;
    } else {
      factors.emplace_back(type.value());
    }
  }
  return SetCurrent(Typification::Tuple(factors).ApplyBool());
}

bool TypeAuditor::ViBoolean(Cursor iter) {
  auto type = ChildTypeDebool(iter, 0, SemanticEID::invalidBoolean);
  if (!type.has_value()) {
    return false;
  }
  return SetCurrent(type->ApplyBool().ApplyBool());
}

bool TypeAuditor::ViTuple(Cursor iter) {
  std::vector<Typification> components{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    const auto type = ChildType(iter, child);
    if (!type.has_value()) {
      return false;
    }
    components.emplace_back(std::get<Typification>(type.value()));
  }
  return SetCurrent(Typification::Tuple(components));
}

bool TypeAuditor::ViEnumeration(Cursor iter) {
  auto test = ChildType(iter, 0);
  if (!test.has_value()) {
    return false;
  }
  auto& type = std::get<Typification>(test.value());
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    auto childType = ChildType(iter, child);
    if (!childType.has_value()) {
      return false;
    } 
    
    auto merge = env.Merge(type, std::get<Typification>(childType.value())); 
    if (!merge.has_value()) {
      OnError(
        SemanticEID::invalidEnumeration,
        iter(child).pos.start,
        type,
        childType.value()
      );
      return false;
    }
    type = std::move(merge.value());
  }
  return SetCurrent(type.Bool());
}

bool TypeAuditor::ViDebool(Cursor iter) {
  auto type = ChildTypeDebool(iter, 0, SemanticEID::invalidDebool); 
  if (!type.has_value()) {
    return false;
  }
  return SetCurrent(type.value());
}

bool TypeAuditor::ViSetexprBinary(Cursor iter) {
  auto type1 = ChildTypeDebool(iter, 0, SemanticEID::invalidTypeOperation);
  if (!type1.has_value()) {
    return false;
  }

  auto type2 = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
  if (!type2.has_value()) {
    return false;
  }

  const auto result = env.Merge(type1.value(), type2.value());
  if (!result.has_value()) {
    OnError(
      SemanticEID::typesNotEqual,
      iter(1).pos.start,
      type1->Bool(),
      type2->Bool()
    );
    return false;
  }
  return SetCurrent(result.value().Bool());
}

bool TypeAuditor::ViProjectSet(Cursor iter) {
  // T(Pri(a)) = B(Pi(D(T(a))))
  const auto maybeArgument = ChildTypeDebool(iter, 0, SemanticEID::invalidProjectionSet);
  if (!maybeArgument.has_value()) {
    return false;
  }
  const auto& argument = maybeArgument.value();
  if (argument.IsAnyType()) {
    return SetCurrent(Typification::EmptySet());
  }
  if (!argument.IsTuple()) {
    OnError(
      SemanticEID::invalidProjectionSet,
      iter(0).pos.start,
      { iter->ToString(), argument.ToString() }
    );
    return false;
  }

  const auto& indicies = iter->data.ToTuple();
  std::vector<Typification> components{};
  components.reserve(size(indicies));
  for (const auto index : indicies) {
    if (!argument.T().TestIndex(index)) {
      OnError(
        SemanticEID::invalidProjectionSet,
        iter(0).pos.start,
        { iter->ToString(), argument.ToString() }
      );
      return false;
    } else {
      components.emplace_back(argument.T().Component(index));
    }
  }
  return SetCurrent(Typification::Tuple(components).ApplyBool());
}

bool TypeAuditor::ViProjectTuple(Cursor iter) {
  // T(pri(a)) = Pi(T(a))
  const auto maybeArgument = ChildType(iter, 0);
  if (!maybeArgument.has_value()) {
    return false;
  }
  const auto& argument = std::get<Typification>(maybeArgument.value());
  if (argument.IsAnyType()) {
    return SetCurrent(argument);
  }
  if (!argument.IsTuple()) {
    OnError(
      SemanticEID::invalidProjectionTuple,
      iter(0).pos.start,
      { iter->ToString(), argument.ToString() }
    );
    return false;
  }
  
  const auto& indicies = iter->data.ToTuple();
  std::vector<Typification> components{};
  components.reserve(size(indicies));
  for (const auto index : indicies) {
    if (!argument.T().TestIndex(index)) {
      OnError(
        SemanticEID::invalidProjectionTuple,
        iter(0).pos.start,
        { iter->ToString(), argument.ToString() }
      );
      return false;
    } else {
      components.emplace_back(argument.T().Component(index));
    }
  }
  return SetCurrent(Typification::Tuple(components));
}

bool TypeAuditor::ViFilter(Cursor iter) {
  const auto& indicies = iter->data.ToTuple();
  const auto tupleParam = ssize(indicies) == iter.ChildrenCount() - 1;
  if (!tupleParam && iter.ChildrenCount() > 2) {
    OnError(SemanticEID::invalidFilterArity, iter->pos.start);
    return false;
  }

  const auto maybeArgument = ChildType(iter, static_cast<Index>(iter.ChildrenCount() - 1));
  if (!maybeArgument.has_value()) {
    return false;
  } 
  const auto& argument = std::get<Typification>(maybeArgument.value());
  if (argument.IsAnyType() || (argument.IsCollection() && argument.B().Base().IsAnyType())) {
    return SetCurrent(Typification::EmptySet());
  }
  if (!argument.IsCollection() || !argument.B().Base().IsTuple()) {
    OnError(
      SemanticEID::invalidFilterArgumentType,
      iter(static_cast<Index>(iter.ChildrenCount() - 1)).pos.start,
      { iter->ToString(), argument.ToString() }
    );
    return false;
  }

  std::vector<Typification> bases{};
  for (const auto index : indicies) {
    if (!argument.B().Base().T().TestIndex(index)) {
      OnError(
        SemanticEID::invalidFilterArgumentType,
        iter(static_cast<Index>(iter.ChildrenCount() - 1)).pos.start,
        { iter->ToString(), argument.ToString() }
      );
      return false;
    } 
    bases.push_back(argument.B().Base().T().Component(index));
  }

  if (tupleParam) {
    for (Index child = 0; child + 1 < iter.ChildrenCount(); ++child) {
      const auto param = ChildType(iter, child);
      if (!param.has_value()) {
        return false;
      }
      const auto& paramType = std::get<Typification>(param.value());
      if (!paramType.IsCollection() ||
          !env.AreCompatible(bases.at(child), paramType.B().Base())) {
        OnError(
          SemanticEID::typesNotEqual,
          iter(child).pos.start,
          bases.at(child).Bool(), paramType
        );
        return false;
      }
      ++child;
    }
  } else {
    const auto param = ChildType(iter, 0);
    if (!param.has_value()) {
      return false;
    }
    const auto& paramType = std::get<Typification>(param.value());
    const auto expected = Typification::Tuple(std::move(bases)).ApplyBool();
    if (!paramType.IsCollection() ||
        !env.AreCompatible(expected, paramType)) {
      OnError(
        SemanticEID::typesNotEqual,
        iter(0).pos.start,
        expected, paramType
      );
      return false;
    }
  }
  return SetCurrent(maybeArgument.value());
}

bool TypeAuditor::ViReduce(Cursor iter) {
  // T(red(a)) = B(DD(T(a)))
  const auto maybeArgument = ChildType(iter, 0);
  if (!maybeArgument.has_value()) {
    return false;
  }
  const auto& argument = std::get<Typification>(maybeArgument.value());
  if (argument.IsAnyType() || (argument.IsCollection() && argument.B().Base().IsAnyType())) {
    return SetCurrent(Typification::EmptySet());
  }
  if (!argument.IsCollection() || !argument.B().Base().IsCollection()) {
    OnError(
      SemanticEID::invalidReduce,
      iter(0).pos.start + 1,
      argument.ToString()
    );
    return false;
  }
  return SetCurrent(argument.B().Base());
}

bool TypeAuditor::SetCurrent(ExpressionType type) noexcept {
  currentType = std::move(type);
  return true;
}

bool TypeAuditor::VisitAllAndSetCurrent(Cursor iter, const ExpressionType& type) {
  return VisitAllChildren(iter) && SetCurrent(type);
}

bool TypeAuditor::VisitChildDeclaration(const Cursor& iter, const Index index, const Typification& domain) {
  currentType = domain;
  const auto guard = isLocalDeclaration.CreateGuard();
  if (!VisitChild(iter, index)) {
    return false;
  }
  return SetCurrent(LogicT{});
}

std::optional<ExpressionType> TypeAuditor::ChildType(Cursor iter, const Index index) {
  assert(iter.ChildrenCount() > index);
  
  ExpressionType result = currentType;
  iter.MoveToChild(index);
  const auto visitSuccess = iter.DispatchVisit(*this);
  std::swap(result, currentType);
  if (!visitSuccess) {
    return std::nullopt;
  } else {
    return result;
  }
}

std::optional<Typification> TypeAuditor::ChildTypeDebool(Cursor iter, const Index index, const SemanticEID eid) {
  const auto maybeResult = ChildType(iter, index);
  if (!maybeResult.has_value() || !std::holds_alternative<Typification>(maybeResult.value())) {
    return std::nullopt;
  }
  const auto& result = std::get<Typification>(maybeResult.value());
  if (result.IsAnyType()) {
    return result;
  }
  if (!result.IsCollection()) {
    OnError(
      eid,
      iter(index).pos.start,
      ToString(maybeResult.value())
    );
    return std::nullopt;
  }
  return result.B().Base();
}

const Typification* TypeAuditor::GetLocalTypification(const std::string& name, const StrPos pos) {
  const auto varIter = std::find_if(
    begin(localVars),
    end(localVars),
    [&](const auto& data) noexcept { return data.arg.name == name; }
  );
  if (varIter == end(localVars)) {
    if (!isArgDeclaration) {
      OnError(SemanticEID::localUndeclared, pos, name);
    }
    return nullptr;
  } else if (!varIter->enabled) {
    OnError(SemanticEID::localOutOfScope, pos, name);
    return nullptr;
  } else {
    ++varIter->useCount;
    return &varIter->arg.type;
  }
}

void TypeAuditor::StartScope() noexcept {
  for (auto& var : localVars) {
    ++var.level;
  }
}

void TypeAuditor::EndScope(const StrPos pos) {
  for (auto& var : localVars) {
    --var.level;
    if (var.level < 0 && var.enabled) {
      var.enabled = false;
      if (var.useCount == 0 && !noWarnings) {
        OnError(
          SemanticEID::localNotUsed,
          pos,
          var.arg.name
        );
      }
    }
  }
}

bool TypeAuditor::AddLocalVariable(const std::string& name, const Typification& type, const StrPos pos) {
  auto varIter = std::find_if(
    begin(localVars),
    end(localVars),
    [&](const auto& data) noexcept { return data.arg.name == name; }
  );
  if (varIter != end(localVars)) {
    if (varIter->enabled) {
      OnError(
        SemanticEID::localShadowing,
        pos,
        name
      );
      return false;
    } else {
      if (!noWarnings) {
        OnError(
          SemanticEID::localDoubleDeclare,
          pos,
          name
        );
      }
      varIter->arg.type = type;
      varIter->enabled = true;
      varIter->level = 0;
       return true;
    }
  } else {
    localVars.emplace_back(LocalData{ TypedID{name, type}, 0, 0, true });
    if (isArgDeclaration) {
      functionArgsID.emplace_back(localVars.size() - 1U);
    }
    return true;
  }
}


void TypeAuditor::ClearLocalVariables() {
  std::erase_if(localVars, [&](const auto& data) noexcept { return data.level <= 0; });
}

} // namespace ccl::rslang