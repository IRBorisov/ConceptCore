#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/SyntaxTree.h"

struct MinimalVisitor : ccl::rslang::ASTVisitor<MinimalVisitor> {
  void Run(const ccl::rslang::SyntaxTree& ast) { 
    ast.Root().DispatchVisit(*this);
  }

  bool VisitDefault(Cursor /*iter*/) { return false; }
};

class UTSyntaxTree : public ::testing::Test {
protected:
  using SyntaxTree = ccl::rslang::SyntaxTree;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;
  using TokenData = ccl::rslang::TokenData;
  using StrRange = ccl::StrRange;
};

TEST_F(UTSyntaxTree, NodeEmpty) {
  SyntaxTree::Node root{ Token{} };
  EXPECT_EQ(0, root.ChildrenCount());
}

TEST_F(UTSyntaxTree, NodeToken) {
  const Token tok(TokenID::EQUAL, StrRange{0, 8}, TokenData{false});
  SyntaxTree::Node test{ tok };
  EXPECT_EQ(tok, test.token);
}

TEST_F(UTSyntaxTree, AddNewChild) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  root.AddNewChild(TokenID::OR);
  ASSERT_EQ(root.ChildrenCount(), 1);
  EXPECT_EQ(root(0).token.id, TokenID::OR);
  EXPECT_EQ(root(0).token.pos, root.token.pos);
  EXPECT_NE(root(0).token.data, root.token.data);

  root.AddNewChild(TokenID::OR);
  ASSERT_EQ(root.ChildrenCount(), 2);
  EXPECT_EQ(root(1).token.id, TokenID::OR);
  EXPECT_EQ(root(1).token.pos, root.token.pos);
  EXPECT_NE(root(1).token.data, root.token.data);

  root.AddNewChild(TokenID::AND);
  ASSERT_EQ(root.ChildrenCount(), 3);
  EXPECT_EQ(root(2).token.id, TokenID::AND);
  EXPECT_EQ(root(2).token.pos, root.token.pos);
  EXPECT_NE(root(2).token.data, root.token.data);
}

TEST_F(UTSyntaxTree, AddChildCopy) {
  const Token rootToken{ TokenID::AND, StrRange{2, 4}, TokenData{42} };
  SyntaxTree::Node root{ rootToken };
  const SyntaxTree::Node donor{ Token{ TokenID::OR, StrRange{2, 3}, TokenData{43} } };
  root.AddChildCopy(donor);
  ASSERT_EQ(root.ChildrenCount(), 1);
  EXPECT_EQ(root(0).token, donor.token);

  root.AddChildCopy(root);
  ASSERT_EQ(root.ChildrenCount(), 2);
  EXPECT_EQ(root(0).token, donor.token);
  EXPECT_EQ(root(1)(0).token, donor.token);
  EXPECT_EQ(root(1).token, rootToken);
}

TEST_F(UTSyntaxTree, AdoptChild) {
  const Token rootToken{ TokenID::AND, StrRange{2, 4}, TokenData{42} };
  SyntaxTree::Node root{ rootToken };
  const Token donorToken{ TokenID::OR, StrRange{2, 3}, TokenData{43} };
  auto orphan = std::make_unique<SyntaxTree::Node>(donorToken);
  root.AdoptChild(std::move(orphan));
  ASSERT_EQ(root.ChildrenCount(), 1);
  EXPECT_EQ(root(0).token, donorToken);
}

TEST_F(UTSyntaxTree, ExtendChild) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  root.AddNewChild(TokenID::OR);
  root.AddNewChild(TokenID::BOOL);
  root.AddNewChild(TokenID::BOOLEAN);
  ASSERT_EQ(root.ChildrenCount(), 3);
  root.ExtendChild(1, TokenID::NOT);
  ASSERT_EQ(root.ChildrenCount(), 3);
  ASSERT_EQ(root(1).ChildrenCount(), 1);
  EXPECT_EQ(root(1).token.id, TokenID::NOT);
  EXPECT_EQ(root(1)(0).token.id, TokenID::BOOL);
}

TEST_F(UTSyntaxTree, RemoveChild) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  root.AddNewChild(TokenID::OR);
  root.AddNewChild(TokenID::BOOL);
  root.AddNewChild(TokenID::BOOLEAN);
  ASSERT_EQ(root.ChildrenCount(), 3);

  root.RemoveChild(1);
  ASSERT_EQ(root.ChildrenCount(), 2);
  EXPECT_EQ(root(0).token.id, TokenID::OR);
  EXPECT_EQ(root(1).token.id, TokenID::BOOLEAN);

  root.RemoveChild(1);
  ASSERT_EQ(root.ChildrenCount(), 1);
  EXPECT_EQ(root(0).token.id, TokenID::OR);
}

TEST_F(UTSyntaxTree, ExtractChild) {
  const Token rootToken{ TokenID::AND, StrRange{2, 4}, TokenData{42} };
  SyntaxTree::Node root{ rootToken };
  const SyntaxTree::Node donor{ Token{ TokenID::OR, StrRange{2, 3}, TokenData{43} } };
  root.AddChildCopy(donor);
  root(0).AddChildCopy(donor);

  const auto extracted = root.ExtractChild(0);
  EXPECT_EQ(root.ChildrenCount(), 0);
  EXPECT_EQ(extracted->token, donor.token);
  EXPECT_EQ(extracted->At(0).token, donor.token);
}

TEST_F(UTSyntaxTree, RemoveAll) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  EXPECT_EQ(root.ChildrenCount(), 0);

  root.RemoveAll();
  EXPECT_EQ(root.ChildrenCount(), 0);

  root.AddNewChild(TokenID::OR);
  root.AddNewChild(TokenID::BOOL);
  root.AddNewChild(TokenID::BOOLEAN);
  EXPECT_EQ(root.ChildrenCount(), 3);

  root.RemoveAll();
  EXPECT_EQ(root.ChildrenCount(), 0);
}

TEST_F(UTSyntaxTree, NodeChildAccess) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  root.AddNewChild(TokenID::OR);
  root.AddNewChild(TokenID::BOOL);

  EXPECT_EQ(root(0).token.id, TokenID::OR);
  EXPECT_EQ(root(1).token.id, TokenID::BOOL);

  EXPECT_EQ(root.At(0).token.id, TokenID::OR);
  EXPECT_EQ(root.At(1).token.id, TokenID::BOOL);

  const auto& constRef = root;
  EXPECT_EQ(constRef(0).token.id, TokenID::OR);
  EXPECT_EQ(constRef(1).token.id, TokenID::BOOL);

  EXPECT_EQ(constRef.At(0).token.id, TokenID::OR);
  EXPECT_EQ(constRef.At(1).token.id, TokenID::BOOL);
}

TEST_F(UTSyntaxTree, NodeComparison) {
  const SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  auto copy = root;
  EXPECT_EQ(copy.token, root.token);
  EXPECT_EQ(copy, root);

  copy.token.pos = StrRange{0, 0};
  EXPECT_NE(copy.token, root.token);
  EXPECT_EQ(copy, root);

  copy = root;
  copy.token.id = TokenID::OR;
  EXPECT_NE(copy.token, root.token);
  EXPECT_NE(copy, root);

  copy = root;
  copy.token.data = TokenData{ false };
  EXPECT_NE(copy.token, root.token);
  EXPECT_NE(copy, root);
}

TEST_F(UTSyntaxTree, CursorAccess) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{0, 10} } };
  root.AddNewChild(TokenID::OR);
  const SyntaxTree::Cursor cursor{ root };
  const SyntaxTree::Cursor child{ root(0) };

  EXPECT_EQ(root.ChildrenCount(), cursor.ChildrenCount());
  EXPECT_EQ(cursor->id, TokenID::AND);
  EXPECT_EQ(child->id, TokenID::OR);
  EXPECT_EQ((*cursor).id, TokenID::AND);
  EXPECT_EQ(cursor(0).id, TokenID::OR);
  EXPECT_EQ(child.Parent().id, TokenID::AND);
}

TEST_F(UTSyntaxTree, CursorNavigation) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{} } };
  root.AddNewChild(TokenID::OR);
  root.AddNewChild(TokenID::BOOL);
  root(0).AddNewChild(TokenID::BOOLEAN);

  SyntaxTree::Cursor cursor{ root };
  EXPECT_EQ(cursor.ChildrenCount(), 2);
  EXPECT_TRUE(cursor.IsRoot());
  EXPECT_FALSE(cursor.MoveToParent());
  EXPECT_FALSE(cursor.MoveToNextSibling());
  EXPECT_FALSE(cursor.MoveToPrevSibling());

  cursor.MoveToChild(1);
  EXPECT_FALSE(cursor.IsRoot());
  EXPECT_EQ(cursor->id, TokenID::BOOL);
  EXPECT_TRUE(cursor.MoveToParent());
  EXPECT_EQ(cursor->id, TokenID::AND);

  cursor.MoveToChild(1);
  EXPECT_FALSE(cursor.MoveToNextSibling());
  EXPECT_TRUE(cursor.MoveToPrevSibling());
  EXPECT_EQ(cursor->id, TokenID::OR);
  EXPECT_FALSE(cursor.MoveToPrevSibling());
  EXPECT_TRUE(cursor.MoveToNextSibling());
  EXPECT_TRUE(cursor.MoveToPrevSibling());
  EXPECT_TRUE(cursor.MoveToParent());

  cursor.MoveToChild(0);
  cursor.MoveToChild(0);
  EXPECT_FALSE(cursor.IsRoot());
  EXPECT_EQ(cursor->id, TokenID::BOOLEAN);
}

TEST_F(UTSyntaxTree, CursorDispatchVisit) {
  const SyntaxTree::Node root{ Token{ TokenID::LIT_EMPTYSET, StrRange{0, 5} } };
  const SyntaxTree ast{ std::make_unique<SyntaxTree::Node>(root) };
  SyntaxTree::Cursor cursor{ root };
  MinimalVisitor visitor{};
  EXPECT_NO_THROW(cursor.DispatchVisit(visitor));
  EXPECT_NO_THROW(visitor.Run(ast));
}

TEST_F(UTSyntaxTree, FindMinimalNode) {
  SyntaxTree::Node root{ Token{ TokenID::AND, StrRange{0, 10} } };
  root.AddChildCopy(SyntaxTree::Node{ Token{ TokenID::OR, StrRange{0, 5} } });
  root.AddChildCopy(SyntaxTree::Node{ Token{ TokenID::BOOL, StrRange{6, 10} } });
  root(0).AddChildCopy(SyntaxTree::Node{ Token{ TokenID::BIGPR, StrRange{0, 3} } });
  root(0).AddChildCopy(SyntaxTree::Node{ Token{ TokenID::CARD, StrRange{4, 5} } });

  const SyntaxTree::Cursor cursor{root};
  EXPECT_FALSE(ccl::rslang::FindMinimalNode(cursor, StrRange{11, 11}).has_value());
  EXPECT_FALSE(ccl::rslang::FindMinimalNode(cursor, StrRange{-1, -1}).has_value());
  EXPECT_FALSE(ccl::rslang::FindMinimalNode(cursor, StrRange{0, 11}).has_value());

  EXPECT_EQ(ccl::rslang::FindMinimalNode(cursor, StrRange{ 0, 0 }).value()->id, TokenID::BIGPR);
  EXPECT_EQ(ccl::rslang::FindMinimalNode(cursor, StrRange{ 4, 5 }).value()->id, TokenID::CARD);
  EXPECT_EQ(ccl::rslang::FindMinimalNode(cursor, StrRange{ 3, 4 }).value()->id, TokenID::OR);
  EXPECT_EQ(ccl::rslang::FindMinimalNode(cursor, StrRange{ 0, 10 }).value()->id, TokenID::AND);
  EXPECT_EQ(ccl::rslang::FindMinimalNode(cursor, StrRange{ 5, 6 }).value()->id, TokenID::AND);
}

TEST_F(UTSyntaxTree, ASTAccess) {
  const SyntaxTree::Node root{ Token{ TokenID::EQUAL, StrRange{0, 10} } };
  const SyntaxTree ast{ std::make_unique<SyntaxTree::Node>(root) };
  EXPECT_EQ(*ast.Root(), root.token);
}

TEST_F(UTSyntaxTree, ASTCompare) {
  const auto ast1 = SyntaxTree{ std::make_unique<SyntaxTree::Node>(Token{ TokenID::EQUAL, StrRange{0, 10} }) };
  const auto astAnother = SyntaxTree{ std::make_unique<SyntaxTree::Node>(Token{ TokenID::AND, StrRange{0, 10} }) };
  const auto copy = ast1;
  EXPECT_EQ(ast1, copy);
  EXPECT_NE(ast1, astAnother);
}

TEST_F(UTSyntaxTree, AST2String) {
  SyntaxTree::Node root{ Token{ TokenID::EQUAL, StrRange{0, 10} } };
  root.AddChildCopy(SyntaxTree::Node{ Token{ TokenID::LIT_INTEGER, StrRange{0, 5}, TokenData{1} } });
  root.AddChildCopy(SyntaxTree::Node{ Token{ TokenID::LIT_INTEGER, StrRange{6, 10}, TokenData{2} } });
  const SyntaxTree ast{ std::make_unique<SyntaxTree::Node>(root) };
  EXPECT_EQ(ccl::rslang::AST2String::Apply(ast), "[=[1][2]]");
}