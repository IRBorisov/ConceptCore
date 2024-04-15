#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/cclTypes.hpp"

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 4834 6031 )
#endif

class UTStaticMap : public ::testing::Test {
protected:
  using MapType = ccl::StaticMap<std::string_view, int32_t, 4>;
  using ValueType = MapType::DataType::value_type;
  static constexpr MapType testMap{
    MapType::DataType({ ValueType{"A", 1}, ValueType{"B", 42}, ValueType{"C", 55}, ValueType{"Hello", 1337} })
  };
};

TEST_F(UTStaticMap, ContainsKey) {
  EXPECT_TRUE(testMap.ContainsKey("Hello"));
  EXPECT_TRUE(testMap.ContainsKey("A"));
  EXPECT_FALSE(testMap.ContainsKey(""));
  EXPECT_FALSE(testMap.ContainsKey(" "));
  EXPECT_FALSE(testMap.ContainsKey("invalid"));
}

TEST_F(UTStaticMap, ContainsValue) {
  EXPECT_TRUE(testMap.ContainsValue(1));
  EXPECT_TRUE(testMap.ContainsValue(1337));
  EXPECT_FALSE(testMap.ContainsValue(-1));
  EXPECT_FALSE(testMap.ContainsValue(0));
  EXPECT_FALSE(testMap.ContainsValue(1338));
}

TEST_F(UTStaticMap, AtKey) {
  EXPECT_EQ(testMap.AtKey("A"), 1);
  EXPECT_EQ(testMap.AtKey("Hello"), 1337);
}

TEST_F(UTStaticMap, AtValue) {
  EXPECT_EQ(testMap.AtValue(1), "A");
  EXPECT_EQ(testMap.AtValue(1337), "Hello");
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif