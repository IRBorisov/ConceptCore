#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RandomGenerator.hpp"
#include "RSLHelper.hpp"
#include "FakeObserver.hpp"

#include "ccl/semantic/RSForm.h"

#include <vector>
#include <unordered_set>
#include <set>
#include <list>

class UTRSForm : public ::testing::Test {
protected:
  using EntityUID = ccl::EntityUID;
  using RSForm = ccl::semantic::RSForm;
  using Constituenta = ccl::semantic::ConceptRecord;
  using CstType = ccl::semantic::CstType;
  using Grammem = ccl::lang::Grammem;
  using Morphology = ccl::lang::Morphology;

protected:
  RSForm schema{};
};

TEST_F(UTRSForm, ObserverNotification) {
  FakeObserver observer{};
  schema.AddObserver(observer);
  EXPECT_EQ(size(observer.events), 0U);
  const auto x1 = schema.Emplace(CstType::base);
  EXPECT_EQ(ssize(observer.events), 1);

  schema.SetExpressionFor(x1, "42");
  EXPECT_EQ(ssize(observer.events), 2);

  schema.SetExpressionFor(x1, "42");
  EXPECT_EQ(ssize(observer.events), 2);
}

TEST_F(UTRSForm, Attributes) {
  EXPECT_EQ("", schema.title);
  EXPECT_EQ("", schema.alias);
  EXPECT_EQ("", schema.comment);

  const std::string testString{ "42" };
  schema.title = testString;
  schema.alias = testString;
  schema.comment = testString;
  EXPECT_EQ(schema.title, testString);
  EXPECT_EQ(schema.alias, testString);
  EXPECT_EQ(schema.comment, testString);
}

TEST_F(UTRSForm, EraseTracking) {
  const auto x1 = schema.Emplace(CstType::base);
  schema.Mods().Track(x1);
  EXPECT_FALSE(schema.Erase(x1));
}

TEST_F(UTRSForm, TrackingEditText) {
  const auto d1 = schema.Emplace(CstType::term);
  schema.Mods().Track(d1);
  EXPECT_FALSE(schema.SetTermFor(d1, "D42"));
  EXPECT_FALSE(schema.SetTermFormFor(d1, "D42", Morphology{}));
  EXPECT_FALSE(schema.SetDefinitionFor(d1, "42"));
  EXPECT_FALSE(schema.SetConventionFor(d1, "42"));
}

TEST_F(UTRSForm, ImportFrom) {
  const auto oldSize = std::ssize(schema.Core());
  EXPECT_NO_THROW(schema.InsertCopy(ccl::VectorOfEntities{}, schema.Core()));
  EXPECT_EQ(std::ssize(schema.Core()), oldSize);

  RSForm other{};
  const auto d1 = other.Emplace(CstType::term);
  const auto s1 = other.Emplace(CstType::structured);
  const auto x1 = other.Emplace(CstType::base);
  schema.InsertCopy({ x1, s1, d1 }, other.Core() );
  EXPECT_EQ(std::ssize(schema.Core()), 3 + oldSize);
}

TEST_F(UTRSForm, HashInsertCst) {
  const auto main = schema.CoreHash();
  schema.Emplace(CstType::base);
  EXPECT_NE(schema.CoreHash(), main);
}

TEST_F(UTRSForm, HashCstName) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  schema.SetAliasFor(x1, "X2");

  EXPECT_NE(schema.CoreHash(), main);
  EXPECT_NE(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashCstExpr) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  schema.SetExpressionFor(x1, "X1");

  EXPECT_NE(schema.CoreHash(), main);
  EXPECT_NE(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashTerm) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  schema.SetTermFor(x1, "42");

  EXPECT_EQ(schema.CoreHash(), main);
  EXPECT_NE(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashTextDef) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  schema.SetDefinitionFor(x1, "42");

  EXPECT_EQ(schema.CoreHash(), main);
  EXPECT_NE(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashComment) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  schema.SetConventionFor(x1, "42");

  EXPECT_EQ(schema.CoreHash(), main);
  EXPECT_NE(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashReorder) {
  const auto x1 = schema.Emplace(CstType::base, "1=1");
  const auto x2 = schema.Emplace(CstType::base, "2=2");
  schema.SetDefinitionFor(x1, "42");
  schema.SetDefinitionFor(x2, "12");
  const auto main = schema.CoreHash();
  const auto full = schema.FullHash();

  ASSERT_TRUE(schema.MoveBefore(x2, schema.List().Find(x1)));
  EXPECT_EQ(schema.CoreHash(), main);
  EXPECT_EQ(schema.FullHash(), full);
}

TEST_F(UTRSForm, HashRemoveAdd) {
  schema.Emplace(CstType::base, "1=1");
  auto x2 = schema.Emplace(CstType::base, "2=2");
  for (auto i = 0; i < 10; ++i) {
    schema.Emplace(CstType::term, "Term" + std::to_string(i));
  }

  const auto coreHash = schema.CoreHash();
  const auto fulllHash = schema.FullHash();
  for (auto i = 0; i < 10; ++i) {
    schema.Erase(x2);
    x2 = schema.Emplace(CstType::base, "2=2");
    EXPECT_EQ(schema.CoreHash(), coreHash);
    EXPECT_EQ(schema.FullHash(), fulllHash);
  }
}

TEST_F(UTRSForm, HashSelfConsistency) {
  const auto x1 = schema.Emplace(CstType::base, "1=1");
  const auto x2 = schema.Emplace(CstType::base, "2=2");
  schema.SetDefinitionFor(x1, "42");
  schema.SetDefinitionFor(x2, "12");

  EXPECT_EQ(schema.CoreHash(), schema.CoreHash());
  EXPECT_EQ(schema.FullHash(), schema.FullHash());
}

TEST_F(UTRSForm, HashCopyConsistency) {
  const auto x1 = schema.Emplace(CstType::base, "1=1");
  const auto x2 = schema.Emplace(CstType::base, "2=2");
  schema.SetDefinitionFor(x1, "42");
  schema.SetDefinitionFor(x2, "12");

  const auto schema2 = schema;

  EXPECT_EQ(schema2.CoreHash(), schema.CoreHash());
  EXPECT_EQ(schema2.FullHash(), schema.FullHash());
}

TEST_F(UTRSForm, Assign) {
  const auto x1 = schema.Emplace(CstType::base);
  const auto s1 = schema.Emplace(CstType::structured, "42");
  schema.SetConventionFor(s1, "comment");
  schema.Mods().Track(x1);

  RSForm copy{};
  copy.Emplace(CstType::base);
  copy.Emplace(CstType::base);
  copy.Emplace(CstType::base);
  copy = schema;

  EXPECT_EQ(std::ssize(copy.Core()), std::ssize(schema.Core()));
  ASSERT_TRUE(copy.Contains(x1));
  ASSERT_TRUE(copy.Contains(s1));
  EXPECT_EQ(schema.GetRS(s1).definition, copy.GetRS(s1).definition);
  EXPECT_EQ(schema.GetRS(s1).convention, copy.GetRS(s1).convention);
  EXPECT_EQ(*schema.Mods()(x1), *copy.Mods()(x1));
}

TEST_F(UTRSForm, AssignObserversConsequences) {
  FakeObserver observer{};
  schema.AddObserver(observer);
  schema.Emplace(CstType::base);
  EXPECT_EQ(ssize(observer.events), 1);

  RSForm schema2{};
  FakeObserver observer2{};
  schema2.AddObserver(observer2);

  schema = schema2;
  EXPECT_EQ(ssize(observer.events), 2);
  EXPECT_EQ(ssize(observer2.events), 0);

  schema.Emplace(CstType::base);
  EXPECT_EQ(ssize(observer.events), 3);
  EXPECT_EQ(ssize(observer2.events), 0);
}