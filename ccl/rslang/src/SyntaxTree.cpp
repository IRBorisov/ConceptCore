#include "ccl/rslang/SyntaxTree.h"

#include "ASTNormalizer.h"

#include <sstream>
#include <cassert>

namespace ccl::rslang {

SyntaxTree::Node::Node(const Node& node2) {
  *this = node2;
}

SyntaxTree::Node& SyntaxTree::Node::operator=(const Node& node2) {
  if (this != &node2) {
    token = node2.token;
    const auto oldCount = size(children);
    for (const auto& child : node2.children) {
      children.emplace_back(std::make_unique<Node>(*child))->parent = this;
    }
    if (oldCount > 0) {
      children.erase(begin(children), begin(children) + static_cast<ptrdiff_t>(oldCount));
      children.shrink_to_fit();
    }
  }
  return *this;
}

Index SyntaxTree::Node::ChildrenCount() const noexcept {
  return static_cast<Index>(ssize(children));
}

SyntaxTree::Node& SyntaxTree::Node::operator()(const Index index) {
  return At(index);
}

SyntaxTree::Node& SyntaxTree::Node::At(const Index index) {
  assert(index < ChildrenCount() && index >= 0);
  return *children.at(static_cast<size_t>(index));
}

const SyntaxTree::Node& SyntaxTree::Node::operator()(const Index index) const  {
  return At(index);
}

const SyntaxTree::Node& SyntaxTree::Node::At(const Index index) const {
  assert(index < ChildrenCount() && index >= 0);
  return *children.at(static_cast<size_t>(index));
}

void SyntaxTree::Node::AdoptChild(RawNode son) {
  assert(son != nullptr);
  children.emplace_back(std::move(son))->parent = this;
}

void SyntaxTree::Node::AddChildCopy(const Node& son) {
  children.emplace_back(std::make_unique<Node>(son))->parent = this;
}

void SyntaxTree::Node::AddNewChild(const TokenID tid) {
  children.emplace_back(std::make_unique<Node>(Token{ tid, token.pos }))->parent = this;
}

void SyntaxTree::Node::RemoveAll() noexcept {
  children.clear();
}

void SyntaxTree::Node::ExtendChild(const Index index, const TokenID tid) {
  assert(index < ChildrenCount() && index >= 0);

  auto newNode = std::make_unique<SyntaxTree::Node>(Token{ tid, At(index).token.pos });
  newNode->AdoptChild(std::move(children.at(static_cast<size_t>(index))));
  newNode->parent = this;
  children.at(static_cast<size_t>(index)) = std::move(newNode);
}

void SyntaxTree::Node::RemoveChild(const Index index) noexcept {
  assert(index < ChildrenCount() && index >= 0);
  children.erase(children.begin() + static_cast<ptrdiff_t>(index));
}

SyntaxTree::RawNode SyntaxTree::Node::ExtractChild(const Index index) {
  assert(index < ChildrenCount() && index >= 0);
  RawNode result = std::move(children.at(static_cast<size_t>(index)));
  children.erase(begin(children) + static_cast<ptrdiff_t>(index));
  return result;
}

bool SyntaxTree::Node::operator==(const Node& node2) const {
  if (size(children) != size(node2.children) ||
      token.id != node2.token.id ||
      token.data != node2.token.data) {
    return false;
  } else {
    for (auto i = 0U; i < size(children); ++i) {
      if (*children.at(i) != *node2.children.at(i)) {
        return false;
      }
    }
    return true;
  }
}

bool SyntaxTree::Cursor::MoveToParent() noexcept {
  if (node->parent != nullptr) {
    node = node->parent;
    return true;
  } else {
    return false;
  }
}

void SyntaxTree::Cursor::MoveToChild(Index son) {
  assert(son < ChildrenCount() && son >= 0);
  node = node->children.at(static_cast<size_t>(son)).get();
}

SyntaxTree::Cursor SyntaxTree::Cursor::Child(Index son) const {
  auto result = *this;
  result.MoveToChild(son);
  return result;
}

bool SyntaxTree::Cursor::MoveToNextSibling() {
  if (node->parent == nullptr) {
    return false;
  }
  for (auto child = next(rbegin(node->parent->children)); child != rend(node->parent->children); ++child) {
    if (child->get() == node) {
      --child;
      node = child->get();
      return true;
    }
  }
  return false;
}

bool SyntaxTree::Cursor::MoveToPrevSibling() {
  if (node->parent == nullptr) {
    return false;
  }
  for (auto child = next(begin(node->parent->children)); child != end(node->parent->children); ++child) {
    if (child->get() == node) {
      --child;
      node = child->get();
      return true;
    }
  }
  return false;
}

const Token* SyntaxTree::Cursor::operator->() const noexcept {
  return &node->token;
}

const Token& SyntaxTree::Cursor::operator*() const noexcept {
  return node->token;
}

bool SyntaxTree::Cursor::IsRoot() const noexcept {
  return node->parent == nullptr;
}

Index SyntaxTree::Cursor::ChildrenCount() const noexcept {
  return node->ChildrenCount();
}

const Token& SyntaxTree::Cursor::operator()(Index index) const {
  return node->children.at(static_cast<size_t>(index))->token;
}

const Token& SyntaxTree::Cursor::Parent() const noexcept {
  return node->parent->token;
}

SyntaxTree::SyntaxTree(const SyntaxTree& tree) {
  *this = tree;
}

SyntaxTree& SyntaxTree::operator=(const SyntaxTree& tree) {
  if (&tree != this) {
    root = std::make_unique<Node>(*tree.root);
  }
  return *this;
}

SyntaxTree::Cursor SyntaxTree::Root() const noexcept {
  return Cursor{ *root };
}

bool SyntaxTree::operator==(const SyntaxTree& t2) const {
  return *root == *t2.root;
}

void SyntaxTree::Normalize(SyntaxTreeContext termFuncs) {
  if (root != nullptr) {
    Normalizer{ termFuncs }.Normalize(*root);
  }
}

std::string AST2String::Apply(const SyntaxTree& ast) {
  static AST2String generator{};
  generator.text.clear();
  ast.Root().DispatchVisit(generator);
  return generator.text;
}

bool AST2String::VisitDefault(Cursor iter) {
  text += '[';
  text += iter->ToString();
  for (Index child = 0; child < iter.ChildrenCount(); ++child) {
    VisitChild(iter, child);
  }
  text += ']';
  return true;
}

std::optional<SyntaxTree::Cursor> FindMinimalNode(SyntaxTree::Cursor root, StrRange range) {
  if (!root->pos.Contains(range)) {
    return std::nullopt;
  } else if (root.ChildrenCount() == 0) {
    return root;
  } else {
    root.MoveToChild(0);
    do {
      const auto result = FindMinimalNode(root, range);
      if (result.has_value()) {
        return result;
      }
    } while (root.MoveToNextSibling());
    root.MoveToParent();
    return root;
  }
}

} // namespace ccl::rslang