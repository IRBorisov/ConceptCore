#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include "ccl/tools/JSON.h"
#include "ccl/lang/Literals.h"
#include "ccl/rslang/Literals.h"

using JSON = nlohmann::ordered_json;

using ccl::lang::operator""_form;
using ccl::rslang::operator""_rs;
using ccl::rslang::operator""_t;

class UTjson: public ::testing::Test {
protected:
  using StrRange = ccl::StrRange;
  using ValueClass = ccl::rslang::ValueClass;
  using ParsingStatus = ccl::semantic::ParsingStatus;
  using CstType = ccl::semantic::CstType;
  using DataType = ccl::src::DataType;
  using SrcType = ccl::src::SrcType;
  using EquationMode = ccl::ops::Equation::Mode;
  using OpsType = ccl::ops::Type;

  using RSModel = ccl::semantic::RSModel;
  using RSForm = ccl::semantic::RSForm;
  using RSCore = ccl::semantic::RSCore;
  using TextInterpretation = ccl::semantic::TextInterpretation;
  using ConceptRecord = ccl::semantic::ConceptRecord;
  using TrackingFlags = ccl::semantic::TrackingFlags;
  using ExpressionType = ccl::rslang::ExpressionType;
  using Typification = ccl::rslang::Typification;
  using SyntaxTree = ccl::rslang::SyntaxTree;
  using TokenData = ccl::rslang::TokenData;
  using Token = ccl::rslang::Token;
  using TokenID = ccl::rslang::TokenID;
  using Error = ccl::rslang::Error;
  using LexicalTerm = ccl::lang::LexicalTerm;
  using ManagedText = ccl::lang::ManagedText;
  using SrcHandle = ccl::src::Handle;
  using OSSchema = ccl::oss::OSSchema;
  using Pict = ccl::oss::Pict;
  using MediaLink = ccl::oss::MediaLink;
  using OperationHandle = ccl::oss::OperationHandle;
  using Grid = ccl::oss::Grid;
  using GridPosition = ccl::oss::GridPosition;
  using TranslationData = ccl::ops::TranslationData;
  using EquationOptions = ccl::ops::EquationOptions;
  using Equation = ccl::ops::Equation;
  using EntityTranslation = ccl::EntityTranslation;
};

TEST_F(UTjson, OSSchema) {
  OSSchema initial{};
  initial.title = "oss_title";
  initial.Src().ossDomain = u8"Domain";
  const auto base1 = initial.InsertBase()->uid;
  initial.SetPictTitle(base1, "base1");
  initial.SetPictAlias(base1, "B1");
  initial.SetPictLink(base1, {"add", "sub"});
  const auto base2 = initial.InsertBase()->uid;
  initial.SetPictTitle(base2, "base2");
  initial.SetPictAlias(base2, "B2");
  initial.Grid().ShiftPict(base2, 1);

  const auto result = initial.InsertOperation(base1, base2)->uid;
  auto equations = std::make_unique<EquationOptions>();
  equations->Insert(42, 1337);
  initial.Ops().InitFor(result, OpsType::rsSynt, std::move(equations));

  const auto json = JSON(initial);
  OSSchema restored{};
  json.get_to(restored);

  EXPECT_EQ(restored.title, initial.title);
  EXPECT_EQ(std::string{ reinterpret_cast<const char*>(restored.Src().ossDomain.c_str()) }, 
            std::string{ reinterpret_cast<const char*>(initial.Src().ossDomain.c_str()) });
  EXPECT_EQ(restored.size(), initial.size());
  EXPECT_EQ(restored(base1)->title, initial(base1)->title);
  EXPECT_EQ(restored(base1)->alias, initial(base1)->alias);
  EXPECT_EQ(restored(base1)->lnk, initial(base1)->lnk);
  EXPECT_EQ(restored(base1)->dataType, initial(base1)->dataType);
  EXPECT_EQ(restored(base2)->title, initial(base2)->title);
  EXPECT_EQ(restored(base2)->alias, initial(base2)->alias);
  EXPECT_EQ(restored(base2)->lnk, initial(base2)->lnk);
  EXPECT_EQ(restored(base2)->dataType, initial(base2)->dataType);
  EXPECT_EQ(restored(result)->title, initial(result)->title);
  EXPECT_EQ(restored(result)->alias, initial(result)->alias);
  EXPECT_EQ(restored(result)->lnk, initial(result)->lnk);
  EXPECT_EQ(restored(result)->dataType, initial(result)->dataType);
  EXPECT_EQ(*restored.Grid()(base1), *initial.Grid()(base1));
  EXPECT_EQ(*restored.Grid()(base2), *initial.Grid()(base2));
  EXPECT_EQ(*restored.Grid()(result), *initial.Grid()(result));
  EXPECT_EQ(restored.Graph().ChildrenOf(base1), initial.Graph().ChildrenOf(base1));
  EXPECT_EQ(restored.Graph().ChildrenOf(base2), initial.Graph().ChildrenOf(base2));
  EXPECT_EQ(restored.Graph().ChildrenOf(result), initial.Graph().ChildrenOf(result));
  EXPECT_EQ(restored.Graph().ParentsOf(base1), initial.Graph().ParentsOf(base1));
  EXPECT_EQ(restored.Graph().ParentsOf(base2), initial.Graph().ParentsOf(base2));
  EXPECT_EQ(restored.Graph().ParentsOf(result), initial.Graph().ParentsOf(result));
  EXPECT_EQ(restored.Ops()(result)->type, initial.Ops()(result)->type);
  EXPECT_EQ(restored.Ops()(result)->outdated, initial.Ops()(result)->outdated);
  EXPECT_EQ(restored.Ops()(result)->translations, initial.Ops()(result)->translations);

  const auto* initialOpts = dynamic_cast<const EquationOptions*>(initial.Ops()(result)->options.get());
  const auto* restoredOpts = dynamic_cast<const EquationOptions*>(restored.Ops()(result)->options.get());
  EXPECT_EQ(restoredOpts->begin()->first, initialOpts->begin()->first);
  EXPECT_EQ(restoredOpts->begin()->second, initialOpts->begin()->second);
  EXPECT_EQ(restoredOpts->PropsFor(42).mode, initialOpts->PropsFor(42).mode);
  EXPECT_EQ(restoredOpts->PropsFor(42).arg, initialOpts->PropsFor(42).arg);
}

TEST_F(UTjson, RSModel) {
  RSModel initial{};
  initial.title = "full";
  initial.alias = "short";
  initial.comment = "comment";

  ConceptRecord x1{};
  x1.uid = 42;
  x1.type = CstType::base;
  x1.alias = "X1";
  initial.InsertCopy(x1);

  ConceptRecord x2{};
  x2.uid = 43;
  x2.type = CstType::base;
  x2.alias = "X2";
  initial.InsertCopy(x2);

  ConceptRecord a1{};
  a1.uid = 1337;
  a1.type = CstType::axiom;
  a1.alias = "A1";
  a1.rs = "X1=X1";

  initial.InsertCopy(a1);
  initial.Values().SetBasicText(x1.uid, TextInterpretation{ {"one", "two"} });
  initial.Calculations().RecalculateAll();

  const auto json = JSON(initial);
  RSModel restored{};
  json.get_to(restored);

  ASSERT_EQ(std::size(restored.List()), std::size(initial.List()));
  EXPECT_EQ(restored.title, initial.title);
  EXPECT_EQ(restored.alias, initial.alias);
  EXPECT_EQ(restored.comment, initial.comment);
  EXPECT_EQ(restored.GetRS(x1.uid).type, x1.type);
  EXPECT_EQ(restored.GetRS(x1.uid).alias, x1.alias);
  EXPECT_EQ(restored.GetRS(x2.uid).type, x2.type);
  EXPECT_EQ(restored.GetRS(x2.uid).alias, x2.alias);
  EXPECT_EQ(restored.GetRS(a1.uid).type, a1.type);
  EXPECT_EQ(restored.GetRS(a1.uid).alias, a1.alias);
  EXPECT_EQ(restored.Calculations().WasCalculated(a1.uid), initial.Calculations().WasCalculated(a1.uid));
  EXPECT_EQ(restored.Values().SDataFor(x1.uid), initial.Values().SDataFor(x1.uid));
  ASSERT_TRUE(restored.Values().TextFor(x1.uid) != nullptr);
  EXPECT_EQ(*restored.Values().TextFor(x1.uid), *initial.Values().TextFor(x1.uid));
  EXPECT_EQ(restored.Values().SDataFor(x2.uid), initial.Values().SDataFor(x2.uid));
  ASSERT_TRUE(restored.Values().TextFor(x2.uid) != nullptr);
  EXPECT_EQ(*restored.Values().TextFor(x2.uid), *initial.Values().TextFor(x2.uid));
  EXPECT_EQ(restored.Values().StatementFor(a1.uid), initial.Values().StatementFor(a1.uid));
}

TEST_F(UTjson, RSModelMinimal) {
  auto json = JSON::parse(R"({"items": [], "data": []})");
  RSModel restored{};
  json.get_to(restored);
  EXPECT_EQ(restored.title, "");
  EXPECT_EQ(restored.alias, "");
  EXPECT_EQ(restored.comment, "");
}

TEST_F(UTjson, RSForm) {
  RSForm initial{};
  initial.title = "full";
  initial.alias = "short";
  initial.comment = "comment";

  TrackingFlags flags{};
  flags.convention = true;
  flags.allowEdit = false;

  ConceptRecord cst{};
  cst.uid = 42;
  cst.type = CstType::term;
  cst.alias = "D42";

  initial.Load(ConceptRecord{ cst });
  initial.Mods().Track(cst.uid, flags);

  const auto json = JSON(initial);
  const auto restored = json.get<RSForm>();

  ASSERT_EQ(std::size(restored.List()), std::size(initial.List()));
  EXPECT_EQ(restored.title, initial.title);
  EXPECT_EQ(restored.alias, initial.alias);
  EXPECT_EQ(restored.comment, initial.comment);
  EXPECT_EQ(restored.GetRS(cst.uid).type, cst.type);
  EXPECT_EQ(restored.Mods()(cst.uid)->convention, flags.convention);
  EXPECT_EQ(restored.Mods()(cst.uid)->allowEdit, flags.allowEdit);
}

TEST_F(UTjson, RSFormMinimal) {
  auto json = JSON::parse(R"({"items": []})");
  const auto restored = json.get<RSForm>();
  EXPECT_EQ(restored.title, "");
  EXPECT_EQ(restored.alias, "");
  EXPECT_EQ(restored.comment, "");
}

TEST_F(UTjson, RSCore) {
  RSCore initial{};
  
  ConceptRecord cst1{};
  cst1.uid = 42;
  cst1.type = CstType::base;
  cst1.alias = "X1";
  cst1.convention = "definition";

  ConceptRecord cst2{};
  cst2.uid = 1337;
  cst2.type = CstType::term;
  cst2.alias = "D1";
  cst2.rs = "X1\\X1";

  initial.Load(ConceptRecord{ cst1 });
  initial.Load(ConceptRecord{ cst2 });

  const auto json = JSON(initial);
  const auto restored = json.get<RSCore>();

  ASSERT_EQ(std::size(restored), std::size(initial));
  EXPECT_EQ(*std::begin(restored), *std::begin(initial));
  EXPECT_EQ(restored.GetRS(cst1.uid).type, cst1.type);
  EXPECT_EQ(restored.GetRS(cst1.uid).alias, cst1.alias);
  EXPECT_EQ(restored.GetRS(cst1.uid).convention, cst1.convention);
  EXPECT_EQ(restored.GetRS(cst2.uid).type, cst2.type);
  EXPECT_EQ(restored.GetRS(cst2.uid).alias, cst2.alias);
  EXPECT_EQ(restored.GetRS(cst2.uid).definition, cst2.rs);
}

TEST_F(UTjson, TextInterpretation) {
  TextInterpretation initial{ {"item1", "item2"} };
  const auto json = JSON(initial);
  const auto restored = json.get<TextInterpretation>();
  EXPECT_EQ(restored , initial);
}

TEST_F(UTjson, ConceptRecord) {
  ConceptRecord initial{};
  const auto form = "nomn,sing"_form;
  initial.uid = 42;
  initial.alias = "X1";
  initial.type = CstType::base;
  initial.rs = "X1\\X1";
  initial.convention = "convention";
  initial.term = LexicalTerm{ "Test_raw", "Test_resolved" };
  initial.term.SetForm(form, "Form");
  initial.definition = ManagedText{ "Test_raw", "Test_resolved" };

  const auto json = JSON(initial);
  const auto restored = json.get<ConceptRecord>();

  EXPECT_EQ(restored.uid, initial.uid);
  EXPECT_EQ(restored.type, initial.type);
  EXPECT_EQ(restored.rs, initial.rs);
  EXPECT_EQ(restored.convention, initial.convention);
  EXPECT_EQ(restored.term, initial.term);
  EXPECT_EQ(restored.term.GetForm(form), initial.term.GetForm(form));
  EXPECT_EQ(restored.definition, initial.definition);
  EXPECT_EQ(restored.definition.Raw(), initial.definition.Raw());
}

TEST_F(UTjson, ConceptRecordMinimal) {
  auto json = JSON::parse(R"({"entityUID": 1, "cstType": "base", "alias": "X1"})");
  const auto restored = json.get<ConceptRecord>();
  EXPECT_EQ(restored.uid, 1U);
  EXPECT_EQ(restored.type, CstType::base);
  EXPECT_EQ(restored.alias, "X1");
}

TEST_F(UTjson, TrackingFlags) {
  TrackingFlags initial{};
  initial.allowEdit = true;
  initial.convention = false;
  initial.term = true;

  const auto json = JSON(initial);
  const auto restored = json.get<TrackingFlags>();
  EXPECT_EQ(restored, initial);
  EXPECT_EQ(restored.allowEdit, initial.allowEdit);
  EXPECT_EQ(restored.convention, initial.convention);
  EXPECT_EQ(restored.definition, initial.definition);
  EXPECT_EQ(restored.term, initial.term);
}

TEST_F(UTjson, LexicalTerm) {
  LexicalTerm initial{"Test_raw", "Test_resolved"};
  const auto form = "nomn,sing"_form;
  initial.SetForm(form, "Form");

  const auto json = JSON(initial);
  const auto restored = json.get<LexicalTerm>();
  EXPECT_EQ(restored, initial);
  EXPECT_EQ(restored.Text().Raw(), initial.Text().Raw());
  EXPECT_EQ(restored.Text().Str(), initial.Text().Str());
  EXPECT_EQ(restored.GetForm(form), initial.GetForm(form));
}

TEST_F(UTjson, LexicalTermMinimal) {
  auto json = JSON::parse(R"({"raw": "test"})");
  const auto restored = json.get<LexicalTerm>();
  EXPECT_EQ(restored.Text().Str(), "test");
}

TEST_F(UTjson, ManagedText) {
  ManagedText initial{ "Test_raw", "Test_resolved" };
  const auto json = JSON(initial);
  const auto restored = json.get<ManagedText>();
  EXPECT_EQ(restored, initial);
  EXPECT_EQ(restored.Raw(), initial.Raw());
  EXPECT_EQ(restored.Str(), initial.Str());
}

TEST_F(UTjson, ManagedTextMinimal) {
  auto json = JSON::parse(R"({"raw": "test"})");
  const auto restored = json.get<ManagedText>();
  EXPECT_EQ(restored.Raw(), "test");
  EXPECT_EQ(restored.Str(), "test");
}

TEST_F(UTjson, SrcHandle) {
  SrcHandle initial{};
  initial.desc.name = u8"42";
  initial.desc.type = SrcType::rsDoc;
  initial.coreHash = 42;
  initial.fullHash = 1337;
  const auto json = JSON(initial);
  const auto restored = json.get<SrcHandle>();
  EXPECT_EQ(restored.desc, initial.desc);
  EXPECT_EQ(restored.coreHash, initial.coreHash);
  EXPECT_EQ(restored.fullHash, initial.fullHash);
}

TEST_F(UTjson, Pict) {
  Pict initial{};
  initial.uid = 42;
  initial.alias = "alias";
  initial.title = "title";
  initial.comment = "comment";
  initial.dataType = DataType::rsSchema;
  initial.lnk = MediaLink{"1", "2"};

  const auto json = JSON(initial);
  const auto restored = json.get<Pict>();
  EXPECT_EQ(restored.uid, initial.uid);
  EXPECT_EQ(restored.alias, initial.alias);
  EXPECT_EQ(restored.title, initial.title);
  EXPECT_EQ(restored.comment, initial.comment);
  EXPECT_EQ(restored.dataType, initial.dataType);
  EXPECT_EQ(restored.lnk, initial.lnk);
}

TEST_F(UTjson, MediaLink) {
  MediaLink initial{ "address", "subaddress" };
  const auto json = JSON(initial);
  const auto restored = json.get<MediaLink>();
  EXPECT_EQ(restored, initial);
  EXPECT_EQ(restored.address, initial.address);
  EXPECT_EQ(restored.subAddr, initial.subAddr);
}

TEST_F(UTjson, OperationHandle) {
  OperationHandle initial{};
  initial.broken = true;
  initial.outdated = false;
  initial.options = nullptr;
  initial.translations = std::make_unique<TranslationData>();
  initial.translations->push_back({});
  initial.translations->begin()->Insert(42, 1337);
  initial.translations->push_back({});

  const auto json = JSON(initial);
  const auto restored = json.get<OperationHandle>();
  EXPECT_EQ(restored.broken, initial.broken);
  EXPECT_EQ(restored.outdated, initial.outdated);
  EXPECT_EQ(restored.options, initial.options);
  EXPECT_EQ(*restored.translations, *initial.translations);
}

TEST_F(UTjson, Grid) {
  Grid initial{};
  initial.emplace(GridPosition{ 1, 1 }, 42);
  initial.emplace(GridPosition{ 1, 2 }, 1337);
  const auto json = JSON(initial);
  const auto restored = json.get<Grid>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, GridPosition) {
  GridPosition initial{ 10, 11 };
  const auto json = JSON(initial);
  const auto restored = json.get<GridPosition>();
  EXPECT_EQ(restored, initial);
  EXPECT_EQ(restored.row, initial.row);
  EXPECT_EQ(restored.column, initial.column);
}

TEST_F(UTjson, EquationOptions) {
  EquationOptions initial{};
  initial.Insert(42, 1337, Equation{ EquationMode::createNew, "Test_term1" });
  initial.Insert(43, 1338, Equation{ EquationMode::keepDel, "Test_term2"});
  const auto json = JSON(initial);
  const auto restored = json.get<EquationOptions>();
  EXPECT_EQ(restored.PropsFor(42).mode, initial.PropsFor(42).mode);
  EXPECT_EQ(restored.PropsFor(42).arg, initial.PropsFor(42).arg);
  EXPECT_EQ(restored.PropsFor(43).mode, initial.PropsFor(43).mode);
  EXPECT_EQ(restored.PropsFor(43).arg, initial.PropsFor(43).arg);
}

TEST_F(UTjson, Equation) {
  Equation initial{ EquationMode::createNew, "Test_term" };
  const auto json = JSON(initial);
  const auto restored = json.get<Equation>();
  EXPECT_EQ(restored.arg, initial.arg);
  EXPECT_EQ(restored.mode, initial.mode);
}

TEST_F(UTjson, EntityTranslation) {
  EntityTranslation initial{};
  initial.Insert(42, 1337);
  initial.Insert(1, 40);
  initial.Insert(2, 37);
  const auto json = JSON(initial);
  const auto restored = json.get<EntityTranslation>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, ValueClass) {
  ValueClass initial{ ValueClass::value };
  const auto json = JSON(initial);
  const auto restored = json.get<ValueClass>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, ParsingStatus) {
  ParsingStatus initial{ ParsingStatus::VERIFIED };
  const auto json = JSON(initial);
  const auto restored = json.get<ParsingStatus>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, CstType) {
  CstType initial{ CstType::function };
  const auto json = JSON(initial);
  const auto restored = json.get<CstType>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, DataType) {
  DataType initial{ DataType::rsSchema };
  const auto json = JSON(initial);
  const auto restored = json.get<DataType>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, SrcType) {
  SrcType initial{ SrcType::rsDoc };
  const auto json = JSON(initial);
  const auto restored = json.get<SrcType>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, EquationMode) {
  EquationMode initial{ EquationMode::keepDel };
  const auto json = JSON(initial);
  const auto restored = json.get<EquationMode>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, OpsType) {
  OpsType initial{ OpsType::rsSynt };
  const auto json = JSON(initial);
  const auto restored = json.get<OpsType>();
  EXPECT_EQ(restored, initial);
}

TEST_F(UTjson, ExpressionType) {
  const ExpressionType logic = ccl::rslang::LogicT{};
  const ExpressionType typif = "B(X1*X1)"_t;
  EXPECT_EQ(JSON(logic), "LOGIC");
  EXPECT_EQ(JSON(typif), std::get<Typification>(typif).ToString());
}

TEST_F(UTjson, Typification) {
  const auto typif = "B(X1*X1)"_t;
  EXPECT_EQ(JSON(typif), typif.ToString());
}

TEST_F(UTjson, TokenData) {
  TokenData empty{};
  TokenData integer{ 1 };
  TokenData str{ "alpha"};
  const auto indecies = std::vector<ccl::rslang::Index>{ 1, 2, 3 };
  TokenData tupleInt{ indecies };
  EXPECT_EQ(JSON(empty).at("dataType"), "none");
  EXPECT_EQ(JSON(empty).at("value"), "");
  EXPECT_EQ(JSON(integer).at("dataType"), "integer");
  EXPECT_EQ(JSON(integer).at("value").get<int32_t>(), 1);
  EXPECT_EQ(JSON(str).at("dataType"), "string");
  EXPECT_EQ(JSON(str).at("value"), "alpha");
  EXPECT_EQ(JSON(tupleInt).at("dataType"), "tuple");
  EXPECT_EQ(JSON(tupleInt).at("value").get<decltype(indecies)>(), indecies);
}

TEST_F(UTjson, Error) {
  Error err{ 0x2001, 42 };
  err.params = {"42", "test"};
  JSON jsonErr(err);
  EXPECT_EQ(jsonErr.at("errorType").get<decltype(err.eid)>(), err.eid);
  EXPECT_EQ(jsonErr.at("position").get<decltype(err.position)>(), err.position);
  EXPECT_EQ(jsonErr.at("isCritical").get<bool>(), err.IsCritical());
  EXPECT_EQ(jsonErr.at("params").get<decltype(err.params)>(), err.params);
}

TEST_F(UTjson, SyntaxTree) {
  using Node = SyntaxTree::Node;
  Node root{ Token{ TokenID::AND, StrRange{2, 4}, TokenData{42} } };
  root.AddChildCopy(Node{ Token{ TokenID::OR, StrRange{0, 5} } });
  root.AddChildCopy(Node{ Token{ TokenID::BOOL, StrRange{6, 10} } });
  root(0).AddChildCopy(Node{ Token{ TokenID::ID_LOCAL, StrRange{0, 3}}});
  root(0).AddChildCopy(Node{ Token{ TokenID::ID_GLOBAL, StrRange{4, 5} } });
  root(1).AddChildCopy(Node{ Token{ TokenID::LIT_INTEGER, StrRange{1, 2} } });
  SyntaxTree ast{ std::make_unique<Node>(root) };

  JSON jsonAST(ast);
  ASSERT_EQ(jsonAST.size(), 6U);
  EXPECT_EQ(jsonAST[0].at("uid").get<int32_t>(), 0);
  EXPECT_EQ(jsonAST[0].at("parent").get<int32_t>(), 0);
  EXPECT_EQ(jsonAST[0].at("typeID").get<TokenID>(), TokenID::AND);
  EXPECT_EQ(jsonAST[2].at("uid").get<int32_t>(), 2);
  EXPECT_EQ(jsonAST[2].at("parent").get<int32_t>(), 1);
  EXPECT_EQ(jsonAST[2].at("typeID").get<TokenID>(), TokenID::ID_LOCAL);
}