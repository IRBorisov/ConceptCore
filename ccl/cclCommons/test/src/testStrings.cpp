#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/Strings.hpp"

using ccl::operator""_c17;

class UTStrings : public ::testing::Test {
protected:
  using StrRange = ccl::StrRange;
  using UTF8Iterator = ccl::UTF8Iterator;
};

TEST_F(UTStrings, Length) {
  EXPECT_TRUE(StrRange{}.length() == 0);
  EXPECT_TRUE(StrRange(1, 1).length() == 0);
  EXPECT_TRUE(StrRange(2, 4).length() == 2);
  EXPECT_TRUE(StrRange(4, 2).length() == -2);
}

TEST_F(UTStrings, Shift) {
  EXPECT_EQ(StrRange(4, 6), StrRange(2, 4).Shift(2));
  EXPECT_EQ(StrRange(0, 2), StrRange(2, 4).Shift(-2));
}

TEST_F(UTStrings, Equals) {
  EXPECT_EQ(StrRange(1, 1), StrRange(1, 1));
  EXPECT_NE(StrRange(1, 2), StrRange(1, 1));
  EXPECT_NE(StrRange(2, 1), StrRange(1, 1));
  EXPECT_EQ(StrRange(2, 1), StrRange(2, 1));
  EXPECT_EQ(StrRange(1, 2), StrRange(1, 2));
  EXPECT_NE(StrRange(2, 1), StrRange(1, 2));
}

TEST_F(UTStrings, Collapse) {
  EXPECT_EQ(StrRange(4, 4), StrRange(2, 4).CollapseEnd());
  EXPECT_EQ(StrRange(2, 2), StrRange(2, 4).CollapseStart());
}

TEST_F(UTStrings, Contains) {
  EXPECT_TRUE(StrRange(1, 3).Contains(1));
  EXPECT_TRUE(StrRange(1, 3).Contains(2));
  EXPECT_FALSE(StrRange(1, 3).Contains(3));
  EXPECT_FALSE(StrRange(1, 3).Contains(4));
  EXPECT_FALSE(StrRange(1, 3).Contains(0));

  EXPECT_TRUE(StrRange(2, 5).Contains(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(1, 6)));
  EXPECT_TRUE(StrRange(2, 5).Contains(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(2, 6)));
  EXPECT_TRUE(StrRange(2, 5).Contains(StrRange(3, 4)));
  EXPECT_TRUE(StrRange(2, 5).Contains(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).Contains(StrRange(6, 7)));
}

TEST_F(UTStrings, SharesBorder) {
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(0, 1)));
  EXPECT_TRUE(StrRange(2, 5).SharesBorder(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(3, 6)));
  EXPECT_TRUE(StrRange(2, 5).SharesBorder(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).SharesBorder(StrRange(6, 7)));
}

TEST_F(UTStrings, Overlaps) {
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).Overlaps(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).Overlaps(StrRange(1, 2)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(1, 3)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(1, 5)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(1, 6)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(2, 3)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(2, 6)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(3, 4)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(3, 5)));
  EXPECT_TRUE(StrRange(2, 5).Overlaps(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).Overlaps(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).Overlaps(StrRange(6, 7)));
}

TEST_F(UTStrings, IsBefore) {
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsBefore(StrRange(5, 6)));
  EXPECT_TRUE(StrRange(2, 5).IsBefore(StrRange(6, 7)));
}

TEST_F(UTStrings, IsAfter) {
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(2, 5)));

  EXPECT_TRUE(StrRange(2, 5).IsAfter(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsAfter(StrRange(6, 7)));
}

TEST_F(UTStrings, Meets) {
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(3, 6)));
  EXPECT_TRUE(StrRange(2, 5).Meets(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).Meets(StrRange(6, 7)));
}

TEST_F(UTStrings, Starts) {
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(2, 3)));
  EXPECT_TRUE(StrRange(2, 5).Starts(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).Starts(StrRange(6, 7)));
}

TEST_F(UTStrings, Finishes) {
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(1, 3)));
  EXPECT_TRUE(StrRange(2, 5).Finishes(StrRange(1, 5)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).Finishes(StrRange(6, 7)));
}

TEST_F(UTStrings, IsDuring) {
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(2, 5)));

  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(0, 1)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(1, 2)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(1, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(1, 5)));
  EXPECT_TRUE(StrRange(2, 5).IsDuring(StrRange(1, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(2, 3)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(2, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(3, 4)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(3, 5)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(3, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(5, 6)));
  EXPECT_FALSE(StrRange(2, 5).IsDuring(StrRange(6, 7)));
}

TEST_F(UTStrings, Merge) {
  EXPECT_EQ(StrRange::Merge({}), StrRange{});
  EXPECT_EQ(StrRange::Merge({ StrRange(1, 2), StrRange(2, 4) }), StrRange(1, 4));
  EXPECT_EQ(StrRange::Merge({ StrRange(2, 4), StrRange(1, 2) }), StrRange(1, 4));
  EXPECT_EQ(StrRange::Merge({ StrRange(1, 2), StrRange(3, 4) }), StrRange(1, 4));
  EXPECT_EQ(StrRange::Merge({ StrRange(1, 3), StrRange(2, 4) }), StrRange(1, 4));
}

TEST_F(UTStrings, Substr) {
  EXPECT_EQ(ccl::Substr("", StrRange{ 0, 0 }), "");
  EXPECT_EQ(ccl::Substr("012345", StrRange(0, 3)), "012");
  EXPECT_EQ(ccl::Substr("012345", StrRange(0, 10)), "");
  EXPECT_EQ(ccl::Substr("012345", StrRange(2, 3)), "2");
  EXPECT_EQ(ccl::Substr(u8"ab\u212Ccd"_c17, StrRange(2, 4)), u8"\u212Cc"_c17);
}

TEST_F(UTStrings, CharSize) {
  EXPECT_EQ(ccl::UTF8CharSize(static_cast<unsigned char>(std::string("a").at(0U))), 1);
  EXPECT_EQ(ccl::UTF8CharSize(static_cast<unsigned char>(std::string("1").at(0U))), 1);
  EXPECT_EQ(ccl::UTF8CharSize(static_cast<unsigned char>(u8"\u0435"_c17.at(0U))), 2);
  EXPECT_EQ(ccl::UTF8CharSize(static_cast<unsigned char>(std::string("\xE2\x84\xAC").at(0U))), 3);
  EXPECT_EQ(ccl::UTF8CharSize(static_cast<unsigned char>(std::string("\xF0\xA0\x9C\x8E").at(0U))), 4);
}

TEST_F(UTStrings, UTF8Iteration) {
  EXPECT_EQ(ccl::UTF8End({}), UTF8Iterator({}));

  const auto text = u8"\u212Cabc\u212Ccba\u212C"_c17;
  auto it = UTF8Iterator(text);
  EXPECT_EQ(*it, '\xE2');
  EXPECT_EQ(it.BytePosition(), 0U);
  EXPECT_EQ(it.Position(), 0);
  EXPECT_EQ(it.SymbolSize(), 3U);

  EXPECT_EQ(*++it, 'a');
  EXPECT_EQ(it.BytePosition(), 3U);
  EXPECT_EQ(it.Position(), 1);
  EXPECT_EQ(it.SymbolSize(), 1U);

  EXPECT_EQ(*++it, 'b');
  EXPECT_EQ(it.BytePosition(), 4U);
  EXPECT_EQ(it.Position(), 2);

  EXPECT_EQ(*++it, 'c');
  EXPECT_EQ(it.BytePosition(), 5U);
  EXPECT_EQ(it.Position(), 3);

  EXPECT_EQ(*++it, '\xE2');
  EXPECT_EQ(it.BytePosition(), 6U);
  EXPECT_EQ(it.Position(), 4);

  EXPECT_EQ(*++it, 'c');
  EXPECT_EQ(it.BytePosition(), 9U);
  EXPECT_EQ(it.Position(), 5);

  EXPECT_EQ(*++it, 'b');
  EXPECT_EQ(it.BytePosition(), 10U);
  EXPECT_EQ(it.Position(), 6);

  EXPECT_EQ(*++it, 'a');
  EXPECT_EQ(it.BytePosition(), 11U);
  EXPECT_EQ(it.Position(), 7);

  EXPECT_EQ(*++it, '\xE2');
  EXPECT_EQ(it.BytePosition(), 12U);
  EXPECT_EQ(it.Position(), 8);

  EXPECT_EQ(++it, ccl::UTF8End(text));
}

TEST_F(UTStrings, IteratorCreation) {
  EXPECT_EQ(UTF8Iterator("123", 42), ccl::UTF8End("123"));
  EXPECT_EQ(UTF8Iterator("123", -1), ccl::UTF8End("123"));
  EXPECT_EQ(UTF8Iterator("\xE2\x84\xAC", 1), ccl::UTF8End("\xE2\x84\xAC"));
}

TEST_F(UTStrings, SizeInCodePoints) {
  EXPECT_EQ(ccl::SizeInCodePoints({}), 0);
  EXPECT_EQ(ccl::SizeInCodePoints(""), 0);
  EXPECT_EQ(ccl::SizeInCodePoints("42"), 2);
  EXPECT_EQ(ccl::SizeInCodePoints(u8"\u212C42"_c17), 3);
}

TEST_F(UTStrings, SplitBySymbol) {
  using ccl::SplitBySymbol;
  {
    const std::string str = "";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 1);
    EXPECT_TRUE(tokens[0] == "");
  }
  {
    const std::string str = "A,B,C";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 3);
    EXPECT_TRUE(tokens[0] == "A");
    EXPECT_TRUE(tokens[1] == "B");
    EXPECT_TRUE(tokens[2] == "C");
  }
  {
    const std::string str = ",B,C";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 3);
    EXPECT_TRUE(tokens[0] == "");
    EXPECT_TRUE(tokens[1] == "B");
    EXPECT_TRUE(tokens[2] == "C");
  }
  {
    const std::string str = "A,B,";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 3);
    EXPECT_TRUE(tokens[0] == "A");
    EXPECT_TRUE(tokens[1] == "B");
    EXPECT_TRUE(tokens[2] == "");
  }
  {
    const std::string str = "A";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 1);
    EXPECT_TRUE(tokens[0] == "A");
  }
  {
    const std::string str = ",";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 2);
    EXPECT_TRUE(tokens[0] == "");
    EXPECT_TRUE(tokens[1] == "");
  }
  {
    const std::string str = ",,";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 3);
    EXPECT_TRUE(tokens[0] == "");
    EXPECT_TRUE(tokens[1] == "");
    EXPECT_TRUE(tokens[2] == "");
  }
  {
    const std::string str = "A,";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 2);
    EXPECT_TRUE(tokens[0] == "A");
    EXPECT_TRUE(tokens[1] == "");
  }
  {
    const std::string str = ",B";
    auto tokens = SplitBySymbol(str, ',');
    EXPECT_TRUE(tokens.size() == 2);
    EXPECT_TRUE(tokens[0] == "");
    EXPECT_TRUE(tokens[1] == "B");
  }
}

TEST_F(UTStrings, TrimWhitespace) {
  using ccl::TrimWhitespace;
  EXPECT_EQ(TrimWhitespace(""), "");
  EXPECT_EQ(TrimWhitespace(" "), "");
  EXPECT_EQ(TrimWhitespace("  "), "");
  EXPECT_EQ(TrimWhitespace(" 1 "), "1");
  EXPECT_EQ(TrimWhitespace("  1  "), "1");
  EXPECT_EQ(TrimWhitespace(" 1"), "1");
  EXPECT_EQ(TrimWhitespace("1 "), "1");
  EXPECT_EQ(TrimWhitespace(" 1 2 3 "), "1 2 3");
  EXPECT_EQ(TrimWhitespace(" 1 2 3  "), "1 2 3");
  EXPECT_EQ(TrimWhitespace("1 2 3 "), "1 2 3");
  EXPECT_EQ(TrimWhitespace("1 2 3"), "1 2 3");
  EXPECT_EQ(TrimWhitespace(" , "), ",");
}

TEST_F(UTStrings, IsInteger) {
  using ccl::IsInteger;
  EXPECT_FALSE(IsInteger(""));
  EXPECT_FALSE(IsInteger(" "));
  EXPECT_FALSE(IsInteger("-"));
  EXPECT_FALSE(IsInteger("1-2"));
  EXPECT_FALSE(IsInteger("-1-2"));
  EXPECT_FALSE(IsInteger("-1-"));
  EXPECT_TRUE(IsInteger("0"));
  EXPECT_TRUE(IsInteger("00"));
  EXPECT_TRUE(IsInteger("1"));
  EXPECT_TRUE(IsInteger("-1"));
  EXPECT_TRUE(IsInteger("1234"));
  EXPECT_TRUE(IsInteger("-1234"));
  EXPECT_TRUE(IsInteger("-9876543210"));
  EXPECT_TRUE(IsInteger("007"));
}