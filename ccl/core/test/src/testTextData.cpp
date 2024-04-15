#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/semantic/TextData.hpp"

#include <string>

class UTTextData : public ::testing::Test {
protected:
  using TextInterpretation = ccl::semantic::TextInterpretation;

  static constexpr auto ID_START = 1;
  static inline const std::string VALID_NAME{ "test" };
  static constexpr auto VALID_ID = 123567;
};

TEST_F(UTTextData, size) {
  EXPECT_EQ(std::ssize(TextInterpretation()), 0);
  EXPECT_EQ(std::ssize(TextInterpretation({})), 0);
  EXPECT_EQ(std::ssize(TextInterpretation({ "1" })), 1);
  EXPECT_EQ(std::ssize(TextInterpretation({ "1", "2" })), 2);
  EXPECT_EQ(std::ssize(TextInterpretation({ "1", "1" })), 2);
}

TEST_F(UTTextData, Clear) {
  TextInterpretation notEmpty{ { "1" } };
  TextInterpretation copy = notEmpty;
  notEmpty.Clear();
  EXPECT_NE(std::ssize(notEmpty), std::ssize(copy));
  EXPECT_EQ(std::ssize(notEmpty), 0);
}

TEST_F(UTTextData, IndexValidation) {
  const TextInterpretation empty{};
  EXPECT_FALSE(empty.HasInterpretantFor(0));
  EXPECT_FALSE(empty.HasInterpretantFor(int32_t{ -1 }));

  const TextInterpretation notEmpty({ "1" });
  EXPECT_FALSE(notEmpty.HasInterpretantFor(0));
  EXPECT_FALSE(notEmpty.HasInterpretantFor(int32_t{ -1 }));
  EXPECT_TRUE(notEmpty.HasInterpretantFor(ID_START));
  EXPECT_FALSE(notEmpty.HasInterpretantFor(static_cast<int32_t>(ID_START + std::ssize(notEmpty))));
}

TEST_F(UTTextData, DefaultIteration) {
  const std::vector<std::string> interpretations{"1", "2", "1", "", "1234a"};
  const TextInterpretation test{ interpretations };
  auto index{ 0U };
  for (const auto& it : test) {
    EXPECT_EQ(it.second, interpretations[index]);
    ++index;
  }
  EXPECT_EQ(index, size(interpretations));
}

TEST_F(UTTextData, CustomIteration) {
  const std::vector<std::string> interpretations{ "1", "3", "2" };
  const TextInterpretation test({ { 0, "1" }, { 3, "2" },{ 2, "3" } });
  auto index{ 0U };
  for (const auto& it : test) {
    EXPECT_EQ(it.second, interpretations[index]);
    ++index;
  }
  EXPECT_EQ(index, size(interpretations));
}

TEST_F(UTTextData, SetInterpretant) {
  TextInterpretation test{};
  test.SetInterpretantFor(ID_START, "X1");
  EXPECT_EQ(test.GetInterpretantFor(1), "X1");

  test.SetInterpretantFor(ID_START, "X2");
  ASSERT_EQ(std::ssize(test), 1);
  EXPECT_EQ(test.GetInterpretantFor(ID_START), "X2");
}

TEST_F(UTTextData, PushBack) {
  TextInterpretation test{};
  std::string name{ "X1" };
  
  const auto id1 = test.PushBack(name);
  ASSERT_EQ(std::ssize(test), 1);
  EXPECT_EQ(test.GetInterpretantFor(id1), name);

  name = "X2";
  const auto id2 = test.PushBack(name);
  ASSERT_EQ(std::ssize(test), 2);
  EXPECT_EQ(test.GetInterpretantFor(id2), name);
}

TEST_F(UTTextData, PushBackAdjustID) {
  TextInterpretation test{};
  test.SetInterpretantFor(ID_START + 1, "X1");
  const auto id = test.PushBack("X2");
  EXPECT_EQ(id, ID_START + 2);
}

TEST_F(UTTextData, Access) {
  const int32_t testID{ 123567 };
  const TextInterpretation test{ { testID, VALID_NAME } };
  EXPECT_TRUE(test.HasInterpretantFor(testID));
  EXPECT_EQ(test.GetInterpretantFor(testID), VALID_NAME);

  const auto invalid = int32_t{ testID + 1 };
  EXPECT_FALSE(test.HasInterpretantFor(invalid));
  const auto tryGetInterpretant = [&] { return test.GetInterpretantFor(invalid);  };
  EXPECT_THROW(tryGetInterpretant(), std::out_of_range);
}

TEST_F(UTTextData, Search) {
  const TextInterpretation test{ { VALID_ID, VALID_NAME } };
  const auto searchResult = test.GetInterpreteeOf(VALID_NAME);
  ASSERT_TRUE(searchResult.has_value());
  EXPECT_EQ(searchResult.value(), VALID_ID);
}

TEST_F(UTTextData, Erase) {
  TextInterpretation test{ { VALID_ID, VALID_NAME } };
  auto size = std::ssize(test);
  ASSERT_NO_THROW(test.EraseInterpretee(VALID_ID + 1));
  EXPECT_EQ(std::ssize(test), size);
  ASSERT_NO_THROW(test.EraseInterpretee(VALID_ID));
  EXPECT_EQ(std::ssize(test), size - 1);

  TextInterpretation test2{ { VALID_ID, VALID_NAME } };
  test2.Clear();
  EXPECT_TRUE(test2.empty());
}