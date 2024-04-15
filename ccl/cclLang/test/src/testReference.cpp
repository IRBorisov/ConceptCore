#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "FakeTermContext.hpp"
#include "MockTextProcessor.hpp"
#include "RefHelper.hpp"

#include "ccl/lang/TextEnvironment.h"
#include "ccl/lang/Reference.h"
#include "ccl/lang/Literals.h"

using ccl::lang::operator""_form;
using ccl::lang::operator""_ref;

class UTReference : public ::testing::Test {
protected:
  using ReferenceType = ccl::lang::ReferenceType;
  using Reference = ccl::lang::Reference;
  using EntityRef = ccl::lang::EntityRef;
  using CollaborationRef = ccl::lang::CollaborationRef;
  using StrRange = ccl::StrRange;
  using TextEnvironment = ccl::lang::TextEnvironment;
  using TextProcessor = ccl::lang::TextProcessor;

protected:
  static inline const std::string sampleCompanionRef { "@{-1|\u0442\u0435\u0441\u0442\u0438\u0440\u0443\u044e\u0449\u0438\u0439}" };
  static inline const std::string sampleRef { "\u0442\u0435\u0441\u0442\u0438\u0440\u0443\u044e\u0449\u0438\u0439" };
  static inline const std::string complexRefStr
    { "42 @{X1|nomn,sing} 43 @{-1|basic} 44 @{X1|nomn,sing} 45" };
  static constexpr auto sampleOffset = -1;
  static inline const Reference x1ref{ EntityRef{ "X1", "sing,nomn"_form } };
};

TEST_F(UTReference, Construction) {
  {
    const EntityRef term{ "X1", "sing,nomn"_form };
    const Reference ref{ term, StrRange(1, 4) };
    EXPECT_EQ(ref.position, StrRange(1, 4));
    EXPECT_EQ(ref.resolvedText, "");
    EXPECT_EQ(ref.GetType(), ReferenceType::entity);
    EXPECT_EQ(ref.GetEntity(), term.entity);
    EXPECT_EQ(ref.GetForm(), term.form);
  }
  {
    const CollaborationRef collab{ "test", 4 };
    const Reference ref{ collab, StrRange(2, 5) };
    EXPECT_EQ(ref.position, StrRange(2, 5));
    EXPECT_EQ(ref.resolvedText, "");
    EXPECT_EQ(ref.GetType(), ReferenceType::collaboration);
    EXPECT_EQ(ref.GetOffset(), collab.offset);
    EXPECT_EQ(ref.GetNominal(), collab.nominal);
  }
}

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 4834 ) // ignore not using return value
#endif

TEST_F(UTReference, ReferenceID) {
  {
    const Reference ref{ EntityRef{ "X1", "sing,nomn"_form } };
    ASSERT_EQ(ref.GetType(), ReferenceType::entity);
    EXPECT_TRUE(ref.IsEntity());
    EXPECT_FALSE(ref.IsCollaboration());
    EXPECT_EQ(ref.GetEntity(), "X1");
    EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
  }
  {
    const Reference ref{ CollaborationRef{ "text", 1337 }};
    ASSERT_EQ(ref.GetType(), ReferenceType::collaboration);
    EXPECT_FALSE(ref.IsEntity());
    EXPECT_TRUE(ref.IsCollaboration());
    EXPECT_EQ(ref.GetOffset(), 1337);
    EXPECT_EQ(ref.GetNominal(), "text");
  }
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

TEST_F(UTReference, ToString) {
  {
    const EntityRef term{ "X1", "sing,nomn"_form };
    EXPECT_EQ(term.ToString(), tccl::EntityRef("X1"));
    const Reference ref{ term };
    EXPECT_EQ(ref.ToString(), term.ToString());
  }
  {
    const CollaborationRef collab{ sampleRef, sampleOffset };
    EXPECT_EQ(collab.ToString(), sampleCompanionRef);
    const Reference ref{ collab };
    EXPECT_EQ(ref.ToString(), collab.ToString());
  }
}

TEST_F(UTReference, ParseErrors) {
  EXPECT_FALSE(Reference::Parse("").IsValid());
  EXPECT_FALSE(Reference::Parse("@{}").IsValid());
  EXPECT_FALSE(Reference::Parse("@{ }").IsValid());
  EXPECT_FALSE(Reference::Parse("@{|}").IsValid());
  EXPECT_FALSE(Reference::Parse("@{ | }").IsValid());
  EXPECT_FALSE(Reference::Parse("@{ || }").IsValid());
  EXPECT_FALSE(Reference::Parse("@{-1a|text}").IsValid());
  EXPECT_FALSE(Reference::Parse("invalid").IsValid());
  EXPECT_FALSE(Reference::Parse(tccl::EntityRef("X1") + " " + tccl::EntityRef("X2")).IsValid());
}

TEST_F(UTReference, ParseEntity) {
  auto ref = Reference::Parse(tccl::EntityRef("X1"));
  ASSERT_TRUE(ref.IsValid());
  ASSERT_TRUE(ref.IsEntity());
  EXPECT_EQ(ref.GetEntity(), "X1");
  EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
}

TEST_F(UTReference, ParseEntityLegacy) {
  const auto form = "sing,nomn"_form;
  {
    auto ref = Reference::Parse("@{X1|nomn|sing|0}");
    ASSERT_TRUE(ref.IsValid());
    ASSERT_TRUE(ref.IsEntity());
    EXPECT_EQ(ref.GetEntity(), "X1");
    EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
  }
  {
    auto ref = Reference::Parse("@{X1|nomn|sing}");
    ASSERT_TRUE(ref.IsValid());
    ASSERT_TRUE(ref.IsEntity());
    EXPECT_EQ(ref.GetEntity(), "X1");
    EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
  }
}

TEST_F(UTReference, ParseCollaboration) {
  auto ref = Reference::Parse(sampleCompanionRef);
  ASSERT_TRUE(ref.IsValid());
  ASSERT_TRUE(ref.IsCollaboration());
  EXPECT_EQ(ref.GetOffset(), sampleOffset);
  EXPECT_EQ(ref.GetNominal(), sampleRef);
}

TEST_F(UTReference, ParseReferences) {
  const auto parsed = Reference::ExtractAll(complexRefStr);
  ASSERT_EQ(3u, std::size(parsed));
  {
    EXPECT_EQ(parsed.at(0).position, StrRange(3, 18));
    ASSERT_TRUE(parsed.at(0).IsEntity());
    const auto& ref = parsed.at(0);
    EXPECT_EQ(ref.GetEntity(), "X1");
    EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
  }
  {
    EXPECT_EQ(parsed.at(1).position, StrRange(22, 33));
    ASSERT_TRUE(parsed.at(1).IsCollaboration());
    const auto& ref = parsed.at(1);
    EXPECT_EQ(ref.GetNominal(), "basic");
    EXPECT_EQ(ref.GetOffset(), -1);
  }
  {
    EXPECT_EQ(parsed.at(2).position, StrRange(37, 52));
    ASSERT_TRUE(parsed.at(2).IsEntity());
    const auto& ref = parsed.at(2);
    EXPECT_EQ(ref.GetEntity(), "X1");
    EXPECT_EQ(ref.GetForm(), "sing,nomn"_form);
  }
}

TEST_F(UTReference, ReferenceLiteral) {
  const auto ref = "@{X1|nomn,plur}"_ref;
  ASSERT_TRUE(ref.IsEntity());
  EXPECT_EQ(ref.GetEntity(), "X1");
  EXPECT_EQ(ref.GetForm(), "nomn,plur"_form);
}

TEST_F(UTReference, TranslateEntity) {
  auto ref = x1ref;
  EXPECT_FALSE(ref.TranslateEntity(ccl::CreateTranslator({ { "X42", "X43" } })));
  EXPECT_TRUE(ref.TranslateEntity(ccl::CreateTranslator({ { "X1", "X2" }, { "X2", "X3" } })));
  EXPECT_EQ(ref.GetEntity(), "X2");
}

TEST_F(UTReference, ResolveEntity) {
  TextEnvironment::SetProcessor(std::make_unique<tccl::MockTextProcessor>());
  auto& processor = static_cast<tccl::MockTextProcessor&>(TextEnvironment::Processor());
  FakeContext context{};

  auto emptyRef = Reference{ EntityRef{"", ""_form} };
  emptyRef.ResolveEntity(context);
  EXPECT_EQ(emptyRef.resolvedText, "!Empty reference!");
  EXPECT_EQ(size(processor.log), 0U);

  auto ref = x1ref;
  ref.ResolveEntity(context);
  EXPECT_EQ(ref.resolvedText, "!Cannot find entity: 'X1'!");
  EXPECT_EQ(size(processor.log), 0U);

  context.Insert("X1", ccl::lang::LexicalTerm{ "Test" });
  ref.ResolveEntity(context);
  EXPECT_EQ(ref.resolvedText, "Test");
  ASSERT_EQ(size(processor.log), 1U);
  EXPECT_EQ(processor.log.at(0).type, tccl::MockTextProcessor::CallType::INFLECT);
  EXPECT_EQ(processor.log.at(0).targetText, "Test");
  EXPECT_EQ(processor.log.at(0).form, x1ref.GetForm());
  processor.ClearAll();
  context.Clear();

  context.Insert("X1", ccl::lang::LexicalTerm{ "" });
  ref.resolvedText.clear();
  ref.ResolveEntity(context);
  EXPECT_EQ(ref.resolvedText, "!Empty reference!");
  ASSERT_EQ(size(processor.log), 1U);
  EXPECT_EQ(processor.log.at(0).type, tccl::MockTextProcessor::CallType::INFLECT);
  EXPECT_EQ(processor.log.at(0).targetText, "");
  EXPECT_EQ(processor.log.at(0).form, x1ref.GetForm());

  TextEnvironment::SetProcessor(std::make_unique<TextProcessor>());
}

TEST_F(UTReference, ResolveCollaborationEmpty) {
  TextEnvironment::SetProcessor(std::make_unique<tccl::MockTextProcessor>());
  auto& processor = static_cast<tccl::MockTextProcessor&>(TextEnvironment::Processor());

  auto emptyRef = Reference{ CollaborationRef{ "", 1 } };
  emptyRef.ResolveCollaboration(nullptr);
  EXPECT_EQ(emptyRef.resolvedText, "!Empty reference!");
  EXPECT_EQ(size(processor.log), 0U);

  auto master = Reference{ x1ref };
  master.resolvedText = "";
  emptyRef.resolvedText = "";
  emptyRef.ResolveCollaboration(&master);
  EXPECT_EQ(emptyRef.resolvedText, "!Empty reference!");
  EXPECT_EQ(size(processor.log), 0U);

  master.resolvedText = "test";
  emptyRef.resolvedText = "";
  emptyRef.ResolveCollaboration(&master);
  EXPECT_EQ(emptyRef.resolvedText, "!Empty reference!");
  EXPECT_EQ(size(processor.log), 0U);

  TextEnvironment::SetProcessor(std::make_unique<TextProcessor>());
}

TEST_F(UTReference, ResolveCollaborationValid) {
  TextEnvironment::SetProcessor(std::make_unique<tccl::MockTextProcessor>());
  auto& processor = static_cast<tccl::MockTextProcessor&>(TextEnvironment::Processor());

  auto collabRef = Reference{ CollaborationRef{ "companion", 1 } };
  collabRef.ResolveCollaboration(nullptr);
  EXPECT_EQ(collabRef.resolvedText, "!Invalid offset for companion: '1'!");
  EXPECT_EQ(size(processor.log), 0U);

  auto master = Reference{ x1ref };
  master.resolvedText = "";
  collabRef.resolvedText = "";
  collabRef.ResolveCollaboration(&master);
  EXPECT_EQ(collabRef.resolvedText, "companion");
  ASSERT_EQ(size(processor.log), 1U);
  EXPECT_EQ(processor.log.at(0).type, tccl::MockTextProcessor::CallType::DEPENDANT);
  EXPECT_EQ(processor.log.at(0).targetText, "companion");
  EXPECT_EQ(processor.log.at(0).mainText, "");
  processor.ClearAll();

  master.resolvedText = "test";
  collabRef.resolvedText = "";
  processor.AddCollaboration(collabRef.GetNominal(), master.resolvedText, "companion_inflected");
  collabRef.ResolveCollaboration(&master);
  EXPECT_EQ(collabRef.resolvedText, "companion_inflected");
  ASSERT_EQ(size(processor.log), 1U);
  EXPECT_EQ(processor.log.at(0).type, tccl::MockTextProcessor::CallType::DEPENDANT);
  EXPECT_EQ(processor.log.at(0).targetText, "companion");
  EXPECT_EQ(processor.log.at(0).mainText, "test");

  TextEnvironment::SetProcessor(std::make_unique<TextProcessor>());
}