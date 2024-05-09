#pragma once

#include "ccl/rslang/RSToken.h"
#include "ccl/Strings.hpp"
#include "ccl/cclMeta.hpp"

#include <memory>
#include <optional>
#include <functional>

namespace ccl::rslang {
#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

// Note: Part of SyntaxTree responsible for standard form. Defined in ASTNormalizer.h
class Normalizer;

class SyntaxTree;
//! SyntaxTree context for string identifiers
using SyntaxTreeContext = std::function<const SyntaxTree* (const std::string&)>;

//! Abstract syntax tree for RS expression
class SyntaxTree {
  friend class Normalizer;

public:
  class Cursor;
  class Node;
  using RawNode = meta::UniqueCPPtr<Node>;

private:
  RawNode root;

public:
  ~SyntaxTree() noexcept = default;
  SyntaxTree(const SyntaxTree& tree);
  SyntaxTree& operator=(const SyntaxTree& tree);
  SyntaxTree(SyntaxTree&&) noexcept = default;
  SyntaxTree& operator=(SyntaxTree&&) noexcept = default;

  explicit SyntaxTree(RawNode root) noexcept
    : root{ std::move(root) } {
    assert(this->root != nullptr);
  }

public:
  [[nodiscard]] Cursor Root() const noexcept;

  [[nodiscard]] bool operator==(const SyntaxTree& t2) const;
  [[nodiscard]] bool operator!=(const SyntaxTree& t2) const { return !(*this == t2); }

  //! Normalize syntax tree
  void Normalize(SyntaxTreeContext termFuncs);


public:
  //! AST node class
  class Node {
    friend class Cursor;
    friend class SyntaxTree;

  private:
    const Node* parent{ nullptr };
    std::vector<RawNode> children{};
  public:
    Token token{};

  public:
    ~Node() noexcept = default;
      
    Node(const Node& node2);
    Node& operator=(const Node& node2);

    explicit Node(Token token) noexcept
      : token{ std::move(token) } {}

  public:
    [[nodiscard]] Index ChildrenCount() const noexcept;

    void AddNewChild(TokenID tid);
    void AddChildCopy(const Node& son);

    //! Add child by consuming son
    /// Contract: son != nullptr
    void AdoptChild(RawNode son);

    //! Create intermediate node between this and child this(index) 
    /// Contract: index should be in bounds
    void ExtendChild(Index index, TokenID tid);

    void RemoveChild(Index index) noexcept;
    RawNode ExtractChild(Index index);
    void RemoveAll() noexcept;

    //! \defgroup ASTNodeChildAccess Accessors for child nodes
    /// @{
    [[nodiscard]] Node& operator()(Index index);
    [[nodiscard]] Node& At(Index index);
    [[nodiscard]] const Node& operator()(Index index) const;
    [[nodiscard]] const Node& At(Index index) const;
    /// @}

    [[nodiscard]] bool operator==(const Node& node2) const;
    [[nodiscard]] bool operator!=(const Node& node2) const { 
      return !(*this == node2);
    }
  };


public:
  //! Cursor for traversing AST
  class Cursor {
    const Node* node;

  public:
    explicit Cursor(const Node& node) noexcept
      : node{ &node } {}

  public:
    [[nodiscard]] const Token* operator->() const noexcept;
    [[nodiscard]] const Token& operator*() const noexcept;

    [[nodiscard]] bool IsRoot() const noexcept;
    [[nodiscard]] Index ChildrenCount() const noexcept;

    bool MoveToParent() noexcept;
    bool MoveToNextSibling();
    bool MoveToPrevSibling();
    void MoveToChild(Index son);

    [[nodiscard]] const Token& operator()(Index index) const;
    [[nodiscard]] const Token& Parent() const noexcept;

    [[nodiscard]] Cursor Child(Index son) const;
    [[nodiscard]] const Node* get() const noexcept { return node; }

    template<typename Visitor>
    bool DispatchVisit(Visitor& visitor) {
      switch (node->token.id) {
      default:
      case TokenID::PUNC_DEFINE:
      case TokenID::PUNC_STRUCT:
        assert(ChildrenCount() != 0);
        return visitor.ViGlobalDeclaration(*this);

      case TokenID::ID_GLOBAL:
      case TokenID::ID_FUNCTION:
      case TokenID::ID_PREDICATE:
        assert(ChildrenCount() == 0);
        return visitor.ViGlobal(*this);

      case TokenID::ID_LOCAL:
        assert(ChildrenCount() == 0);
        return visitor.ViLocal(*this);

      case TokenID::ID_RADICAL:
        assert(ChildrenCount() == 0);
        return visitor.ViRadical(*this);

      case TokenID::NT_FUNC_DEFINITION:
        assert(ChildrenCount() == 2);
        return visitor.ViFunctionDefinition(*this);
      case TokenID::NT_FUNC_CALL:
        assert(ChildrenCount() > 1);
        return visitor.ViFunctionCall(*this);

      case TokenID::LIT_INTSET:
        assert(ChildrenCount() == 0);
        return visitor.ViIntegerSet(*this);
      case TokenID::LIT_INTEGER:
        assert(ChildrenCount() == 0);
        return visitor.ViInteger(*this);
      case TokenID::LIT_EMPTYSET:
        assert(ChildrenCount() == 0);
        return visitor.ViEmptySet(*this);

      case TokenID::NT_TUPLE_DECL:
        assert(ChildrenCount() > 1);
        return visitor.ViTupleDeclaration(*this);
      case TokenID::NT_ENUM_DECL:
        return visitor.ViEnumDeclaration(*this);
      case TokenID::NT_ARGUMENTS:
        assert(ChildrenCount() > 0);
        return visitor.ViArgumentsEnum(*this);
      case TokenID::NT_ARG_DECL:
        assert(ChildrenCount() == 2);
        return visitor.ViArgument(*this);

      case TokenID::PLUS:
      case TokenID::MINUS:
      case TokenID::MULTIPLY:
        assert(ChildrenCount() == 2);
        return visitor.ViArithmetic(*this);
      case TokenID::CARD:
        assert(ChildrenCount() == 1);
        return visitor.ViCard(*this);

      case TokenID::FORALL:
      case TokenID::EXISTS:
        assert(ChildrenCount() == 3);
        return visitor.ViQuantifier(*this);

      case TokenID::NOT:
        assert(ChildrenCount() == 1);
        return visitor.ViNegation(*this);
      case TokenID::AND:
      case TokenID::OR:
      case TokenID::IMPLICATION:
      case TokenID::EQUIVALENT:
        assert(ChildrenCount() == 2);
        return visitor.ViLogicBinary(*this);

      case TokenID::EQUAL:
      case TokenID::NOTEQUAL:
        assert(ChildrenCount() == 2);
        return visitor.ViEquals(*this);

      case TokenID::GREATER:
      case TokenID::LESSER:
      case TokenID::GREATER_OR_EQ:
      case TokenID::LESSER_OR_EQ:
        assert(ChildrenCount() == 2);
        return visitor.ViIntegerPredicate(*this);

      case TokenID::IN:
      case TokenID::NOTIN:
      case TokenID::SUBSET:
      case TokenID::SUBSET_OR_EQ:
      case TokenID::NOTSUBSET:
        assert(ChildrenCount() == 2);
        return visitor.ViSetexprPredicate(*this);

      case TokenID::NT_DECLARATIVE_EXPR:
        assert(ChildrenCount() == 3);
        return visitor.ViDeclarative(*this);
      case TokenID::NT_IMPERATIVE_EXPR:
        assert(ChildrenCount() > 1);
        return visitor.ViImperative(*this);
      case TokenID::NT_IMP_DECLARE:
        assert(ChildrenCount() == 2);
        return visitor.ViImpDeclare(*this);
      case TokenID::NT_IMP_ASSIGN:
        assert(ChildrenCount() == 2);
        return visitor.ViImpAssign(*this);
      case TokenID::NT_IMP_LOGIC:
        assert(ChildrenCount() == 1);
        return visitor.ViImpCheck(*this);

      case TokenID::DECART:
        assert(ChildrenCount() > 1);
        return visitor.ViDecart(*this);
      case TokenID::BOOLEAN:
        assert(ChildrenCount() == 1);
        return visitor.ViBoolean(*this);

      case TokenID::NT_RECURSIVE_FULL:
        assert(ChildrenCount() == 4);
        return visitor.ViRecursion(*this);
      case TokenID::NT_RECURSIVE_SHORT:
        assert(ChildrenCount() == 3);
        return visitor.ViRecursion(*this);

      case TokenID::NT_TUPLE:
        assert(ChildrenCount() > 1);
        return visitor.ViTuple(*this);
      case TokenID::NT_ENUMERATION:
        return visitor.ViEnumeration(*this);
      case TokenID::BOOL:
        assert(ChildrenCount() == 1);
        return visitor.ViBool(*this);
      case TokenID::DEBOOL:
        assert(ChildrenCount() == 1);
        return visitor.ViDebool(*this);

      case TokenID::UNION:
      case TokenID::INTERSECTION:
      case TokenID::SET_MINUS:
      case TokenID::SYMMINUS:
        assert(ChildrenCount() == 2);
        return visitor.ViSetexprBinary(*this);

      case TokenID::BIGPR:
        assert(ChildrenCount() == 1);
        return visitor.ViProjectSet(*this);
      case TokenID::SMALLPR:
        assert(ChildrenCount() == 1);
        return visitor.ViProjectTuple(*this);
      case TokenID::FILTER:
        assert(ChildrenCount() > 1);
        return visitor.ViFilter(*this);
      case TokenID::REDUCE:
        assert(ChildrenCount() == 1);
        return visitor.ViReduce(*this);
      }
    }
  };
};

//! Abstract AST visitor - double dispatching
template <typename Visitor>
class ASTVisitor : public meta::crtp<Visitor, ASTVisitor> {
  friend class SyntaxTree::Cursor;

protected:
  using Cursor = SyntaxTree::Cursor;

  bool VisitChild(Cursor iter, Index index) {
    assert(index < iter.ChildrenCount());
    iter.MoveToChild(index);
    return iter.DispatchVisit(this->BaseT());
  }
  bool VisitAllChildren(const Cursor& iter) {
    for (Index child = 0; child < iter.ChildrenCount(); ++child) {
      if (!VisitChild(iter, child)) {
        return false;
      }
    }
    return true;
  }

private:
  bool ViGlobalDeclaration(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViFunctionDefinition(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViFunctionCall(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViGlobal(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViRadical(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViLocal(Cursor iter)  { return this->BaseT().VisitDefault(iter); }
  bool ViInteger(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViIntegerSet(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViEmptySet(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViTupleDeclaration(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViEnumDeclaration(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViArgumentsEnum(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViArgument(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViArithmetic(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViCard(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViQuantifier(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViNegation(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViLogicBinary(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViEquals(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViIntegerPredicate(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViSetexprPredicate(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViDeclarative(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViImperative(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViImpDeclare(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViImpAssign(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViImpCheck(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViRecursion(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViDecart(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViBoolean(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViTuple(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViEnumeration(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViBool(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViDebool(Cursor iter) { return this->BaseT().VisitDefault(iter); }

  bool ViSetexprBinary(Cursor iter)  { return this->BaseT().VisitDefault(iter); }
  bool ViProjectSet(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViProjectTuple(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViFilter(Cursor iter) { return this->BaseT().VisitDefault(iter); }
  bool ViReduce(Cursor iter) { return this->BaseT().VisitDefault(iter); }
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

//! Generator of string representation of AST
class AST2String final : public ASTVisitor<AST2String> {
  friend class SyntaxTree::Cursor;
  friend class ASTVisitor<AST2String>;

  std::string text{};

public:
  static std::string Apply(const SyntaxTree& ast);

protected:
  bool VisitDefault(Cursor iter);
};

//! Search most deep AST node containing range
std::optional<SyntaxTree::Cursor> FindMinimalNode(SyntaxTree::Cursor root, StrRange range);

} // namespace ccl::rslang