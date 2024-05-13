#include "ASTNormalizer.h"

#include "ccl/rslang/RSGenerator.h"

#include <stack>

namespace ccl::rslang {

void Normalizer::Normalize(SyntaxTree::Node& root) {
  switch (root.token.id) {
  default: break;
  case TokenID::FORALL:
  case TokenID::EXISTS: {
    Quantifier(root);
    break;
  }
  case TokenID::NT_RECURSIVE_FULL:
  case TokenID::NT_RECURSIVE_SHORT: {
    Recursion(root);
    break;
  }
  case TokenID::NT_DECLARATIVE_EXPR: {
    Declarative(root);
    break;
  }
  case TokenID::NT_IMPERATIVE_EXPR: {
    Imperative(root);
    break;
  }
  case TokenID::NT_FUNC_CALL: {
    Function(root);
    break;
  }
  }

  for (Index child = 0; child < root.ChildrenCount(); ++child) {
    Normalize(root(child));
  }
}

void Normalizer::Quantifier(SyntaxTree::Node& quant) {
  const auto declToken = quant(0).token.id;
  if (declToken == TokenID::NT_TUPLE_DECL) {
    TupleDeclaration(quant(0), quant(2));
  } else if (declToken == TokenID::NT_ENUM_DECL) {
    EnumDeclaration(quant);
  }
}

void Normalizer::Declarative(SyntaxTree::Node& root) {
  if (root(0).token.id != TokenID::NT_TUPLE_DECL) {
    return;
  }
  const auto newName = ProcessTupleDeclaration(root(0));
  SubstituteTupleVariables(root(1), newName);
  SubstituteTupleVariables(root(2), newName);
}

void Normalizer::Recursion(SyntaxTree::Node& root) {
  if (root(0).token.id != TokenID::NT_TUPLE_DECL) {
    return;
  }
  const auto newName = ProcessTupleDeclaration(root(0));
  SubstituteTupleVariables(root(2), newName);
  if (root.token.id == TokenID::NT_RECURSIVE_FULL) {
    SubstituteTupleVariables(root(3), newName);
  }
}

void Normalizer::Imperative(SyntaxTree::Node& root) {
  for (Index child = 1; child < root.ChildrenCount(); ++child) {
    const auto childID = root(child).token.id;
    if (childID != TokenID::ITERATE && childID != TokenID::ASSIGN) {
      continue;
    }
    auto& declRoot = root(child);
    if (declRoot(0).token.id != TokenID::NT_TUPLE_DECL) {
      continue;
    }
    const auto newName = ProcessTupleDeclaration(declRoot(0));
    for (Index child2 = 0; child2 < root.ChildrenCount(); ++child2) {
      if (child2 != child) {
        SubstituteTupleVariables(root(child2), newName);
      }
    }
    SubstituteTupleVariables(root, newName);
  }
}

void Normalizer::EnumDeclaration(SyntaxTree::Node& quant) {
  auto newQuant = std::make_unique<SyntaxTree::Node>(quant.token);
  newQuant->AddChildCopy(quant(0));
  newQuant->AddChildCopy(quant(1));
  newQuant->AdoptChild(quant.ExtractChild(2));

  if (quant(0).ChildrenCount() > 2) {
    newQuant->At(0).RemoveChild(0);
  } else {
    newQuant->At(0) = newQuant->At(0)(1);
  }
  quant.AdoptChild(std::move(newQuant));
  quant(0) = quant(0)(0);
}

void Normalizer::TupleDeclaration(
  SyntaxTree::Node& declaration,
  SyntaxTree::Node& predicate
) {
  const auto newName = ProcessTupleDeclaration(declaration);
  SubstituteTupleVariables(predicate, newName);
}

std::string Normalizer::ProcessTupleDeclaration(SyntaxTree::Node& root) {
  tupleSubstitutes.clear();
  std::string newName{ '@' };

  std::stack<const SyntaxTree::Node*> nodeStack{};
  std::stack<std::vector<Index>> pathStack{};
  nodeStack.push(&root);
  pathStack.emplace();
  while (!std::empty(nodeStack)) {
    auto curPath = pathStack.top();
    pathStack.pop();
    const auto* curNode = nodeStack.top();
    nodeStack.pop();
    if (curNode->token.id == TokenID::ID_LOCAL) {
      const auto& name = curNode->token.data.ToText();
      newName += name;
      tupleSubstitutes.insert({ name, curPath });
    } else if (const auto childCount = curNode->ChildrenCount(); childCount > 0) {
      for (auto child = static_cast<Index>(childCount - 1); child >= 0; --child) {
        curPath.emplace_back(static_cast<Index>(child + 1));
        pathStack.push(curPath);
        nodeStack.push(&curNode->At(child));
        curPath.pop_back();
      }
    }
  }
  root.RemoveAll();
  root.token.data = TokenData{ newName };
  root.token.id = TokenID::ID_LOCAL;
  return newName;
}

void Normalizer::SubstituteTupleVariables(SyntaxTree::Node& target, const std::string& newName) {
  for (Index child = 0; child < target.ChildrenCount(); ++child) {
    if (target(child).token.id != TokenID::ID_LOCAL) {
      SubstituteTupleVariables(target(child), newName);
    } else {
      const auto& localName = target(child).token.data.ToText();
      if (tupleSubstitutes.contains(localName)) {
        const auto& indexes = tupleSubstitutes.at(localName);
        target(child).token.data = TokenData{ newName };
        for (const auto prIndex : indexes) {
          target.ExtendChild(child, TokenID::SMALLPR);
          target(child).token.pos = target(child)(0).token.pos;
          target(child).token.data = TokenData{ std::vector<Index>{ prIndex } };
        }
      }
    }
  }
}

void Normalizer::Function(SyntaxTree::Node& func) {
  nodeSubstitutes.clear();
  nameSubstitutes.clear();
  
  const auto* funcTree = termFuncs(func(0).token.data.ToText());
  if (funcTree != nullptr) {
    const auto argNames = ArgNames(funcTree->root->At(1).At(0));
    for (Index child = 1; child < func.ChildrenCount(); ++child) {
      nodeSubstitutes.insert({ argNames.at(static_cast<size_t>(child) - 1), &func(child) });
    }
    SyntaxTree newTree = *funcTree;
    SubstituteArgs(newTree.root->At(1).At(1), func.token.pos);
    func = newTree.root->At(1).At(1);
    Normalize(func);
  }
}

std::vector<std::string> Normalizer::ArgNames(const SyntaxTree::Node& declaration) {
  std::vector<std::string> result{};
  result.reserve(static_cast<size_t>(declaration.ChildrenCount()));
  for (Index child = 0; child < declaration.ChildrenCount(); ++child) {
    result.emplace_back(declaration(child)(0).token.data.ToText());
  }
  return result;
}

void Normalizer::SubstituteArgs(SyntaxTree::Node& target, const StrRange pos) {
  target.token.pos = pos;
  if (target.token.id != TokenID::ID_LOCAL) {
    for (Index child = 0; child < target.ChildrenCount(); ++child) {
      SubstituteArgs(target(child), pos);
    }
  } else {
    const auto& localName = target.token.ToString();
    if (nodeSubstitutes.contains(localName)) {
      target = *nodeSubstitutes.at(localName);
    } else {
      const auto& oldName = target.token.ToString();
      std::string newName{};
      const auto iter = nameSubstitutes.find(oldName);
      if (iter == std::end(nameSubstitutes)) {
        ++localVarBase;
        newName = R"(__var)" + std::to_string(localVarBase);
        nameSubstitutes.insert(make_pair(oldName, newName));
      } else {
        newName = iter->second;
      }
      target.token.data = TokenData{ newName };
    }
  }
}

} // namespace ccl::rslang