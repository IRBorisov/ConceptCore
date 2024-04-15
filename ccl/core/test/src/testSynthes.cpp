#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "RSLHelper.hpp"

#include "ccl/semantic/RSForm.h"
#include "ccl/ops/RSOperations.h"
#include "RSLHelper.hpp"

#include <algorithm>

class UTSynthes : public ::testing::Test {
protected:
  using RSForm = ccl::semantic::RSForm;
  using CstType = ccl::semantic::CstType;
  using BinarySynthes = ccl::ops::BinarySynthes;
  using EquationOptions = ccl::ops::EquationOptions;

protected:
  UTSynthes();

protected:
  RSForm schema1{};
  RSForm schema2{};

  ccl::EntityUID x1_1, x2_1, s1_1;
  ccl::EntityUID x1_2, s1_2;
};

UTSynthes::UTSynthes() {
  x1_1 = schema1.Emplace(CstType::base);
  schema1.SetTermFor(x1_1, "X1_1");
  x2_1 = schema1.Emplace(CstType::base);
  schema1.SetTermFor(x2_1, "X2_1");
  s1_1 = schema1.Emplace(CstType::structured, "B(X1)"_rs);
  schema1.SetTermFor(s1_1, "X1_1");

  x1_2 = schema2.Emplace(CstType::base);
  schema2.SetTermFor(x1_2, "X1_2");
  s1_2 = schema2.Emplace(CstType::structured, "B(X1)"_rs);
  schema2.SetTermFor(s1_2, "S1_2");
}

TEST_F(UTSynthes, Merge) {
  auto operation = BinarySynthes(schema1, schema2, EquationOptions{});
  ASSERT_TRUE(operation.IsCorrectlyDefined());

  const auto result = operation.Execute();
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(std::size(result->Core()), 
            std::size(schema1.Core()) + std::size(schema2.Core()));

  const auto& translations = operation.Translations();
  ASSERT_EQ(ssize(translations), 2);
  EXPECT_EQ(std::size(schema1.Core()), std::size(translations[0]));
  EXPECT_EQ(std::size(schema2.Core()), std::size(translations[1]));
}

TEST_F(UTSynthes, MergeSelf) {
  auto operation = BinarySynthes(schema1, schema1, EquationOptions{});
  ASSERT_TRUE(operation.IsCorrectlyDefined());

  const auto result = operation.Execute();
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(std::size(result->Core()), std::size(schema1.Core()));

  const auto& translations = operation.Translations();
  EXPECT_EQ(translations[0], translations[1]);
  EXPECT_EQ(std::size(schema1.Core()), std::size(translations[0]));
}

TEST_F(UTSynthes, EquationInvalid) {
  auto operation = BinarySynthes(schema1, schema2, EquationOptions{ x1_2, x1_1 });
  EXPECT_FALSE(operation.IsCorrectlyDefined());
}

TEST_F(UTSynthes, EquationBasicTranslations) {
  auto operation = BinarySynthes(schema1, schema2, EquationOptions{ x2_1, x1_2 });
  ASSERT_TRUE(operation.IsCorrectlyDefined());

  const auto result = operation.Execute();
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(std::size(result->Core()), 
            std::size(schema1.Core()) + std::size(schema2.Core()) - std::size(operation.Equations()));

  const auto& translations = operation.Translations();
  EXPECT_EQ(std::size(schema1.Core()), std::size(translations[0]));
  EXPECT_EQ(std::size(schema2.Core()), std::size(translations[1]));
  EXPECT_EQ(translations[0](x2_1), translations[1](x1_2));
  EXPECT_TRUE(result->Contains(translations[0](x2_1)));
}

TEST_F(UTSynthes, EquationFlippingTransitions) {
  auto operation = BinarySynthes(schema1, schema2, EquationOptions{ s1_1, x1_2 });
  ASSERT_TRUE(operation.IsCorrectlyDefined());

  const auto result = operation.Execute();
  ASSERT_TRUE(result != nullptr);
  EXPECT_EQ(std::size(result->Core()), 
            std::size(schema1.Core()) + std::size(schema2.Core()) - std::size(operation.Equations()));

  const auto& translations = operation.Translations();
  EXPECT_EQ(std::size(schema1.Core()), std::size(translations[0]));
  EXPECT_EQ(std::size(schema2.Core()), std::size(translations[1]));
  EXPECT_EQ(translations[0](s1_1), translations[1](x1_2));
  EXPECT_TRUE(result->Contains(translations[0](s1_1)));
}