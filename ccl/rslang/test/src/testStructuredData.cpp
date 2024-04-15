#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/StructuredData.h"

#include <cmath>

class UTStructuredData : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using StructuredData = ccl::object::StructuredData;
  using Factory = ccl::object::Factory;

protected:
  static constexpr int32_t val1 = 3;
  static constexpr int32_t val2 = 5;
  static constexpr int32_t val3 = 68;

  static inline const StructuredData empty = Factory::EmptySet();
  static inline const StructuredData el1 = Factory::Val(val1);
  static inline const StructuredData el2 = Factory::Val(val2);
  static inline const StructuredData el3 = Factory::Val(val3);
};

TEST_F(UTStructuredData, DefaultConstruction) {
  StructuredData def{};
  EXPECT_EQ(empty, def);
  EXPECT_TRUE(def.B().IsEmpty());
  EXPECT_TRUE(def.IsCollection());
}

TEST_F(UTStructuredData, EmptySetIdentification) {
  EXPECT_EQ(empty, empty);
  EXPECT_EQ(empty, Factory::EmptySet());

  EXPECT_FALSE(empty.IsElement());
  EXPECT_TRUE(empty.IsCollection());
  EXPECT_FALSE(empty.IsTuple());

  EXPECT_TRUE(empty.B().IsEmpty());
}

TEST_F(UTStructuredData, EmptySetSubiterator) {
  const auto iter = std::begin(empty.B());
  EXPECT_EQ(std::end(empty.B()), iter);
}

TEST_F(UTStructuredData, TerminalIdentification) {
  const auto copy = el1;
  EXPECT_EQ(el1, el1);
  EXPECT_EQ(copy, el1);
  EXPECT_NE(el1, el2);

  EXPECT_TRUE(el1.IsElement());
  EXPECT_FALSE(el1.IsCollection());
  EXPECT_FALSE(el1.IsTuple());
}

TEST_F(UTStructuredData, TupleIdentification) {
  const auto tuple1 = Factory::Tuple({ el1 , el2 });
  const auto sameTuple = Factory::Tuple({ el1 , el2 });
  const auto tuple2 = Factory::Tuple({ el2 , el1 });
  EXPECT_EQ(tuple1, tuple1);
  EXPECT_EQ(tuple1, sameTuple);
  EXPECT_NE(tuple1, tuple2);

  EXPECT_FALSE(tuple1.IsElement());
  EXPECT_FALSE(tuple1.IsCollection());
  EXPECT_TRUE(tuple1.IsTuple());
  EXPECT_EQ(tuple1.T().Arity(), 2);
}

TEST_F(UTStructuredData, TupleArity) {
  const auto dualTuple = Factory::Tuple({ el1 , el2 });
  EXPECT_EQ(dualTuple.T().Arity(), 2);

  const auto tripleTuple = Factory::Tuple({ el1, el1 , el2 });
  EXPECT_EQ(tripleTuple.T().Arity(), 3);
}

TEST_F(UTStructuredData, TupleArityEmptySet) {
  const auto tuple = Factory::Tuple({ el1 , empty });
  EXPECT_EQ(tuple.T().Arity(), 2);
}

TEST_F(UTStructuredData, TupleFromElement) {
  EXPECT_EQ(Factory::Tuple({ el1 }), el1);
}

TEST_F(UTStructuredData, TupleComponent) {
  const auto tuple = Factory::Tuple({ el1 , el2 });
  EXPECT_EQ(tuple.T().Component(Typification::PR_START), el1);
  EXPECT_EQ(tuple.T().Component(Typification::PR_START + 1), el2);
}

TEST_F(UTStructuredData, TupleComposite) {
  const auto triplete = Factory::Tuple({ el1, el2, el1 });
  const auto twoAndOne = Factory::Tuple({ Factory::Tuple({ el1, el2 }), el1 });
  const auto oneAndTwo = Factory::Tuple({ el1, Factory::Tuple({ el2, el1 }) });
  EXPECT_NE(twoAndOne, oneAndTwo);
  EXPECT_NE(triplete, twoAndOne);
  EXPECT_NE(triplete, oneAndTwo);
}

TEST_F(UTStructuredData, SetIdentification) {
  const auto singleton = Factory::Set({ el1 });
  EXPECT_FALSE(singleton.IsElement());
  EXPECT_TRUE(singleton.IsCollection());
  EXPECT_FALSE(singleton.IsTuple());
  EXPECT_EQ(singleton.B().Cardinality(), 1);

  const auto sameSingleton = Factory::Set({ el1, el1 });
  EXPECT_EQ(singleton, sameSingleton);

  const auto multiElement = Factory::Set({ el1, el2 });
  EXPECT_EQ(multiElement.B().Cardinality(), 2);

  const auto sameMultiElement = Factory::Set({ el2, el1 });
  EXPECT_EQ(multiElement, sameMultiElement);
}

TEST_F(UTStructuredData, SetEmptyEnumeration) {
  const auto emptyEnum = Factory::SetV(std::vector<int32_t>{});
  const auto emptyEnum2 = Factory::Set(std::vector<StructuredData>{});
  EXPECT_EQ(emptyEnum, empty);
  EXPECT_EQ(emptyEnum2, empty);
}

TEST_F(UTStructuredData, SetAddBasic) {
  auto dataSet = empty;
  EXPECT_TRUE(dataSet.ModifyB().AddElement(el1));
  EXPECT_FALSE(dataSet.ModifyB().AddElement(el1));
  EXPECT_TRUE(dataSet.ModifyB().AddElement(el2));
}

TEST_F(UTStructuredData, SetAddEmpty) {
  auto dataSet = empty;
  EXPECT_TRUE(dataSet.ModifyB().AddElement(empty));
  EXPECT_FALSE(dataSet.ModifyB().AddElement(empty));
}

TEST_F(UTStructuredData, SetAddSecondElementToSetWithEmptySet) {
  auto dataSet = empty;
  auto anotherSet = Factory::Set({ el1, el2 });
  EXPECT_TRUE(dataSet.ModifyB().AddElement(empty));
  EXPECT_TRUE(dataSet.ModifyB().AddElement(anotherSet));
}

TEST_F(UTStructuredData, SetAddSet) {
  auto dataSuperSet = Factory::Singleton(Factory::Singleton(el1));
  auto anotherSet = Factory::Singleton(el2);
  EXPECT_TRUE(dataSuperSet.ModifyB().AddElement(anotherSet));
  EXPECT_FALSE(dataSuperSet.ModifyB().AddElement(anotherSet));
}

TEST_F(UTStructuredData, SetAddElementForEntity) {
  auto dataSet = Factory::Singleton(el1);
  EXPECT_TRUE(dataSet.ModifyB().AddElement(Factory::Val(val2)));
  EXPECT_FALSE(dataSet.ModifyB().AddElement(Factory::Val(val2)));
}

TEST_F(UTStructuredData, SetCardinality) {
  const auto dataSet = Factory::Set({ el1, el2 });
  EXPECT_EQ(dataSet.B().Cardinality(), 2);
}

TEST_F(UTStructuredData, SetCardinalityEmpty) {
  const auto zero = empty;
  const auto one = Factory::Set({ empty, zero });
  const auto two = Factory::Set({ empty, one });
  const auto three = Factory::Set({ empty, two });
  EXPECT_EQ(zero.B().Cardinality(), 0);
  EXPECT_EQ(one.B().Cardinality(), 1);
  EXPECT_EQ(two.B().Cardinality(), 2);
  EXPECT_EQ(three.B().Cardinality(), 2);
}

TEST_F(UTStructuredData, SetIteration) {
  const auto dataSet = Factory::Set({ el1, el2 });
  auto iter = std::begin(dataSet.B());
  EXPECT_EQ(*iter, el1);
  ++iter;
  EXPECT_EQ(*iter, el2);
  ++iter;
  EXPECT_EQ(iter, std::end(dataSet.B()));
}

TEST_F(UTStructuredData, SetContains) {
  const auto dataSet = Factory::Set({ el1 });
  const auto copyElement = el1;
  const auto recreatedElement = Factory::Val(val1);
  EXPECT_TRUE(dataSet.B().Contains(el1));
  EXPECT_TRUE(dataSet.B().Contains(copyElement));
  EXPECT_TRUE(dataSet.B().Contains(recreatedElement));
  EXPECT_FALSE(dataSet.B().Contains(el2));

  EXPECT_FALSE(empty.B().Contains(el1));
  EXPECT_FALSE(empty.B().Contains(empty));
}

TEST_F(UTStructuredData, SetIsSubsetOrEq) {
  const auto dataSet = Factory::Set({ el1, el2 });
  const auto subSet = Factory::Set({ el1 });
  EXPECT_TRUE(subSet.B().IsSubsetOrEq(dataSet.B()));
  EXPECT_TRUE(dataSet.B().IsSubsetOrEq(dataSet.B()));
  EXPECT_FALSE(dataSet.B().IsSubsetOrEq(subSet.B()));
}

TEST_F(UTStructuredData, SetUnionNoIntersecion) {
  const auto dataSet1 = Factory::Set({ el1, el2 });
  const auto dataSet2 = Factory::Set({ el3 });
  const auto result = Factory::Set({ el1, el2, el3 });
  EXPECT_EQ(dataSet1.B().Union(dataSet2.B()), result);
  EXPECT_EQ(dataSet2.B().Union(dataSet1.B()), result);
}

TEST_F(UTStructuredData, SetUnionIntersecting) {
  const auto dataSet1 = Factory::Set({ el1, el2 });
  const auto dataSet2 = Factory::Set({ el1, el3 });
  const auto result = Factory::Set({ el1, el2, el3 });
  EXPECT_EQ(dataSet1.B().Union(dataSet2.B()), result);
  EXPECT_EQ(dataSet2.B().Union(dataSet1.B()), result);
}

TEST_F(UTStructuredData, SetUnionSelf) {
  const auto dataSet = Factory::Set({ el1 });
  EXPECT_EQ(dataSet.B().Union(dataSet.B()), dataSet);
}

TEST_F(UTStructuredData, SetIntersect) {
  const auto dataSet = Factory::Set({ el1, el2 });
  const auto subset = Factory::Set({ el1 });
  const auto result = Factory::Set({ el1 });
  EXPECT_EQ(dataSet.B().Intersect(subset.B()), result);
  EXPECT_EQ(subset.B().Intersect(dataSet.B()), result);
}

TEST_F(UTStructuredData, SetDiff) {
  const auto dataSet = Factory::Set({ el1, el2 });
  const auto subset = Factory::Set({ el1 });
  const auto result = Factory::Set({ el2 });
  EXPECT_EQ(dataSet.B().Diff(subset.B()), result);
  EXPECT_EQ(subset.B().Diff(dataSet.B()), empty);
  EXPECT_EQ(subset.B().Diff(result.B()), subset);
}

TEST_F(UTStructuredData, SetSymDiff) {
  const auto dataSet = Factory::Set({ el1, el2 });
  const auto subset = Factory::Set({ el1, el3 });
  const auto result = Factory::Set({ el2, el3 });
  EXPECT_EQ(dataSet.B().SymDiff(subset.B()), result);
  EXPECT_EQ(subset.B().SymDiff(dataSet.B()), result);
}

TEST_F(UTStructuredData, SetOpetaionsOnEmpty) {
  const auto dataSet = Factory::Set({ el1 });
  EXPECT_EQ(dataSet.B().Union(empty.B()), dataSet);
  EXPECT_EQ(dataSet.B().Intersect(empty.B()), empty);
  EXPECT_EQ(dataSet.B().Diff(empty.B()), dataSet);
  EXPECT_EQ(dataSet.B().SymDiff(empty.B()), dataSet);
  EXPECT_EQ(empty.B().Union(empty.B()), empty);
  EXPECT_EQ(empty.B().Intersect(empty.B()), empty);
  EXPECT_EQ(empty.B().Diff(empty.B()), empty);
  EXPECT_EQ(empty.B().SymDiff(empty.B()), empty);
}

TEST_F(UTStructuredData, SetSingleton) {
  const auto singleton = Factory::Singleton(el1);
  auto regularSet = empty;
  regularSet.ModifyB().AddElement(el1);
  const auto enumeration = Factory::Set({ el1 });
  EXPECT_EQ(singleton, regularSet);
  EXPECT_EQ(singleton, enumeration);
}

TEST_F(UTStructuredData, SetReduce) {
  const auto bSet1 = Factory::Set({ el1, el2 });
  const auto bSet2 = Factory::Set({ el1 });
  const auto bSet3 = Factory::Set({ el2, el3 });
  const auto bbSet = Factory::Set({ bSet1, bSet2, bSet3 });
  const auto result = Factory::Set({ el1, el2, el3 });
  EXPECT_EQ(bbSet.B().Reduce(), result);
}

TEST_F(UTStructuredData, SetDebool) {
  const auto bSet1 = Factory::Set({ el1 });
  const auto bSet2 = Factory::Set({ Factory::Set( {el1} ) });
  const auto bSet3 = Factory::Set({ Factory::Tuple({el2, el3}) });
  EXPECT_EQ(bSet1.B().Debool(), el1);
  EXPECT_EQ(bSet2.B().Debool(), Factory::Set({ el1 }));
  EXPECT_EQ(bSet3.B().Debool(), Factory::Tuple({ el2, el3 }));
}

TEST_F(UTStructuredData, SetProjection) {
  const auto tuple1 = Factory::Tuple({ el1, el2 });
  const auto tuple2 = Factory::Tuple({ el1, el3 });
  const auto bTuple = Factory::Set({ tuple1 , tuple2 });
  const auto pr1 = Factory::Set({ el1 });
  const auto pr2 = Factory::Set({ el2, el3 });
  EXPECT_EQ(bTuple.B().Projection({ Typification::PR_START }), pr1);
  EXPECT_EQ(bTuple.B().Projection({ Typification::PR_START + 1 }), pr2);
  EXPECT_EQ(bTuple.B().Projection({ Typification::PR_START, Typification::PR_START + 1 }), bTuple);
  EXPECT_NE(bTuple.B().Projection({ Typification::PR_START + 1, Typification::PR_START }), bTuple);
}

TEST_F(UTStructuredData, DecartianCreate) {
  const auto bSet1 = Factory::Set({ el1, el2 });
  const auto bSet2 = Factory::Set({ el1, el2, el3 });
  const auto decart = Factory::Decartian({ bSet1, bSet2 });
  EXPECT_TRUE(decart.IsCollection());
  EXPECT_TRUE(std::begin(decart.B())->IsTuple());
  EXPECT_EQ(bSet1.B().Cardinality() * bSet2.B().Cardinality(), decart.B().Cardinality());
}

TEST_F(UTStructuredData, DecartianAddElement) {
  const auto bSet1 = Factory::Set({ el1 });
  const auto bSet2 = Factory::Set({ el1 });
  auto decart = Factory::Decartian({ bSet1, bSet2 });
  EXPECT_FALSE(decart.ModifyB().AddElement(Factory::Set({ el1 })));
  EXPECT_FALSE(decart.ModifyB().AddElement(Factory::Tuple({ el1, el1 })));
  EXPECT_FALSE(decart.ModifyB().AddElement(Factory::Tuple({ el1, el2 })));
  EXPECT_FALSE(decart.ModifyB().AddElement(Factory::EmptySet()));
}

TEST_F(UTStructuredData, DecartianEmpty) {
  const auto bSet1 = Factory::Set({ el1, el2 });
  const auto bSet2 = Factory::EmptySet();
  EXPECT_EQ(Factory::Decartian({ bSet1, bSet2 }), Factory::EmptySet());
}

TEST_F(UTStructuredData, DecartianAsymmetry) {
  const auto bSet1 = Factory::Singleton(el1);
  const auto bSet2 = Factory::Singleton(el3);
  const auto decart12 = Factory::Decartian({ bSet1, bSet2 });
  const auto decart21 = Factory::Decartian({ bSet2, bSet1 });
  EXPECT_NE(decart12, decart21);
}

TEST_F(UTStructuredData, DecartianContains) {
  const auto bSet1 = Factory::Set({ el1, el2 });
  const auto bSet2 = Factory::Set({ el1, el2, el3 });
  const auto decart = Factory::Decartian({ bSet1, bSet2 });
  EXPECT_TRUE(decart.B().Contains(Factory::Tuple({ el1, el3 })));
  EXPECT_FALSE(decart.B().Contains(Factory::Tuple({ el3, el3 })));
  EXPECT_FALSE(decart.B().Contains(Factory::Tuple({ el1, el3, el3 })));
}

TEST_F(UTStructuredData, DecartianIteration) {
  const auto bSet1 = Factory::Set({ el1, el2 });
  const auto bSet2 = Factory::Set({ el1, el2, el3 });
  const auto decart = Factory::Decartian({ bSet1, bSet2 });
  auto elements = std::set<StructuredData>{};
  for (const auto& el : decart.B()) {
    elements.insert(el);
  }
  EXPECT_EQ(decart.B().Cardinality(), ssize(elements));
}

TEST_F(UTStructuredData, BooleanCreate) {
  const auto bSet = Factory::Set({ el1, el2, el3 });
  const auto boolean = Factory::Boolean(bSet);
  EXPECT_TRUE(boolean.IsCollection());
  EXPECT_TRUE(std::begin(boolean.B())->IsCollection());
  EXPECT_EQ(std::pow(2, bSet.B().Cardinality()), boolean.B().Cardinality());
}

TEST_F(UTStructuredData, BooleanAddElement) {
  const auto bSet = Factory::Set({ el1});
  auto boolean = Factory::Boolean(bSet);
  EXPECT_FALSE(boolean.ModifyB().AddElement(Factory::Set({ el1 })));
  EXPECT_FALSE(boolean.ModifyB().AddElement(Factory::EmptySet()));
}

TEST_F(UTStructuredData, BooleanOverflow) {
  auto bSet = empty;
  for (auto i = 0; i <= StructuredData::BOOL_INFINITY; ++i) {
    bSet.ModifyB().AddElement(Factory::Val(i));
  }
  const auto boolean = Factory::Boolean(bSet);
  EXPECT_EQ(boolean.B().Cardinality(), StructuredData::SET_INFINITY);
}

TEST_F(UTStructuredData, BooleanIteration) {
  const auto bSet = Factory::Set({ el1, el2, el3 });
  const auto boolean1 = Factory::Boolean(bSet);
  const auto boolean2 = Factory::Boolean(bSet);

  auto elements = std::set<StructuredData>{};
  for (const auto& el : boolean1.B()) {
    elements.insert(el);
  }
  EXPECT_EQ(boolean1.B().Cardinality(), ssize(elements));

  elements.clear();
  for (const auto& el : boolean2.B()) {
    elements.insert(el);
  }
  EXPECT_EQ(boolean2.B().Cardinality(), ssize(elements));
}

TEST_F(UTStructuredData, BooleanIterationCacheOverflow) {
  auto base = Factory::EmptySet();
  for (auto i = 0; i < 10; i++) {
    base.ModifyB().AddElement(Factory::Val(i));
  }
  const auto boolean = Factory::Boolean(base);
  auto elements = Factory::EmptySet();
  for (const auto& el : boolean.B()) {
    elements.ModifyB().AddElement(el);
  }
  EXPECT_EQ(elements, boolean);
}

TEST_F(UTStructuredData, ElementOrdering) {
  EXPECT_EQ(el1.E().Value() < el2.E().Value(), el1 < el2);
}

TEST_F(UTStructuredData, SetOrdering) {
  const auto triSet = Factory::Set({ el1, el2, el3 });
  const auto duoSet1 = Factory::Set({ el2, el3 });
  const auto duoSet2 = Factory::Set({ el1, el2 });

  EXPECT_TRUE(empty < Factory::Singleton(el1));
  EXPECT_TRUE(duoSet1 < triSet);
  EXPECT_TRUE(duoSet2 < duoSet1);
}

TEST_F(UTStructuredData, TupleOrdering) {
  const auto tuple1 = Factory::Tuple({ el1, el1 });
  const auto tuple2 = Factory::Tuple({ el1, el2 });
  const auto tuple3 = Factory::Tuple({ el2, el1 });
  EXPECT_TRUE(tuple1 < tuple2);
  EXPECT_TRUE(tuple1 < tuple3);
  EXPECT_TRUE(tuple2 < tuple3);
}

TEST_F(UTStructuredData, ToString) {
  EXPECT_EQ(empty.ToString(), "{}");
  EXPECT_EQ(el1.ToString(), "3");

  auto bSet = Factory::Set({ el1, el2 });
  EXPECT_EQ(bSet.ToString(), "{3, 5}");

  auto tuple = Factory::Tuple({ el1, Factory::Tuple({ el1, el2 }) });
  EXPECT_EQ(tuple.ToString(), "(3, (3, 5))");

  auto bEmpty = Factory::Boolean(Factory::Boolean(empty));
  EXPECT_EQ(bEmpty.ToString(), "{{}, {{}}}");
}

TEST_F(UTStructuredData, DataCompatibility) {
  using ccl::object::CheckCompatible;

  EXPECT_TRUE(CheckCompatible(el1, "X1"_t));
  EXPECT_FALSE(CheckCompatible(el1, "B(X1)"_t));
  EXPECT_FALSE(CheckCompatible(el1, "X1*X1"_t));

  const auto tuple = Factory::Tuple({ el1, el2 });
  EXPECT_TRUE(CheckCompatible(tuple, "X1*X1"_t));
  EXPECT_FALSE(CheckCompatible(tuple, "X1*X1*X1"_t));

  const auto bSet = Factory::Set({ el1, el2 });
  EXPECT_TRUE(CheckCompatible(bSet, "B(X1)"_t));

  EXPECT_FALSE(CheckCompatible(empty, "X1"_t));
  EXPECT_TRUE(CheckCompatible(empty, "B(X1)"_t));
}