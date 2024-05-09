#include "ccl/rslang/ValueAuditor.h"

namespace ccl::rslang {

namespace {

bool CombineOperationValues(const TokenID op, const bool v1, const bool v2) noexcept {
  switch (op) {
  default:
  case TokenID::SYMMINUS:
  case TokenID::UNION: return v1 && v2;
  case TokenID::INTERSECTION: return v1 || v2;
  case TokenID::SET_MINUS: return v1;
  }
}

} // namespace

bool ValueAuditor::Check(const rslang::SyntaxTree& tree) {
  Clear();
  return tree.Root().DispatchVisit(*this);
}

void ValueAuditor::OnError(const SemanticEID eid, const StrPos position) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position });
  }
}

void ValueAuditor::OnError(const SemanticEID eid, const StrPos position, std::string param) {
  if (reporter.has_value()) {
    reporter.value()(Error{ static_cast<uint32_t>(eid), position, { std::move(param) } });
  }
}

void ValueAuditor::Clear() noexcept {
  current = ValueClass::invalid;
}

bool ValueAuditor::SetCurrent(const ValueClass type) noexcept {
  current = type;
  return true;
}

bool ValueAuditor::VisitAllAndSetCurrent(Cursor iter, const ValueClass type) {
  return VisitAllChildren(iter) && SetCurrent(type);
}

bool ValueAuditor::AssertChildIsValue(Cursor iter, const Index index) {
  if (!VisitChild(iter, index)) {
    return false;
  } else if (current != ValueClass::value) {
    OnError(SemanticEID::invalidPropertyUsage, iter(index).pos.start);
    return false;
  } else {
    return true;
  }
}

bool ValueAuditor::AssertAllValues(Cursor iter) {
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (!AssertChildIsValue(iter, child)) {
      return false;
    }
  }
  return true;
}

bool ValueAuditor::ViGlobalDeclaration(Cursor iter) {
  if (iter->id == TokenID::PUNC_STRUCT) {
    return VisitChild(iter, 1) && SetCurrent(ValueClass::value);
  } else if (iter.ChildrenCount() == 1) {
    return SetCurrent(ValueClass::value);
  } else {
    return VisitChild(iter, 1);
  }
}

bool ValueAuditor::ViFunctionCall(Cursor iter) {
  const auto& alias = iter(0).data.ToText();
  const auto funcType = globalClass(alias);
  if (funcType == ValueClass::invalid) {
    OnError(SemanticEID::globalNoValue, iter->pos.start, alias);
    return false;
  }

  auto childrenIsValue = true;
  std::vector<ValueClass> args{};
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    if (!VisitChild(iter, child)) {
      return false;
    }
    args.emplace_back(current);
    childrenIsValue = childrenIsValue && current == ValueClass::value;
  }

  if (childrenIsValue) {
    current = funcType;
    return true;
  } else {
    return RunCheckOnFunc(iter, alias, args);
  }
}

bool ValueAuditor::RunCheckOnFunc(
  Cursor iter, 
  const std::string& funcName, 
  const std::vector<ValueClass>& argumentVals
) {
  const auto* ast = globalAST(funcName);
  if (ast == nullptr) {
    OnError(SemanticEID::globalMissingAST, iter->pos.start, funcName);
    return false;
  }

  ValueAuditor funcAuditor{ globalClass, globalAST };
  const auto args = ast->Root().Child(1).Child(0);
  assert(args.ChildrenCount() == ssize(argumentVals));
  for (Index child = 0; child < args.ChildrenCount(); ++child) {
    if (argumentVals.at(static_cast<size_t>(child)) == ValueClass::props) {
      funcAuditor.localProps.emplace_back(args.Child(child)(0).data.ToText());
    }
  }

  if (!ast->Root().Child(1).Child(1).DispatchVisit(funcAuditor)) {
    OnError(SemanticEID::globalFuncNoInterpretation, iter->pos.start);
    return false;
  }
  current = funcAuditor.VType();
  return true;
}

bool ValueAuditor::ViGlobal(Cursor iter) {
  const auto& alias = iter->data.ToText();
  const auto type = globalClass(alias);
  if (type == ValueClass::invalid) {
    OnError(SemanticEID::globalNoValue, iter->pos.start, alias);
    return false;
  }
  current = type;
  return true;
}

bool ValueAuditor::ViRadical(Cursor /*iter*/) {
  return SetCurrent(ValueClass::value);
}

bool ValueAuditor::ViLocal(Cursor iter) {
  const auto& localName = iter->data.ToText();
  if (std::find(begin(localProps), end(localProps), localName) == end(localProps)) {
    return SetCurrent(ValueClass::value);
  } else {
    return SetCurrent(ValueClass::props);
  }
}

bool ValueAuditor::ViQuantifier(Cursor iter) {
  return AssertChildIsValue(iter, 1) && VisitChild(iter, 2);
}

bool ValueAuditor::ViSetexprPredicate(Cursor iter) {
  switch (iter->id) {
  default:
  case TokenID::IN:
  case TokenID::NOTIN:
  case TokenID::SUBSET_OR_EQ:
    return VisitChild(iter, 1) && AssertChildIsValue(iter, 0);

  case TokenID::SUBSET:
  case TokenID::NOTSUBSET:
    return AssertAllValues(iter);
  }
}

bool ValueAuditor::ViDeclarative(Cursor iter) {
  return VisitChild(iter, 2) && VisitChild(iter, 1);
}

bool ValueAuditor::ViImperative(Cursor iter) {
  for (Index child = 1; child < iter.ChildrenCount(); ++child) {
    if (!VisitChild(iter, child)) {
      return false;
    }
  }
  return AssertChildIsValue(iter, 0);
}

bool ValueAuditor::ViDecart(Cursor iter) {
  auto type = ValueClass::value;
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (!VisitChild(iter, child)) {
      return false;
    }
    if (current == ValueClass::props) {
      type = current;
    }
  }
  return SetCurrent(type);
}

bool ValueAuditor::ViBoolean(Cursor iter) {
  return VisitChild(iter, 0) && SetCurrent(ValueClass::props);
}

bool ValueAuditor::ViSetexprBinary(Cursor iter) {
  if (!VisitChild(iter, 0)) {
    return false;
  }
  const auto firstValue = current == ValueClass::value;

  if (!VisitChild(iter, 1)) {
    return false;
  }
  const auto secondValue = current == ValueClass::value;

  if (CombineOperationValues(iter->id, firstValue, secondValue)) {
    return SetCurrent(ValueClass::value);
  } else {
    return SetCurrent(ValueClass::props);
  }
}

} // namespace ccl::rslang