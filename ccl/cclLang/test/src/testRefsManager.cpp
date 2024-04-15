#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "MockTextProcessor.hpp"
#include "FakeTermContext.hpp"
#include "RefHelper.hpp"

#include "ccl/lang/TextEnvironment.h"
#include "ccl/lang/RefsManager.h"
#include "ccl/lang/Literals.h"

using ccl::lang::operator""_form;
using ccl::lang::operator""_ref;

struct FakeUpdater {
  using StrRange = ccl::StrRange;
  std::vector<StrRange> ranges;

  FakeUpdater(std::vector<StrRange> ranges) 
    : ranges{ std::move(ranges) } {}

  std::optional<StrRange> operator()(const StrRange& prev) {
    if (prev == StrRange{ 0, 0 }) {
      if (empty(ranges)) {
        return std::nullopt;
      } else {
        return *begin(ranges);
      }
    }
    if (auto it = std::find(begin(ranges), end(ranges), prev); 
        it == end(ranges)) {
      return std::nullopt;
    } else if (++it; it == end(ranges)) {
      return std::nullopt;
    }  else {
      return *it;
    }
  }
};

class UTRefsManager : public ::testing::Test {
protected:
  using RefsManager = ccl::lang::RefsManager;
  using Reference = ccl::lang::Reference;
  using Morphology = ccl::lang::Morphology;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using StrRange = ccl::StrRange;
  using TextEnvironment = ccl::lang::TextEnvironment;
  using TextProcessor = ccl::lang::TextProcessor;

  FakeContext context{};
  RefsManager refs{};

protected:
  UTRefsManager();

  static inline const std::string basicRefStr{"@{X1|sing,nomn}"};
  static inline const std::string complexRefStr
    {"42 @{X1|sing,nomn} 43 @{-1|basic} 44 @{X1|sing,nomn} 45"};

  // refStr:  42 @{X1|sing,nomn} 43 @{-1|basic} 44 @{X1|sing,nomn} 45
  // normStr: 42 Test 43 basic 44 Test 45
  // ---------0000000000111111111122222222
  // ---------0123456789012345678901234567
};

UTRefsManager::UTRefsManager() {
  context.Insert("X1", LexicalTerm{ "Test" });
  context.Insert("X2", LexicalTerm{ "Test2" });
  refs.SetContext(context);
}

TEST_F(UTRefsManager, Constructor) {
  const auto emptyMan = RefsManager{};
  EXPECT_TRUE(empty(emptyMan.get()));
  const auto manager = RefsManager{ context };
  EXPECT_TRUE(empty(emptyMan.get()));
}

TEST_F(UTRefsManager, Clear) {
  refs.clear();
  EXPECT_EQ(size(refs.get()), 0U);

  refs.SetContext(context);
  EXPECT_EQ(size(refs.get()), 0U);

  refs.Resolve(basicRefStr);
  EXPECT_EQ(size(refs.get()), 1U);
  refs.clear();
  EXPECT_EQ(size(refs.get()), 0U);
}

TEST_F(UTRefsManager, ResolveValid) {
  EXPECT_EQ(refs.Resolve(""), "");
  EXPECT_EQ(std::size(refs.get()), 0U);

  EXPECT_EQ(refs.Resolve("sample text"), "sample text");
  EXPECT_EQ(std::size(refs.get()), 0U);

  EXPECT_EQ(refs.Resolve(basicRefStr), "Test");
  ASSERT_EQ(size(refs.get()), 1U);
  EXPECT_EQ(begin(refs.get())->position, StrRange(0, 4));

  EXPECT_EQ(refs.Resolve(complexRefStr), "42 Test 43 basic 44 Test 45");
  ASSERT_EQ(size(refs.get()), 3U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(3, 7));
  EXPECT_EQ(refs.get().at(1).position, StrRange(11, 16));
  EXPECT_EQ(refs.get().at(2).position, StrRange(20, 24));
}

TEST_F(UTRefsManager, ResolveCompanionOffsets) {
  TextEnvironment::SetProcessor(std::make_unique<tccl::MockTextProcessor>());
  auto& processor = static_cast<tccl::MockTextProcessor&>(TextEnvironment::Processor());

  EXPECT_EQ(refs.Resolve("@{-1|basic}"), "!Invalid offset for basic: '-1'!");
  EXPECT_EQ(refs.Resolve("@{-1|basic} @{X1|sing,nomn}"), "!Invalid offset for basic: '-1'! Test");
  EXPECT_EQ(refs.Resolve("@{X1|sing,nomn} @{-1|basic}"), "Test basic");
  EXPECT_EQ(refs.Resolve("@{1|basic} @{X1|sing,nomn}"), "basic Test");
  EXPECT_EQ(refs.Resolve("@{1|basic1} @{1|basic1} @{X1|sing,nomn}"), "basic1 basic1 Test");
  EXPECT_EQ(refs.Resolve("@{2|basic1} @{1|basic1} @{X1|sing,nomn}"), "!Invalid offset for basic1: '2'! basic1 Test");
  processor.AddCollaboration("basic", "Test", "basic_inflected");
  EXPECT_EQ(refs.Resolve("@{1|basic} @{X1|sing,nomn}"), "basic_inflected Test");

  TextEnvironment::SetProcessor(std::make_unique<TextProcessor>());
}

TEST_F(UTRefsManager, Insert) {
  const auto newRef = "@{X2|sing,nomn}"_ref;
  refs.Resolve("");
  EXPECT_EQ(refs.Insert(newRef, 0)->resolvedText, "Test2");
  ASSERT_EQ(size(refs.get()), 1U);
  EXPECT_EQ(begin(refs.get())->position, StrRange(0, 5));

  refs.Resolve(complexRefStr);
  EXPECT_TRUE(refs.Insert(newRef, 3) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 4) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 6) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 7) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 20) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 21) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 23) == nullptr);
  EXPECT_TRUE(refs.Insert(newRef, 24) == nullptr);

  refs.Resolve(complexRefStr);
  EXPECT_FALSE(refs.Insert(newRef, 2) == nullptr);

  refs.Resolve(complexRefStr);
  EXPECT_FALSE(refs.Insert(newRef, 19) == nullptr);
  
  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.Insert(newRef, 8)->resolvedText, "Test2");
  ASSERT_EQ(size(refs.get()), 4U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(3, 7));
  EXPECT_EQ(refs.get().at(1).position, StrRange(8, 13));
  EXPECT_EQ(refs.get().at(2).position, StrRange(16, 21));
  EXPECT_EQ(refs.get().at(3).position, StrRange(25, 29));
}

TEST_F(UTRefsManager, EraseRefsRange) {
  EXPECT_TRUE(refs.EraseIn(StrRange{ 0, 0 }).has_value());
  EXPECT_TRUE(refs.EraseIn(StrRange{ 0, 42 }).has_value());

  refs.Resolve(complexRefStr);
  EXPECT_FALSE(refs.EraseIn(StrRange{ 0, 4 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 0, 5 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 0, 6 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 3, 6 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 6, 7 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 4, 5 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 5, 5 }).has_value());
  EXPECT_FALSE(refs.EraseIn(StrRange{ 7, 11 }).has_value());

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 42, 42 }).value(), StrRange(42, 42));
  EXPECT_EQ(refs.EraseIn(StrRange{ 0, 27 }).value(), StrRange(0, 27));
  EXPECT_EQ(std::size(refs.get()), 0U);

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 0, 3 }).value(), StrRange(0, 3));
  ASSERT_EQ(std::size(refs.get()), 3U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(0, 4));
  EXPECT_EQ(refs.get().at(1).position, StrRange(8, 13));
  EXPECT_EQ(refs.get().at(2).position, StrRange(17, 21));

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 0, 7 }).value(), StrRange(0, 7));
  ASSERT_EQ(std::size(refs.get()), 2U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(4, 9));
  EXPECT_EQ(refs.get().at(1).position, StrRange(13, 17));

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 20, 24 }).value(), StrRange(20, 24));
  ASSERT_EQ(size(refs.get()), 2U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(3, 7));
  EXPECT_EQ(refs.get().at(1).position, StrRange(11, 16));
}

TEST_F(UTRefsManager, EraseRefsExpand) {
  EXPECT_TRUE(refs.EraseIn(StrRange{ 0, 1 }, true).has_value());

  refs.Resolve(complexRefStr);
  refs.EraseIn((StrRange{ 7, 10 }));
  EXPECT_FALSE(refs.EraseIn(StrRange{ 7, 8 }, true).has_value());

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 2, 3 }, true).value(), StrRange(2, 3));
  ASSERT_EQ(size(refs.get()), 3U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(2, 6));
  EXPECT_EQ(refs.get().at(1).position, StrRange(10, 15));
  EXPECT_EQ(refs.get().at(2).position, StrRange(19, 23));

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 3, 4 }, true).value(), StrRange(3, 7));
  ASSERT_EQ(size(refs.get()), 2U);
  EXPECT_EQ(refs.get().at(0).position, StrRange(7, 12));
  EXPECT_EQ(refs.get().at(1).position, StrRange(16, 20));

  refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.EraseIn(StrRange{ 6, 7 }, true).value(), StrRange(3, 7));
  ASSERT_EQ(size(refs.get()), 2U);
}

TEST_F(UTRefsManager, FirstRef) {
  EXPECT_TRUE(refs.FirstIn(StrRange{ 0, 0 }) == nullptr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 0, 42 }) == nullptr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 42, 42 }) == nullptr);

  refs.Resolve(complexRefStr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 0, 0 }) == nullptr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 0, 1 }) == nullptr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 27, 27 }) == nullptr);
  EXPECT_TRUE(refs.FirstIn(StrRange{ 28, 42 }) == nullptr);

  EXPECT_EQ(refs.FirstIn(StrRange{ 0, 3 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 3, 3 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 3, 6 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 3, 7 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 0, 7 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 6, 12 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 0, 12 }), &refs.get()[0]);
  EXPECT_EQ(refs.FirstIn(StrRange{ 12, 12 }), &refs.get()[1]);
}

TEST_F(UTRefsManager, UpdatePositions) {
  EXPECT_TRUE(refs.UpdatePositions(FakeUpdater{ {} }));
  EXPECT_FALSE(refs.UpdatePositions(FakeUpdater{ { StrRange{ 0, 1 } } }));

  refs.Resolve(complexRefStr);
  EXPECT_FALSE(refs.UpdatePositions(FakeUpdater{ {} }));
  EXPECT_FALSE(refs.UpdatePositions(FakeUpdater{ { StrRange{ 0, 1 }, StrRange{ 2, 3 } } }));
  EXPECT_FALSE(refs.UpdatePositions(FakeUpdater{ 
    { StrRange{ 0, 1 }, StrRange{ 2, 3 }, StrRange{ 4, 5 }, StrRange{ 6, 7 } } }));
  
  refs.Resolve(complexRefStr);
  EXPECT_TRUE(refs.UpdatePositions(FakeUpdater{  { StrRange{ 0, 1 }, StrRange{ 2, 3 }, StrRange{ 4, 5 } } }));
  EXPECT_EQ(refs.get().at(0).position, StrRange(0, 1));
  EXPECT_EQ(refs.get().at(1).position, StrRange(2, 3));
  EXPECT_EQ(refs.get().at(2).position, StrRange(4, 5));

  refs.Resolve(complexRefStr);
  EXPECT_TRUE(refs.UpdatePositions(FakeUpdater{  { StrRange{ 0, 4 }, StrRange{ 5, 10 }, StrRange{ 11, 15 } } }));
  EXPECT_EQ(refs.get().at(0).position, StrRange(0, 4));
  EXPECT_EQ(refs.get().at(1).position, StrRange(5, 10));
  EXPECT_EQ(refs.get().at(2).position, StrRange(11, 15));
}

TEST_F(UTRefsManager, OutputRefs) {
  using ccl::operator""_c17;

  EXPECT_TRUE(empty(refs.OutputRefs({}, {})));
  EXPECT_TRUE(empty(refs.OutputRefs({}, StrRange{ 0, 10 })));

  refs.Resolve({});
  EXPECT_TRUE(empty(refs.OutputRefs({}, {})));

  refs.Resolve(u8"\u212Cabc"_c17);
  EXPECT_EQ(refs.OutputRefs(u8"\u212Cabc"_c17), u8"\u212Cabc"_c17);

  const auto normStr = refs.Resolve(complexRefStr);
  EXPECT_EQ(refs.OutputRefs(normStr), complexRefStr);
}

TEST_F(UTRefsManager, OutputRefsRange) {
  const auto normStr = refs.Resolve(complexRefStr);

  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 27 }), complexRefStr);
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 2 }), "42");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 0 }), "");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 26, 27 }), "5");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 27, 28 }), "");

  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 7 }), "42 @{X1|sing,nomn}");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 6 }), "42 @{X1|sing,nomn}");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 5 }), "42 @{X1|sing,nomn}");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 4 }), "42 ");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 0, 3 }), "42 ");

  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 3, 10 }), "@{X1|sing,nomn} 43");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 4, 10 }), "@{X1|sing,nomn} 43");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 5, 10 }), "@{X1|sing,nomn} 43");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 6, 10 }), " 43");
  EXPECT_EQ(refs.OutputRefs(normStr, StrRange{ 7, 10 }), " 43");
}