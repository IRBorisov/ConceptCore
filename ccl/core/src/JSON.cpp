#include "ccl/tools/JSON.h"

#include "ccl/rslang/RSExpr.h"

using JSON = nlohmann::ordered_json;

namespace {

template<typename ObjectType>
void LoadOptionalKey(const JSON& data, const std::string_view key, ObjectType& destination) {
  if (data.contains(key)) {
    data.at(key).get_to(destination);
  }
}

JSON ExtractPicts(const ccl::oss::OSSchema& oss) {
  auto pictsArray = JSON::array();
  for (const auto& pict : oss) {
    auto object = JSON(pict);
    if (const auto* srcHandle = oss.Src()(pict.uid); srcHandle != nullptr) {
      object["attachedSource"] = *srcHandle;
    }
    if (const auto* opHandle = oss.Ops()(pict.uid); opHandle != nullptr) {
      object["attachedOperation"] = *opHandle;
    }
    object["position"] = oss.Grid()(pict.uid).value();
    pictsArray += std::move(object);
  }
  return pictsArray;
}

void LoadPicts(const JSON& data, ccl::oss::OSSchema& oss) {
  using ccl::oss::Pict;
  using ccl::oss::PictID;
  using ccl::oss::GridPosition;
  using ccl::oss::OperationHandle;
  using ccl::src::Handle;

  for (auto it = begin(data); it != end(data); ++it) {
    auto pict = it->get<Pict>();
    auto position = it->at("position").get<GridPosition>();
    Handle srcHandle{};
    std::unique_ptr<OperationHandle> opHandle{ nullptr };
    if (it->contains("attachedSource")) {
      it->at("attachedSource").get_to(srcHandle);
    }
    if (it->contains("attachedOperation")) {
      opHandle = std::make_unique<OperationHandle>();
      it->at("attachedOperation").get_to(*opHandle);
    }
    oss.LoadPict(std::move(pict), position, srcHandle, std::move(opHandle));
  }
}

JSON ExtractData(const ccl::semantic::RSModel& model) {
  using ccl::object::SDCompact;

  auto data = JSON::array();
  for (const auto uid : model.Core()) {
    auto object = JSON{
      {"entityUID", uid},
      {"wasCalculated", model.Calculations().WasCalculated(uid)}
    };
    const auto type = model.GetRS(uid).type;
    if (ccl::semantic::IsRSObject(type)) {
      const auto* typif = model.GetParse(uid).Typification();
      const auto rsData = model.Values().SDataFor(uid);
      if (typif != nullptr && rsData.has_value()) {
        object["value"] = SDCompact::FromSData(rsData.value(), *typif).data;
      }
      if (const auto* texts = model.Values().TextFor(uid); texts != nullptr && texts->size() > 0) {
        object["texts"] = *texts;
      }
    } else if (!ccl::semantic::IsCallable(type)) {
      if (const auto logicValue = model.Values().StatementFor(uid); logicValue.has_value()) {
        object["value"] = logicValue.value();
      }
    }
    data += std::move(object);
  }
  return data;
}

void LoadData(const JSON& data, ccl::semantic::RSModel& model) {
  using ccl::object::SDCompact;
  using ccl::EntityUID;
  using ccl::SetOfEntities;
  using ccl::semantic::TextInterpretation;

  SetOfEntities calculated{};
  for (auto it = begin(data); it != end(data); ++it) {
    const auto uid = it->at("entityUID").get<EntityUID>();
    if (it->at("wasCalculated").get<bool>()) {
      calculated.insert(uid);
    }
    const auto type = model.GetRS(uid).type;
    if (ccl::semantic::IsRSObject(type)) {
      if (it->contains("value")) {
        const auto* typification = model.GetParse(uid).Typification();
        if (typification != nullptr) {
          const auto compact = it->at("value").get<SDCompact::Data>();
          const auto rsdata = SDCompact::Unpack(compact, *typification);
          if (rsdata.has_value()) {
            model.Values().LoadData(uid, rsdata.value());
          }
        }
      }
      if (it->contains("texts")) {
        model.Values().LoadData(uid, it->at("texts").get<TextInterpretation>());
      }
    } else if (!ccl::semantic::IsCallable(type) && it->contains("value")) {
      model.Values().LoadData(uid, it->at("value").get<bool>());
    }
  }
  model.Calculations().LoadCalulated(calculated);
}

//! Traverse AST to produce JSON
class AST2json final : public ccl::rslang::ASTVisitor<AST2json> {
  friend class ccl::rslang::SyntaxTree::Cursor;
  friend class ASTVisitor<AST2json>;

  using NodeIndex = size_t;

  NodeIndex baseID = 0;
  std::unordered_map<const ccl::rslang::Token*, NodeIndex> ids{};

public:
  JSON jsonNodes = JSON::array();

protected:
  bool VisitDefault(Cursor iter) {
    JSON node{};
    const auto myID = baseID;
    ++baseID;
    ids.emplace(&*iter, myID);
    node["uid"] = myID;
    
    if (iter.IsRoot()) {
      node["parent"] = myID;
    } else {
      node["parent"] = ids.at(&iter.Parent());
    }
    node["typeID"] = iter->id;
    node["start"] = iter->pos.start;
    node["finish"] = iter->pos.finish;
    node["data"] = iter->data;

    jsonNodes += std::move(node);
    for (ccl::rslang::Index child = 0; child < iter.ChildrenCount(); ++child) {
      VisitChild(iter, child);
    }
    return true;
  }
};


} // namespace

namespace ccl {

namespace semantic {

void to_json(JSON& object, const RSModel& model) {
  object = {
    {"type", "rsmodel"},
    {"title", model.title},
    {"alias", model.alias},
    {"comment", model.comment},
    {"items", model.Core()}
  };
  object["data"] = ExtractData(model);
}

void from_json(const JSON& object, RSModel& model) {
  LoadOptionalKey(object, "title", model.title);
  LoadOptionalKey(object, "alias", model.alias);
  LoadOptionalKey(object, "comment", model.comment);
  object.at("items").get_to<RSCore>(model.LoadCore());
  model.FinalizeLoadingCore();
  LoadData(object.at("data"), model);
}

void to_json(JSON& object, const RSForm& schema) {
  object = {
    {"type", "rsform"},
    {"title", schema.title},
    {"alias", schema.alias},
    {"comment", schema.comment},
    {"items", schema.Core()}
  };
  object["tracking"] = JSON::array();
  for (const auto uid : schema.List()) {
    const auto* trackPtr = schema.Mods()(uid);
    if (trackPtr != nullptr) {
      object["tracking"] += {
        {"entityUID", uid},
        {"flags", *trackPtr}
      };
    }
  }
}

void from_json(const JSON& object, RSForm& schema) {
  LoadOptionalKey(object, "title", schema.title);
  LoadOptionalKey(object, "alias", schema.alias);
  LoadOptionalKey(object, "comment", schema.comment);
  object.at("items").get_to(schema.LoadCore());
  if (object.contains("tracking")) {
    const auto& tracking = object.at("tracking");
    for (auto it = begin(tracking); it != end(tracking); ++it) {
      const auto uid = it->at("entityUID").get<EntityUID>();
      schema.Mods().Track(uid, it->at("flags").get<TrackingFlags>());
    }
  }
}

void to_json(JSON& object, const RSCore& core) {
  object = JSON::array();
  for (const auto uid : core.List()) {
    const auto& parse = core.GetParse(uid);
    const auto* typification = parse.Typification();
    const auto* exprTree = core.RSLang().ASTContext()(core.GetRS(uid).alias);
    JSON cstJSON(core.AsRecord(uid));
    cstJSON["parse"] = {
      {"status", parse.status},
      {"valueClass", parse.valueClass}
    };
    if (typification != nullptr) {
      cstJSON["parse"]["typification"] = *typification;
    } else {
      cstJSON["parse"]["typification"] = "";
    }
    if (exprTree != nullptr) {
      cstJSON["parse"]["syntaxTree"] = ccl::rslang::AST2String::Apply(*exprTree);
    } else {
      cstJSON["parse"]["syntaxTree"] = "";
    }
    if (parse.arguments.has_value()) {
      cstJSON["parse"]["args"] = parse.arguments.value();
    } else {
      cstJSON["parse"]["args"] = JSON::array();
    }
    object += std::move(cstJSON);
  }
}

void from_json(const JSON& object, RSCore& core) {
  for (auto it = begin(object); it != end(object); ++it) {
    core.Load(it->get<ConceptRecord>());
  }
  core.UpdateState();
}

void to_json(JSON& object, const TextInterpretation& text) {
  object = JSON::array();
  for (const auto& textElement : text) {
    object += textElement.second;
  }
}

void from_json(const JSON& object, TextInterpretation& text) {
  for (auto it = begin(object); it != end(object); ++it) {
    text.PushBack(it->get<std::string>());
  }
}

void to_json(JSON& object, const ConceptRecord& record) {
  object = {
    {"entityUID", record.uid},
    {"type", "constituenta"},
    {"cstType", record.type},
    {"alias", record.alias},
    {"convention", record.convention},
    {"term", record.term},
    {"definition",{
      {"formal", record.rs},
      {"text", record.definition}
    }}
  };
}

void from_json(const JSON& object, ConceptRecord& record) {
  object.at("entityUID").get_to(record.uid);
  object.at("cstType").get_to(record.type);
  object.at("alias").get_to(record.alias);
  LoadOptionalKey(object, "convention", record.convention);
  LoadOptionalKey(object, "term", record.term);
  if (object.contains("definition")) {
    LoadOptionalKey(object.at("definition"), "formal", record.rs);
    LoadOptionalKey(object.at("definition"), "text", record.definition);
  }
}

void to_json(JSON& object, const TrackingFlags& mods) {
  object = {
    {"mutable", mods.allowEdit},
    {"editTerm", mods.term},
    {"editDefinition", mods.definition},
    {"editConvention", mods.convention}
  };
}

void from_json(const JSON& object, TrackingFlags& mods) {
  object.at("mutable").get_to(mods.allowEdit);
  object.at("editTerm").get_to(mods.term);
  object.at("editDefinition").get_to(mods.definition);
  object.at("editConvention").get_to(mods.convention);
}

} // namespace semantic


namespace rslang {

void to_json(JSON& object, const ExpressionType& type) {
  object = std::visit(meta::Overloads{
    [](const Typification& t) { return t.ToString();  },
    [](const LogicT& /*t*/) { return std::string{"LOGIC"}; },
                      }, type);
}

void to_json(JSON& object, const FunctionArguments& args) {
  object = JSON::array();
  for (const auto& arg: args) {
    object += {
      { "alias", arg.name },
      { "typification", arg.type.ToString() }
    };
  }
}

void to_json(JSON& object, const Typification& typif) {
  object = typif.ToString();
}

void to_json(JSON& object, const SyntaxTree& ast) {
  AST2json visitor{};
  ast.Root().DispatchVisit(visitor);
  object = std::move(visitor.jsonNodes);
}

void to_json(JSON& object, const TokenData& data) {
  if (!data.HasValue()) {
    object = {
      {"dataType", "none"},
      {"value", ""}
    };
  } else if (data.IsInt()) {
    object = {
      {"dataType", "integer"},
      {"value", data.ToInt()}
    };
  } else if (data.IsText()) {
    object = {
      {"dataType", "string"},
      {"value", data.ToText()}
    };
  } else if (data.IsTuple()) {
    object = {
      {"dataType", "tuple"},
      {"value", data.ToTuple()}
    };
  } else {
    object = {
      {"dataType", "unknown"},
      {"value", ""}
    };
  }
}

void to_json(JSON& object, const Error& error) {
  object = {
    {"errorType", error.eid},
    {"position", error.position},
    {"isCritical", error.IsCritical()},
    {"params", error.params}
  };
}

} // namespace rslang


namespace lang {

void to_json(JSON& object, const LexicalTerm& term) {
  object = term.Text();
  object["forms"] = JSON::array();
  for (const auto& [form, text] : term.GetAllManual()) {
    object["forms"] += JSON{
      {"text", text},
      {"tags", form.ToString()}
    };
  }
}

void from_json(const JSON& object, LexicalTerm& term) {
  term = LexicalTerm{ object.get<ManagedText>() };
  if (object.contains("forms")) {
    const auto& forms = object.at("forms");
    for (auto it = begin(forms); it != end(forms); ++it) {
      const auto morpho = Morphology{ it->at("tags").get<std::string>() };
      term.SetForm(morpho, it->at("text").get<std::string>());
    }
  }
}

void to_json(JSON& object, const ManagedText& text) {
  object = {
    {"raw", text.Raw()},
    {"resolved", text.Str()},
  };
}

void from_json(const JSON& object, ManagedText& text) {
  std::string resolved{};
  LoadOptionalKey(object, "resolved", resolved);
  text = ManagedText{
    object.at("raw").get<std::string>(),
    std::move(resolved)
  };
}

} // namespace lang


namespace src {

void to_json(JSON& object, const Handle& srcHandle) {
  object = {
    {"resourceID", u8to_string(srcHandle.desc.name)},
    {"resourceType", srcHandle.desc.type},
    {"coreHash", srcHandle.coreHash},
    {"fullHash", srcHandle.fullHash}
  };
}

void from_json(const JSON& object, Handle& srcHandle) {
  srcHandle.desc.name = to_u8string(object.at("resourceID").get<std::string>());
  object.at("resourceType").get_to(srcHandle.desc.type);
  object.at("coreHash").get_to(srcHandle.coreHash);
  object.at("fullHash").get_to(srcHandle.fullHash);
}

} // namespace ccl::src


namespace oss {

void to_json(JSON& object, const OSSchema& oss) {
  object = {
    {"type", "oss"},
    {"title", oss.title},
    {"comment", oss.comment},
    {"sourceDomain", u8to_string(oss.Src().ossDomain)},
    {"items", ExtractPicts(oss)},
    {"layout", oss.Grid().data()},
    {"connections", oss.Graph().EdgeList()},
  };
}

void from_json(const JSON& object, OSSchema& oss) {
  object.at("title").get_to(oss.title);
  object.at("comment").get_to(oss.comment);
  oss.Src().ossDomain = to_u8string(object.at("sourceDomain").get<std::string>());
  LoadPicts(object.at("items"), oss);
  const auto connections = object.at("connections").get<std::vector<std::pair<PictID, PictID>>>();
  for (const auto& [child, parent] : connections) {
    oss.Graph().LoadParent(child, parent);
  }
}

void to_json(JSON& object, const Pict& pict) {
  object = {
    {"pictUID", pict.uid},
    {"dataType", pict.dataType},
    {"title", pict.title},
    {"alias", pict.alias},
    {"comment", pict.comment},
    {"link", pict.lnk}
  };
  if (!std::empty(pict.lnk)) {
    object["link"] = pict.lnk;
  }
}

void from_json(const JSON& object, Pict& pict) {
  object.at("pictUID").get_to(pict.uid);
  object.at("dataType").get_to(pict.dataType);
  object.at("title").get_to(pict.title);
  object.at("alias").get_to(pict.alias);
  object.at("comment").get_to(pict.comment);
  if (object.contains("link")) {
    object.at("link").get_to(pict.lnk);
  }
}

void to_json(JSON& object, const MediaLink& link) {
  object = {
    {"address", link.address},
    {"subAddr", link.subAddr}
  };
}

void from_json(const JSON& object, MediaLink& link) {
  object.at("address").get_to(link.address);
  object.at("subAddr").get_to(link.subAddr);
}

void to_json(JSON& object, const OperationHandle& operation) {
  object = {
    {"operationType", operation.type},
    {"isBroken", operation.broken },
    {"isOutdated", operation.outdated}
  };
  const auto* options = dynamic_cast<const ops::EquationOptions*>(operation.options.get());
  if (options != nullptr) {
    object["options"] = JSON{
      {"type", "equations"},
      {"data", *options}
    };
  }
  if (operation.translations != nullptr) {
    object["translations"] = *operation.translations;
  }
}

void from_json(const JSON& object, OperationHandle& operation) {
  object.at("operationType").get_to(operation.type);
  object.at("isBroken").get_to(operation.broken);
  object.at("isOutdated").get_to(operation.outdated);
  if (object.contains("options")) {
    auto opts = std::make_unique<ops::EquationOptions>();
    object.at("options").at("data").get_to(*opts);
    operation.options = meta::UniqueCPPtr<ops::Options>{ std::move(opts) };
  }
  if (object.contains("translations")) {
    operation.translations = std::make_unique<ops::TranslationData>();
    object.at("translations").get_to(*operation.translations);
  }
}

void to_json(JSON& object, const Grid& grid) {
  object = JSON::array();
  for (const auto& [position, pid] : grid) {
    auto newItem = JSON(position);
    newItem["pictUID"] = pid;
    object += std::move(newItem);
  }
}

void from_json(const JSON& object, Grid& grid) {
  for (auto it = begin(object); it != end(object); ++it) {
    grid.emplace(it->get<GridPosition>(), it->at("pictUID").get<PictID>());
  }
}

void to_json(JSON& object, const GridPosition& position) {
  object = {
    {"row", position.row},
    {"column", position.column}
  };
}

void from_json(const JSON& object, GridPosition& position) {
  object.at("row").get_to(position.row);
  object.at("column").get_to(position.column);
}

} // namespace oss


namespace ops {

void to_json(JSON& object, const TranslationData& translations) {
  object = JSON(translations);
}

void from_json(const JSON& object, TranslationData& translations) {
  object.get_to(translations);
}

void to_json(JSON& object, const EquationOptions& equations) {
  object = JSON::array();
  for (const auto& [key, value] : equations) {
    object += JSON{
      {"operand1", key},
      {"operand2", value},
      {"parameters", equations.PropsFor(key)},
    };
  }
}

void from_json(const JSON& object, EquationOptions& equations) {
  for (auto it = begin(object); it != end(object); ++it) {
    equations.Insert(
      it->at("operand1").get<EntityUID>(),
      it->at("operand2").get<EntityUID>(),
      it->at("parameters").get<ops::Equation>()
    );
  }
}

void to_json(JSON& object, const Equation& equation) {
  object = {
    {"equationType", equation.mode},
    {"newTerm", equation.arg}
  };
}

void from_json(const JSON& object, Equation& equation) {
  object.at("equationType").get_to(equation.mode);
  object.at("newTerm").get_to(equation.arg);
}

} // namespace ops

void to_json(JSON& object, const EntityTranslation& translation) {
  object = JSON::array();
  for (const auto& [uid1, uid2] : translation) {
    object += {uid1, uid2};
  }
}

void from_json(const JSON& object, EntityTranslation& translation) {
  for (auto it = begin(object); it != end(object); ++it) {
    auto key = begin(*it)->get<EntityUID>();
    auto value = next(begin(*it))->get<EntityUID>();
    translation.Insert(key, value);
  }
}

} // namespace ccl