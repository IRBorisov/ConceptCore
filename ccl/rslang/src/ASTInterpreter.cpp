#include "ccl/rslang/ASTInterpreter.h"

#include "NameCollector.h"

#include <stack>

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26446 ) // Do not warn about operator[] access
#endif

using ccl::object::StructuredData;
using ccl::object::Factory;

namespace ccl::rslang {

class ASTInterpreter::ImpEvaluator {
public:
  ImpEvaluator(ASTInterpreter& parent, const Cursor imperative)
    : parent{ parent }, imperative{ imperative } {}

public:
  StructuredData value{ Factory::EmptySet() };
  
private:
  struct BlockMeta {
    TokenID type{ TokenID::NT_IMP_LOGIC };
    uint32_t arg{};
    std::optional<object::StructuredData> domain{};
  };

  ASTInterpreter& parent;
  const Cursor imperative;

  std::vector<BlockMeta> metaData{};
  std::stack<Index> blockStack{};
  std::stack<object::SDIterator> iterStack{};
  Index current{ 0 };
  bool incrementIter{};

public:
  [[nodiscard]] bool Evaluate() {
    CreateBlockMetadata();
    for (current = 0; ; ++current) {
      incrementIter = false;
      if (IsDone()) {
        if (!SaveElement()) {
          return false;
        }
      } else {
        if (!ProcessBlock()) {
          return false;
        }
      }
      if (incrementIter && !PrepareNextIteration()) {
        return true;
      }
      if (++parent.iterationCounter > MAX_ITERATIONS) {
        parent.OnError(ValueEID::iterationsLimit, imperative->pos.start,
                       std::to_string(MAX_ITERATIONS));
        return false;
      }
    }
  }

private:
  void CreateBlockMetadata() {
    auto iter = imperative;
    iter.MoveToChild(1);
    do {
      BlockMeta newBlock{};
      newBlock.type = iter->id;
      switch (newBlock.type) {
      case TokenID::NT_IMP_DECLARE: {
        newBlock.arg = *begin(parent.nodeVars[iter.Child(0).get()]);
        break;
      }
      case TokenID::NT_IMP_ASSIGN: {
        newBlock.arg = *begin(parent.nodeVars[iter.Child(0).get()]);
        break;
      }
      default:
      case TokenID::NT_IMP_LOGIC: break;
      }
      metaData.emplace_back(std::move(newBlock));
    } while (iter.MoveToNextSibling());
  }

  [[nodiscard]] bool IsDone() const noexcept {
    return current + 1 >= imperative.ChildrenCount();
  }

  [[nodiscard]] bool SaveElement() {
    auto element = parent.EvaluateChild(imperative, 0);
    if (!element.has_value()) {
      return false;
    } else {
      value.ModifyB().AddElement(std::get<StructuredData>(element.value()));
      incrementIter = true;
      return true;
    }
  }

  [[nodiscard]] bool ProcessBlock() {
    auto child = imperative;
    child.MoveToChild(static_cast<Index>(current + 1));
    auto& meta = metaData.at(static_cast<size_t>(current));

    switch (meta.type) {
    default:
    case TokenID::NT_IMP_LOGIC: {
      const auto predicatValue = parent.EvaluateChild(child, 0);
      if (!predicatValue.has_value()) {
        return false;
      } else {
        incrementIter = !std::get<bool>(predicatValue.value());
        return true;
      }
    }
    case TokenID::NT_IMP_DECLARE: {
      const auto domain = parent.ExtractDomain(child);
      if (!domain.has_value()) {
        return false;
      } else if (domain->B().IsEmpty()) {
        incrementIter = true;
        return true;
      } else {
        meta.domain = domain.value();

        auto element = std::begin(meta.domain->B());
        parent.idsData[meta.arg] = *element;
        blockStack.emplace(current);
        iterStack.emplace(element);
        return true;
      }
    }
    case TokenID::NT_IMP_ASSIGN: {
      const auto localValue = parent.ExtractDomain(child);
      if (!localValue.has_value()) {
        return false;
      } else {
        parent.idsData[meta.arg] = localValue.value();
        return true;
      }
    }
    }
  }

  [[nodiscard]] bool PrepareNextIteration() {
    while (!empty(blockStack)) {
      object::SDIterator element = iterStack.top();
      current = blockStack.top();
      const auto& data = metaData.at(static_cast<decltype(metaData)::size_type>(current));
      ++element;
      if (element == std::end(data.domain->B())) { // NOLINT(bugprone-unchecked-optional-access)
        blockStack.pop();
        iterStack.pop();
      } else {
        iterStack.pop();
        iterStack.push(element);
        parent.idsData[data.arg] = *element;
        break;
      }
    }
    return !empty(blockStack);
  }
};

void ASTInterpreter::OnError(const ValueEID eid, const StrPos position) {
  const Error err{ static_cast<uint32_t>(eid), position };
  if (err.IsCritical()) {
    ++countCriticalErrors;
  }
  if (reporter.has_value()) {
    std::invoke(reporter.value(), err);
  }
}

void ASTInterpreter::OnError(const ValueEID eid, const StrPos position, std::string param) {
  const Error err{ static_cast<uint32_t>(eid), position, { std::move(param) } };
  if (err.IsCritical()) {
    ++countCriticalErrors;
  }
  if (reporter.has_value()) {
    std::invoke(reporter.value(), err);
  }
}

std::optional<ExpressionValue> ASTInterpreter::Evaluate(const rslang::SyntaxTree& tree) {
  Clear();
  if (!NameCollector{ *this }.Visit(tree.Root())
      || !tree.Root().DispatchVisit(*this)) {
    AfterVisit(false);
    return std::nullopt;
  } else {
    AfterVisit(true);
    return curValue;
  }
}

void ASTInterpreter::AfterVisit(bool result) {
  if (!result) {
    if (countCriticalErrors == 0) {
      OnError(ValueEID::unknownError, 0);
    }
  }
}

void ASTInterpreter::Clear() noexcept {
  idsBase.clear();
  idsData.clear();
  nodeVars.clear();
  curValue = {};
  iterationCounter = 0;
  countCriticalErrors = 0;
}

bool ASTInterpreter::VisitAndReturn(ExpressionValue&& value) noexcept {
  curValue = std::move(value);
  return true;
}

bool ASTInterpreter::VisitAndReturn(const ExpressionValue& value) noexcept {
  curValue = value;
  return true;
}

bool ASTInterpreter::ViGlobalDefinition(Cursor iter) {
  return VisitChild(iter, 1);
}

bool ASTInterpreter::ViLocal(Cursor iter) {
  return VisitAndReturn(idsData[*begin(nodeVars[iter.get()])]);
}

bool ASTInterpreter::ViInteger(Cursor iter) {
  return VisitAndReturn(Factory::Val(iter->data.ToInt()));
}

bool ASTInterpreter::ViIntegerSet(Cursor iter) {
  OnError(ValueEID::iterateInfinity, iter->pos.start);
  return false;
}

bool ASTInterpreter::ViEmptySet(Cursor /*iter*/) {
  return VisitAndReturn(Factory::EmptySet());
}

bool ASTInterpreter::ViArithmetic(Cursor iter) {
  if (const auto val1 = EvaluateChild(iter, 0); !val1.has_value()) {
    return false;
  } else if (const auto val2 = EvaluateChild(iter, 1); !val2.has_value()) {
    return false;
  } else {
    const auto op1 = std::get<StructuredData>(val1.value()).E().Value();
    const auto op2 = std::get<StructuredData>(val2.value()).E().Value();
    switch (iter->id) {
    default:
    case TokenID::PLUS:
      return VisitAndReturn(Factory::Val(op1 + op2));
    case TokenID::MINUS:
      return VisitAndReturn(Factory::Val(op1 - op2));
    case TokenID::MULTIPLY:
      return VisitAndReturn(Factory::Val(op1 * op2));
    }
  }
}

bool ASTInterpreter::ViCard(Cursor iter) {
  if (const auto base = EvaluateChild(iter, 0); !base.has_value()) {
    return false;
  } else if (const auto size = std::get<StructuredData>(base.value()).B().Cardinality();
             size == StructuredData::SET_INFINITY) {
    OnError(ValueEID::typedOverflow, iter->pos.start, std::to_string(StructuredData::SET_INFINITY));
    return false;
  } else {
    return VisitAndReturn(Factory::Val(size));
  }
}

bool ASTInterpreter::ViQuantifier(Cursor iter) {
  const auto quantorDomain = ExtractDomain(iter);
  if (!quantorDomain.has_value()) {
    return false;
  }

  const auto varID = *begin(nodeVars[iter.Child(0).get()]);
  const auto quantorForAll = iter->id == TokenID::FORALL;
  for (const auto& child : quantorDomain->B()) {
    if (++iterationCounter > MAX_ITERATIONS) {
      OnError(ValueEID::iterationsLimit, iter->pos.start, std::to_string(MAX_ITERATIONS));
      return false;
    }
    idsData[varID] = child;
    if (const auto iterationValue = EvaluateChild(iter, 2); !iterationValue.has_value()) {
      return false;
    } else if (std::get<bool>(iterationValue.value()) != quantorForAll) {
      return VisitAndReturn(!quantorForAll);
    }
  }
  return VisitAndReturn(quantorForAll);
}

std::optional<StructuredData> ASTInterpreter::ExtractDomain(Cursor iter) {
  if (!VisitChild(iter, 1)) {
    return std::nullopt;
  } else {
    return std::optional<StructuredData>{std::get<StructuredData>(curValue)};
  }
}

bool ASTInterpreter::ViNegation(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else {
    return VisitAndReturn(!std::get<bool>(childValue.value()));
  }
}

bool ASTInterpreter::ViLogicBinary(Cursor iter) {
  if (const auto val1 = EvaluateChild(iter, 0); !val1.has_value()) {
    return false;
  } else if (TryEvaluateFromFirstArg(iter->id, std::get<bool>(val1.value()))) {
    return true;
  } else if (const auto val2 = EvaluateChild(iter, 1); !val2.has_value()) {
    return false;
  } else {
    const auto op1 = std::get<bool>(val1.value());
    const auto op2 = std::get<bool>(val2.value());
    switch (iter->id) {
    default:
    case TokenID::AND: curValue = op1 && op2;  return true;
    case TokenID::OR: curValue = op1 || op2;  return true;
    case TokenID::IMPLICATION: curValue = !op1 || op2;  return true;
    case TokenID::EQUIVALENT: curValue = op1 == op2;  return true;
    }
  }
}

bool ASTInterpreter::TryEvaluateFromFirstArg(TokenID operation, bool firstArgValue) noexcept {
  if ((operation == TokenID::AND && !firstArgValue) ||
    (operation == TokenID::OR && firstArgValue)) {
    return VisitAndReturn(firstArgValue);
  } else if (operation == TokenID::IMPLICATION && !firstArgValue) {
    return VisitAndReturn(!firstArgValue);
  } else {
    return false;
  }
}

bool ASTInterpreter::ViEquals(Cursor iter) {
  if (const auto val1 = EvaluateChild(iter, 0); !val1.has_value()) {
    return false;
  } else if (const auto val2 = EvaluateChild(iter, 1); !val2.has_value()) {
    return false;
  } else {
    return VisitAndReturn((val1 == val2) != (iter->id == TokenID::NOTEQUAL));
  }
}

bool ASTInterpreter::ViOrdering(Cursor iter) {
  if (const auto val1 = EvaluateChild(iter, 0); !val1.has_value()) {
    return false;
  } else if (const auto val2 = EvaluateChild(iter, 1); !val2.has_value()) {
    return false;
  } else {
    const auto op1 = std::get<StructuredData>(val1.value()).E().Value();
    const auto op2 = std::get<StructuredData>(val2.value()).E().Value();
    switch (iter->id) {
    default:
    case TokenID::GREATER: return VisitAndReturn(op1 > op2);
    case TokenID::LESSER: return VisitAndReturn(op1 < op2);
    case TokenID::GREATER_OR_EQ: return VisitAndReturn(op1 >= op2);
    case TokenID::LESSER_OR_EQ: return VisitAndReturn(op1 <= op2);
    }
  }
}

bool ASTInterpreter::ViDeclarative(Cursor iter) {
  const auto setDomain = ExtractDomain(iter);
  if (!setDomain.has_value()) {
    return false;
  }
  const auto varID = *begin(nodeVars[iter.Child(0).get()]);
  auto result = Factory::EmptySet();
  for (const auto& child : setDomain->B()) {
    if (++iterationCounter > MAX_ITERATIONS) {
      OnError(ValueEID::iterationsLimit, iter->pos.start, std::to_string(MAX_ITERATIONS));
      return false;
    }
    idsData[varID] = child;
    if (const auto predicatValue = EvaluateChild(iter, 2); !predicatValue.has_value()) {
      return false;
    } else if (std::get<bool>(predicatValue.value())) {
      result.ModifyB().AddElement(child);
    }
  }
  return VisitAndReturn(std::move(result));
}

bool ASTInterpreter::ViImperative(const Cursor iter) {
  ImpEvaluator eval{ *this, iter };
  if (!eval.Evaluate()) {
    return false;
  } else {
    return VisitAndReturn(eval.value);
  }
}

bool ASTInterpreter::ViRecursion(Cursor iter) {
  const auto initial = ExtractDomain(iter);
  if (!initial.has_value()) {
    return false;
  }
  const auto varID = *begin(nodeVars[iter.Child(0).get()]);
  StructuredData current = initial.value();
  do {
    if (++iterationCounter > MAX_ITERATIONS) {
      OnError(ValueEID::iterationsLimit, iter->pos.start, std::to_string(MAX_ITERATIONS));
      return false;
    }

    idsData[varID] = current;
    if (iter->id == TokenID::NT_RECURSIVE_FULL) {
      if (const auto predicat = EvaluateChild(iter, 2); !predicat.has_value()) {
        return false;
      } else if (!std::get<bool>(predicat.value())) {
        break;
      }
    }

    if (!VisitChild(iter, iter->id == TokenID::NT_RECURSIVE_FULL ? 3 : 2)) {
      return false;
    }
    current = std::get<StructuredData>(curValue);
  } while (idsData[varID] != current);
  return VisitAndReturn(std::move(current));
}

bool ASTInterpreter::ViDecart(Cursor iter) {
  std::vector<StructuredData> args{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (const auto childValue = EvaluateChild(iter, child); !childValue.has_value()) {
      return false;
    } else {
      args.emplace_back(std::get<StructuredData>(childValue.value()));
    }
  }

  curValue = Factory::Decartian(args);
  if (std::get<StructuredData>(curValue).B().Cardinality() == StructuredData::SET_INFINITY) {
    OnError(ValueEID::typedOverflow, iter->pos.start, std::to_string(StructuredData::SET_INFINITY));
    return false;
  } else {
    return true;
  }
}

bool ASTInterpreter::ViBoolean(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else if (const auto value = std::get<StructuredData>(childValue.value());
             (iter.IsRoot() || (iter.Parent().id != TokenID::IN && iter.Parent().id != TokenID::NT_DECLARATIVE_EXPR)) &&
             value.B().Cardinality() >= StructuredData::BOOL_INFINITY) {
    OnError(ValueEID::booleanLimit, iter->pos.start, std::to_string(StructuredData::BOOL_INFINITY));
    return false;
  } else {
    return VisitAndReturn(Factory::Boolean(value));
  }
}

bool ASTInterpreter::ViTuple(Cursor iter) {
  std::vector<StructuredData> args{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (const auto childValue = EvaluateChild(iter, child); !childValue.has_value()) {
      return false;
    } else {
      args.emplace_back(std::get<StructuredData>(childValue.value()));
    }
  }
  return VisitAndReturn(Factory::Tuple(args));
}

bool ASTInterpreter::ViSetEnum(Cursor iter) {
  std::vector<StructuredData> args{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (const auto childValue = EvaluateChild(iter, child); !childValue.has_value()) {
      return false;
    } else {
      args.emplace_back(std::get<StructuredData>(childValue.value()));
    }
  }
  return VisitAndReturn(Factory::Set(args));
}

bool ASTInterpreter::ViBool(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else {
    return VisitAndReturn(Factory::Singleton(std::get<StructuredData>(childValue.value())));
  }
}

bool ASTInterpreter::ViTypedBinary(Cursor iter) {
  if (const auto val1 = EvaluateChild(iter, 0); !val1.has_value()) {
    return false;
  } else if (const auto val2 = EvaluateChild(iter, 1); !val2.has_value()) {
    return false;
  } else {
    const auto& op1 = std::get<StructuredData>(val1.value());
    const auto& op2 = std::get<StructuredData>(val2.value());
    switch (iter->id) {
    default:
    case TokenID::UNION: return VisitAndReturn(op1.B().Union(op2.B()));
    case TokenID::INTERSECTION: return VisitAndReturn(op1.B().Intersect(op2.B()));
    case TokenID::SET_MINUS: return VisitAndReturn(op1.B().Diff(op2.B()));
    case TokenID::SYMMINUS: return VisitAndReturn(op1.B().SymDiff(op2.B()));

    case TokenID::IN: return VisitAndReturn(op2.B().Contains(op1));
    case TokenID::NOTIN: return VisitAndReturn(!op2.B().Contains(op1));
    case TokenID::SUBSET: return VisitAndReturn(!(op1 == op2) && op1.B().IsSubsetOrEq(op2.B()));
    case TokenID::NOTSUBSET: return VisitAndReturn(op1 == op2 || !op1.B().IsSubsetOrEq(op2.B()));
    case TokenID::SUBSET_OR_EQ: return VisitAndReturn(op1.B().IsSubsetOrEq(op2.B()));
    }
  }
}

bool ASTInterpreter::ViProjectSet(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else {
    return VisitAndReturn(std::get<StructuredData>(childValue.value()).B().Projection(iter->data.ToTuple()));
  }
}

bool ASTInterpreter::ViProjectTuple(Cursor iter) {
  const auto childValue = EvaluateChild(iter, 0);
  if (!childValue.has_value()) {
    return false;
  }
  
  const auto& indicies = iter->data.ToTuple();
  std::vector<StructuredData> components{};
  components.reserve(size(indicies));
  for (const auto index : indicies) {
    components.emplace_back(std::get<StructuredData>(childValue.value()).T().Component(index));
  }
  return VisitAndReturn(Factory::Tuple(components));
}

bool ASTInterpreter::ViFilter(Cursor iter) {
  const auto argumentValue = EvaluateChild(iter, static_cast<Index>(iter.ChildrenCount() - 1));
  if (!argumentValue.has_value()) {
    return false;
  }
  const auto& argument = std::get<StructuredData>(argumentValue.value());
  if (argument.B().IsEmpty()) {
    return VisitAndReturn(Factory::EmptySet());
  }

  const auto& indicies = iter->data.ToTuple();
  std::vector<StructuredData> params{};
  params.reserve(size(indicies));
  for (Index child = 0; child < iter.ChildrenCount() - 1; ++child) {
    if (const auto param = EvaluateChild(iter, child); !param.has_value()) {
      return false;
    } else if (const auto val = std::get<StructuredData>(param.value()); val.B().IsEmpty()) {
      return VisitAndReturn(Factory::EmptySet());
    } else {
      params.emplace_back(val);
    }
  }

  auto result = Factory::EmptySet();
  for (const auto& element : argument.B()) {
    auto validElement = true;
    for (auto i = 0U; i < size(indicies); ++i) {
      if (!params[i].B().Contains(element.T().Component(indicies[i]))) {
        validElement = false;
        break;
      }
    }
    if (validElement) {
      result.ModifyB().AddElement(element);
    }
  }
  return VisitAndReturn(std::move(result));
}

bool ASTInterpreter::ViReduce(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else {
    return VisitAndReturn(std::get<StructuredData>(childValue.value()).B().Reduce());
  }
}

bool ASTInterpreter::ViDebool(Cursor iter) {
  if (const auto childValue = EvaluateChild(iter, 0); !childValue.has_value()) {
    return false;
  } else if (const auto value = std::get<StructuredData>(childValue.value());
             value.B().Cardinality() != 1) {
    OnError(ValueEID::invalidDebool, iter->pos.start);
    return false;
  } else {
    return VisitAndReturn(std::get<StructuredData>(childValue.value()).B().Debool());
  }
}

std::optional<ExpressionValue> ASTInterpreter::EvaluateChild(Cursor iter, const Index index) {
  assert(iter.ChildrenCount() > index);

  ExpressionValue result = curValue;
  iter.MoveToChild(index);
  const auto visitSuccess = iter.DispatchVisit(*this);
  std::swap(result, curValue);
  if (!visitSuccess) {
    return std::nullopt;
  } else {
    return result;
  }
}

} // namespace ccl::rslang

#ifdef _MSC_VER
  #pragma warning( pop )
#endif