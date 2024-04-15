#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/CstList.h"
#include "ccl/semantic/CstFilters.hpp"
#include "RSLHelper.hpp"

#include <vector>

class UTCstList : public ::testing::Test {
protected:
  using CstList = ccl::semantic::CstList;
  using ListIterator = ccl::semantic::ListIterator;
  using CstType = ccl::semantic::CstType;
  using EntityUID = ccl::EntityUID;
  using VectorOfEntities = ccl::VectorOfEntities;

protected:
  static constexpr EntityUID x1{ 42 };
  static constexpr EntityUID x2{ 43 };
  static constexpr EntityUID c1{ 44 };
  static constexpr EntityUID c2{ 45 };
  static constexpr EntityUID s1{ 46 };
  static constexpr EntityUID s2{ 47 };
  static constexpr EntityUID d1{ 48 };
  static constexpr EntityUID d2{ 49 };
  static constexpr EntityUID a1{ 50 };
  static constexpr EntityUID a2{ 51 };
  static constexpr EntityUID t1{ 52 };
  static constexpr EntityUID t2{ 53 };
  static constexpr EntityUID f1{ 54 };
  static constexpr EntityUID f2{ 55 };
  static constexpr EntityUID p1{ 56 };
  static constexpr EntityUID p2{ 57 };
  static constexpr EntityUID invalid{ 13 };

  static inline const std::unordered_map<EntityUID, CstType> types = { 
    {x1, CstType::base}, {x2, CstType::base}, {c1, CstType::constant}, {c2, CstType::constant}, {s1, CstType::structured}, {s2, CstType::structured},
    {d1, CstType::term}, {d2, CstType::term}, {a1, CstType::axiom}, {a2, CstType::axiom}, {t1, CstType::theorem}, {t2, CstType::theorem},
    {f1, CstType::function}, {f2, CstType::function}, {p1, CstType::predicate}, {p2, CstType::predicate}
  };

  static inline const auto typesLoader = [&types = UTCstList::types](const EntityUID target) -> CstType { 
    return types.contains(target) ? types.at(target) : CstType::base;
  };

  static inline const auto mockLoader = [&types = UTCstList::types](const EntityUID /*target*/) -> CstType {
    if (std::empty(types)) {
      return CstType::axiom;
    } else {
      return CstType::base;
    }
  };

  CstList list{ typesLoader };

  void SetupAll();
};

void UTCstList::SetupAll() {
  list.Insert(x1);
  list.Insert(x2);
  list.Insert(c1);
  list.Insert(c2);
  list.Insert(s1);
  list.Insert(s2);
  list.Insert(d1);
  list.Insert(d2);
  list.Insert(a1);
  list.Insert(a2);
  list.Insert(t1);
  list.Insert(t2);
  list.Insert(f1);
  list.Insert(f2);
  list.Insert(p1);
  list.Insert(p2);
}

TEST_F(UTCstList, Construction) {
  {
    EXPECT_EQ(std::size(list), 0U);
    EXPECT_EQ(list.Find(x1), std::end(list));
  }

  list.Insert(x1);
  list.Insert(a1);
  EXPECT_NE(list.Find(x1), std::end(list));
  EXPECT_EQ(std::size(list), 2U);
  {
    CstList copy{ list };
    EXPECT_EQ(std::size(copy), 2U);
    EXPECT_NE(copy.Find(x1), std::end(copy));
  }
  {
    CstList copyCtor{ typesLoader };
    copyCtor = list;
    EXPECT_EQ(std::size(copyCtor), 2U);
    EXPECT_NE(copyCtor.Find(x1), std::end(copyCtor));
  }
  {
    CstList temp{ list };
    CstList movedCopy{ std::move(temp) };
    EXPECT_EQ(std::size(movedCopy), 2U);
    EXPECT_NE(movedCopy.Find(x1), std::end(movedCopy));
  }
  {
    CstList temp{ list };
    CstList moveAss{ typesLoader };
    moveAss = std::move(temp);
    EXPECT_EQ(std::size(moveAss), 2U);
    EXPECT_NE(moveAss.Find(x1), std::end(moveAss));
  }
}

TEST_F(UTCstList, STLapi) {
  {
    EXPECT_TRUE(std::empty(list));
    EXPECT_EQ(std::size(list), 0U);
    EXPECT_EQ(std::begin(list), std::end(list));
  }

  list.Insert(x1);
  list.Insert(x2);
  list.Insert(a1);
  list.Insert(f1);
  {
    ASSERT_EQ(std::size(list), 4U);
    EXPECT_FALSE(std::empty(list));
    EXPECT_NE(std::begin(list), std::end(list));
  }
  {
    VectorOfEntities expected{ x1, x2, a1, f1 };
    VectorOfEntities result{};
    for (const auto uid : list) {
      result.emplace_back(uid);
    }
    EXPECT_EQ(result, expected);
  }
}

TEST_F(UTCstList, Find) {
  EXPECT_EQ(list.Find(x1), std::end(list));
  list.Insert(x1);
  EXPECT_EQ(list.Find(invalid), std::end(list));
  ASSERT_EQ(list.Find(x1), std::begin(list));
  EXPECT_EQ(list.Find(x1).operator*(), x1);
}

TEST_F(UTCstList, Insert) {
  {
    CstList test{ list };
    test.Insert(x1);
    ASSERT_EQ(std::size(test), 1U);
    EXPECT_EQ(*std::begin(test), x1);

    test.Insert(x1);
    ASSERT_EQ(std::size(test), 2U);
    EXPECT_EQ(*std::begin(test), x1);
    EXPECT_EQ(*std::next(std::begin(test)), x1);
  }
  {
    CstList noTypes{ mockLoader };
    noTypes.Insert(d1);
    noTypes.Insert(x1);
    EXPECT_EQ(*std::begin(noTypes), d1);
    EXPECT_EQ(*std::next(std::begin(noTypes)), x1);
  }
  {
    CstList typedList{ typesLoader };
    typedList.Insert(d1);
    typedList.Insert(x1);
    EXPECT_EQ(*std::begin(typedList), x1);
    EXPECT_EQ(*std::next(std::begin(typedList)), d1);
  }
}

TEST_F(UTCstList, Erase) {
  {
    ASSERT_NO_THROW(list.Erase(invalid));
    EXPECT_EQ(std::size(list), 0U);
  }
  {
    CstList test{ list };
    test.Insert(x1);
    test.Insert(x2);
    test.Insert(x1);
    test.Erase(x1);
    ASSERT_EQ(std::size(test), 1U);
    EXPECT_EQ(*std::begin(test), x2);
  }
  {
    CstList test{ list };
    test.Insert(x1);
    test.Insert(x2);
    test.Insert(d1);
    test.Erase(x2);
    ASSERT_EQ(std::size(test), 2U);
    EXPECT_EQ(*std::begin(test), x1);
    EXPECT_EQ(*std::next(std::begin(test)), d1);
  }
}

TEST_F(UTCstList, MoveBefore) {
  SetupAll();
  EXPECT_FALSE(list.MoveBefore(invalid, std::end(list)));
  EXPECT_FALSE(list.MoveBefore(invalid, list.Find(x1)));

  EXPECT_TRUE(list.MoveBefore(x1, list.Find(x1)));
  EXPECT_TRUE(list.MoveBefore(x2, list.Find(x1)));
  EXPECT_TRUE(list.MoveBefore(d1, list.Find(d1)));
  EXPECT_TRUE(list.MoveBefore(d2, list.Find(d1)));
  EXPECT_TRUE(list.MoveBefore(d1, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(d1, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(a1, list.Find(d1)));
  EXPECT_TRUE(list.MoveBefore(a2, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(t1, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(f1, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(p1, list.Find(a1)));
  EXPECT_TRUE(list.MoveBefore(p2, list.Find(p1)));
  EXPECT_TRUE(list.MoveBefore(x1, list.Find(c1)));

  EXPECT_FALSE(list.MoveBefore(c1, list.Find(x1)));
  EXPECT_FALSE(list.MoveBefore(x1, list.Find(d1)));
  EXPECT_FALSE(list.MoveBefore(d1, list.Find(x1)));
  EXPECT_FALSE(list.MoveBefore(s1, list.Find(x1)));

  EXPECT_FALSE(list.MoveBefore(x1, std::end(list)));
  EXPECT_FALSE(list.MoveBefore(c1, std::end(list)));
  EXPECT_FALSE(list.MoveBefore(s1, std::end(list)));
  EXPECT_TRUE(list.MoveBefore(a1, std::end(list)));
  EXPECT_TRUE(list.MoveBefore(d1, std::end(list)));
  EXPECT_TRUE(list.MoveBefore(f1, std::end(list)));
  EXPECT_TRUE(list.MoveBefore(t1, std::end(list)));
}

TEST_F(UTCstList, SortSubset) {
  SetupAll();
  const ccl::SetOfEntities emptyList{};
  const auto sortedEmpty = list.SortSubset(emptyList);
  EXPECT_EQ(ssize(sortedEmpty), 0);

  const ccl::SetOfEntities idList{ x1, d1, s1, a1, c1 };
  const auto sorted = list.SortSubset(idList);
  const VectorOfEntities expected{ x1, c1, s1, d1, a1 };
  EXPECT_EQ(sorted, expected);
}

TEST_F(UTCstList, SortSubsetInvalid) {
  SetupAll();
  ccl::SetOfEntities idList{ x1 };
  idList.insert(invalid);
  const auto sorted = list.SortSubset(idList);
  EXPECT_EQ(ssize(idList) - 1, ssize(sorted));
  EXPECT_EQ(*begin(sorted), x1);
}