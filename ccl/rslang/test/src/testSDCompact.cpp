#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeRSEnvironment.hpp"

#include "ccl/rslang/RSToken.h"
#include "ccl/rslang/SDataCompact.h"

#include <cmath>

class UTSDCompact : public ::testing::Test {
protected:
  using Typification = ccl::rslang::Typification;
  using StructuredData = ccl::object::StructuredData;
  using SDCompact = ccl::object::SDCompact;
  using Factory = ccl::object::Factory;

protected:
  static constexpr int32_t val1 = 1;
  static constexpr int32_t val2 = 5;
  static constexpr int32_t val3 = 68;

  static inline const StructuredData empty = Factory::EmptySet();
  static inline const StructuredData element1 = Factory::Val(val1);
  static inline const StructuredData element2 = Factory::Val(val2);
  static inline const StructuredData element3 = Factory::Val(val3);
};

TEST_F(UTSDCompact, CreateHeader) {
  const auto header = std::vector<std::string>{ "B", "X2", "B", "X1" };
  EXPECT_EQ(SDCompact::CreateHeader("B(X2*B(X1))"_t), header);
}

TEST_F(UTSDCompact, Empty) {
  const auto type = "B(X1)"_t;
  const SDCompact::Data compact = { {0, 0} };
  EXPECT_EQ(SDCompact::FromSData(empty, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type).value(), empty);
}

TEST_F(UTSDCompact, StructuredEmpty) {
  const auto type = "B(B(X1)*B(X1))"_t;
  const SDCompact::Data compact = { {0, 0, 0, 0, 0} };
  EXPECT_EQ(SDCompact::FromSData(empty, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type).value(), empty);
  EXPECT_EQ(SDCompact(type, compact).Unpack(type).value(), empty);
}

TEST_F(UTSDCompact, Element) {
  const SDCompact::Data compact = { { val1 } };
  EXPECT_EQ(SDCompact::FromSData(element1, "X1"_t).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, "X1"_t).value(), element1);
  EXPECT_FALSE(SDCompact::Unpack(compact, "B(X1)"_t).has_value());
}

TEST_F(UTSDCompact, Tuple) {
  const SDCompact::Data compact = { { val1, val2 } };
  const auto type = "X1*X1"_t;
  const auto dataRS = Factory::Tuple({ element1, element2 });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
  EXPECT_FALSE(SDCompact::Unpack(compact, "X1*X1*X1"_t).has_value());
}

TEST_F(UTSDCompact, Set) {
  const SDCompact::Data compact = {
    { 2, val1 },
    { 2, val2 } };
  const auto type = "B(X1)"_t;
  const auto dataRS = Factory::Set({ element1, element2 });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, SetUnknownCount) {
  const SDCompact::Data unknownCompact = {
    { SDCompact::unknownCount, val1 },
    { SDCompact::unknownCount, val2 } };
  const SDCompact::Data compact = {
    { 2, val1 },
    { 2, val2 } };
  const auto type = "B(X1)"_t;
  const auto dataRS = Factory::Set({ element1, element2 });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(unknownCompact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, UnpackSetInvalidSize) {
  const SDCompact::Data compact1 = {
    { 3, val1 },
    { 3, val2 } };
  const SDCompact::Data compact2 = {
    { 1, val1 },
    { 2, val2 } };
  const auto type = "B(X1)"_t;
  EXPECT_FALSE(SDCompact::Unpack(compact1, type).has_value());
  EXPECT_FALSE(SDCompact::Unpack(compact2, type).has_value());
}

TEST_F(UTSDCompact, Decartian) {
  const SDCompact::Data compact =  {
    { 4, val1, val1 },
    { 4, val1, val2 },
    { 4, val2, val1 },
    { 4, val2, val2 } };
  const auto type = "B(X1*X1)"_t;
  auto dataRS = Factory::Set({ element1, element2 });
  dataRS = Factory::Decartian({ dataRS, dataRS });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, Boolean) {
  const SDCompact::Data compact = {
    { 4, 0, 0 },
    { 4, 1, val1 },
    { 4, 1, val2 },
    { 4, 2, val1 },
    { 4, 2, val2 } };
  const auto type = "BB(X1)"_t;
  const auto dataRS = Factory::Boolean(Factory::Set({ element1, element2 }));
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, SetInDecartRightside) {
  const SDCompact::Data compact = {
    { 4, val1, 2, val1 },
    { 4, val1, 2, val2 },
    { 4, val1, 2, val1 },
    { 4, val1, 2, val3 },
    { 4, val2, 2, val1 },
    { 4, val2, 2, val2 },
    { 4, val2, 2, val1 },
    { 4, val2, 2, val3 } };
  const auto type = "B(X1*B(X1))"_t;
  auto dataRS = Factory::Set({ element1, element2 });
  dataRS = Factory::Decartian({ dataRS, Factory::Set({ dataRS, Factory::Set({ element1, element3 }) }) });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, SetInDecartLeftside) {
  const SDCompact::Data compact =
    { { 4, 2, val1 },
      { 4, 2, val2, val1 },
      { 4, 2, val1 },
      { 4, 2, val2, val2 },
      { 4, 2, val1 },
      { 4, 2, val3, val1 },
      { 4, 2, val1 },
      { 4, 2, val3, val2 } };
  const auto type = "B(B(X1)*X1)"_t;
  auto dataRS = Factory::Set({ element1, element2 });
  dataRS = Factory::Decartian({ Factory::Set({ dataRS, Factory::Set({ element1, element3 }) }), dataRS });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}

TEST_F(UTSDCompact, DecartianBoolean) {
  const SDCompact::Data compact = {
    { 16, 0, 0, 0, 0 },
    { 16, 0, 0, 1, val1 },
    { 16, 0, 0, 1, val2 },
    { 16, 0, 0, 2, val1 },
    { 16, 0, 0, 2, val2 },
    { 16, 1, val1, 0, 0 },
    { 16, 1, val1, 1, val1 },
    { 16, 1, val1, 1, val2 },
    { 16, 1, val1, 2, val1 },
    { 16, 1, val1, 2, val2 },
    { 16, 1, val2, 0, 0 },
    { 16, 1, val2, 1, val1 },
    { 16, 1, val2, 1, val2 },
    { 16, 1, val2, 2, val1 },
    { 16, 1, val2, 2, val2 },
    { 16, 2, val1 },
    { 16, 2, val2, 0, 0 },
    { 16, 2, val1 },
    { 16, 2, val2, 1, val1 },
    { 16, 2, val1 },
    { 16, 2, val2, 1, val2 },
    { 16, 2, val1 },
    { 16, 2, val2, 2, val1 },
    { 16, 2, val2, 2, val2 } };
  const auto type = "B(B(X1)*B(X1))"_t;
  auto dataRS = Factory::Boolean(Factory::Set({ element1, element2 }));
  dataRS = Factory::Decartian({ dataRS , dataRS });
  EXPECT_EQ(SDCompact::FromSData(dataRS, type).data, compact);
  EXPECT_EQ(SDCompact::Unpack(compact, type)->ToString(), dataRS.ToString());
}