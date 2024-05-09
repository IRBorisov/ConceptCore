#include "NameCollector.h"

#pragma warning( push )
#pragma warning( disable : 26446 ) // Do not warn about operator[] access

namespace ccl::rslang {

using object::Factory;

bool ASTInterpreter::NameCollector::Visit(Cursor iter) {
  return iter.DispatchVisit(*this);
}

bool ASTInterpreter::NameCollector::MergeChildren(Cursor iter) {
  std::vector<uint32_t> args{};
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    if (!VisitChild(iter, child)) {
      return false;
    }
    const auto* childNode = iter.Child(child).get();
    args.insert(end(args), begin(parent.nodeVars[childNode]), end(parent.nodeVars[childNode]));
  }
  parent.nodeVars[iter.get()] = args;
  return true;
}

bool ASTInterpreter::NameCollector::ViGlobalDeclaration(Cursor iter) {
  if (iter->id == TokenID::PUNC_STRUCT) {
    return false; // TODO: specify error
  }
  switch (iter(0).id) {
  default:
  case TokenID::ID_FUNCTION:
  case TokenID::ID_PREDICATE: {
    return false; // TODO: specify error
  }
  case TokenID::ID_GLOBAL: {
    return iter.ChildrenCount() == 2 && VisitChild(iter, 1);
  }
  }
}

bool ASTInterpreter::NameCollector::ViGlobal(Cursor iter) {
  const auto& idName = iter->data.ToText();
  if (parent.idsBase.contains(idName)) {
    parent.nodeVars[iter.get()] = { parent.idsBase[idName] };
    return true;
  }

  const auto nextID = static_cast<uint32_t>(size(parent.idsData));
  parent.idsBase.insert({ idName, nextID });
  const auto data = parent.context(iter->data.ToText());
  if (!data.has_value()) {
    parent.OnError(ValueEID::globalMissingValue, iter->pos.start, iter->data.ToText());
    return false;
    
  } 
  parent.idsData.emplace_back(data.value());
  parent.nodeVars[iter.get()] = { nextID };
  return true;
}

bool ASTInterpreter::NameCollector::ViLocal(Cursor iter) {
  const auto& idName = iter->data.ToText();
  if (parent.idsBase.contains(idName)) {
    parent.nodeVars[iter.get()] = { parent.idsBase[idName] };
  } else {
    const auto nextID = static_cast<uint32_t>(size(parent.idsData));
    parent.idsData.emplace_back(Factory::EmptySet());
    parent.idsBase.insert({ idName, nextID });
    parent.nodeVars[iter.get()] = { nextID };
  }
  return true;
}

bool ASTInterpreter::NameCollector::ViQuantifier(Cursor iter) {
  if (!MergeChildren(iter)) {
    return false;
  }
  auto& vars = parent.nodeVars[iter.get()];
  const auto varID = *begin(parent.nodeVars[iter.Child(0).get()]);
  vars.erase(std::remove(begin(vars), end(vars), varID), end(vars));
  return true;
}

bool ASTInterpreter::NameCollector::ViImperative(Cursor iter) {
  if (!MergeChildren(iter)) {
    return false;
  }

  auto& vars = parent.nodeVars[iter.get()];
  auto child = iter;
  child.MoveToChild(1);
  do {
    switch (child->id) {
    case TokenID::NT_IMP_ASSIGN:
    case TokenID::NT_IMP_DECLARE: {
      const auto varID = *begin(parent.nodeVars[iter.Child(0).get()]);
      vars.erase(std::remove(begin(vars), end(vars), varID), end(vars));
      break;
    }
    default:
    case TokenID::NT_IMP_LOGIC: break;
    }
  } while (child.MoveToNextSibling());
  return true;
}

} // namespace ccl::rslang

#pragma warning( pop )