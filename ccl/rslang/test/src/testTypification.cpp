#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/rslang/RSToken.h"
#include "ccl/rslang/Typification.h"

#include <cmath>

class UTTypification : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using StructureType = ccl::rslang::StructureType;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;

protected:
  static inline const std::string uid1 = "X1";
  static inline const std::string uid2 = "X2";

  static inline const Typification basic = Typification(uid1);
  static inline const Typification anotherBasic = Typification(uid2);
};

TEST_F(UTTypification, Basic) {
  auto newBasic = Typification(uid1);
  EXPECT_EQ(basic, newBasic);
  EXPECT_NE(basic, anotherBasic);
  EXPECT_NE(basic, Typification::Integer());
  EXPECT_TRUE(newBasic.IsElement());
  EXPECT_FALSE(newBasic.IsCollection());
  EXPECT_FALSE(newBasic.IsTuple());
}

TEST_F(UTTypification, Tuple) {
  auto tuple = Typification::Tuple({ basic, basic });
  EXPECT_FALSE(tuple.IsElement());
  EXPECT_FALSE(tuple.IsCollection());
  EXPECT_TRUE(tuple.IsTuple());
  EXPECT_EQ(tuple.T().Arity(), 2);

  tuple = Typification::Tuple({ basic });
  EXPECT_TRUE(tuple.IsElement());
  EXPECT_EQ(tuple, basic);
}

TEST_F(UTTypification, AddComponent) {
  auto tuple = Typification::Tuple({ basic, basic });
  tuple.T().AddComponent(anotherBasic);
  EXPECT_EQ(tuple.T().Arity(), 3);
}

TEST_F(UTTypification, TupleSelf) {
  auto tuple = Typification::Tuple({ basic, basic });
  tuple.T().AddComponent(anotherBasic);
  tuple = Typification::Tuple({ tuple, basic });
  EXPECT_EQ(tuple.T().Arity(), 2);
  EXPECT_EQ(tuple.T().Component(Typification::PR_START).T().Arity(), 3);
}

TEST_F(UTTypification, TupleTripleVsComposite) {
  auto duplete = Typification::Tuple({ basic, basic });
  auto triplete = duplete;
  triplete.T().AddComponent(basic);
  auto duoAndOne = Typification::Tuple({ duplete, basic });
  auto oneAndDuo = Typification::Tuple({ basic, duplete });
  EXPECT_NE(triplete, duoAndOne);
  EXPECT_NE(triplete, oneAndDuo);
  EXPECT_NE(duoAndOne, oneAndDuo);
}

TEST_F(UTTypification, Bool) {
  const auto sBool = basic.Bool();
  EXPECT_FALSE(sBool.IsElement());
  EXPECT_TRUE(sBool.IsCollection());
  EXPECT_FALSE(sBool.IsTuple());
}

TEST_F(UTTypification, ElementTypification) {
  const auto sBool = basic.Bool();
  const auto elementType = sBool.B().Base();
  EXPECT_EQ(elementType, basic);
}

TEST_F(UTTypification, Debool) {
  auto sBool = basic;
  EXPECT_TRUE(sBool.ApplyBool().IsCollection());
  EXPECT_EQ(sBool.B().Base(), basic);
}

TEST_F(UTTypification, ToString) {
  const auto bool1 = basic.Bool().Bool();
  auto boolTuple = Typification::Tuple({ anotherBasic, bool1 });
  boolTuple.T().AddComponent(Typification::Tuple({ basic, basic }));
  boolTuple.ApplyBool();

  // B(X2*BB(X1)*(X1*X1))
  std::string toString = Token::Str(TokenID::BOOLEAN) + "(" + uid2 
    + Token::Str(TokenID::DECART) + Token::Str(TokenID::BOOLEAN) + Token::Str(TokenID::BOOLEAN) 
    + "(" + uid1 + ")" + Token::Str(TokenID::DECART) + "(" + uid1 + Token::Str(TokenID::DECART) + uid1 + "))";
  EXPECT_EQ(toString, boolTuple.ToString());
}

struct TypeVisitor {
  using StructureType = ccl::rslang::StructureType;
  using Typification = ccl::rslang::Typification;

  int32_t collections = 0;
  int32_t elements = 0;
  int32_t tuples = 0;

  void operator()(const Typification& type) {
    switch (type.Structure()) {
    case StructureType::basic: ++elements; break;
    case StructureType::collection: ++collections; break;
    case StructureType::tuple: ++tuples; break;
    }
  }
};

TEST_F(UTTypification, ConstVisit) {
  const auto bX1xX1 = Typification::Tuple({ basic, basic }).Bool();
  TypeVisitor visitor;
  bX1xX1.ConstVisit(visitor);
  EXPECT_EQ(visitor.collections, 1);
  EXPECT_EQ(visitor.tuples, 1);
  EXPECT_EQ(visitor.elements, 2);
}

TEST_F(UTTypification, SubstituteBase) {
  Typification::Substitutes substitutes{};
  substitutes.insert({ uid1, anotherBasic.Bool() });
  
  auto test = basic;
  test.SubstituteBase(substitutes);
  EXPECT_EQ(test, anotherBasic.Bool());

  test = basic.Bool();
  test.SubstituteBase(substitutes);
  EXPECT_EQ(test, anotherBasic.Bool().Bool());

  test = Typification::Tuple({ basic, anotherBasic });
  test.SubstituteBase(substitutes);
  EXPECT_EQ(test, Typification::Tuple({ anotherBasic.Bool(), anotherBasic }));
}