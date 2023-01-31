#include "ccl/tools/JSON.h"

#include "ccl/rslang/RSExpr.h"

namespace nlo = nlohmann;

namespace {

nlo::ordered_json ExtractPicts(const ccl::oss::OSSchema& oss) {
	auto pictsArray = nlo::ordered_json::array();
	for (const auto& pict : oss) {
		auto object = nlo::ordered_json(pict);
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

void LoadPicts(const nlo::ordered_json& data, ccl::oss::OSSchema& oss) {
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
			it->at("attachedOperation").get_to(*opHandle.get());
		}
		oss.LoadPict(std::move(pict), position, srcHandle, std::move(opHandle));
	}
}

nlo::ordered_json ExtractData(const ccl::semantic::RSModel& model) {
	using ccl::object::SDCompact;

	auto data = nlo::ordered_json::array();
	for (const auto uid : model.Core()) {
		auto object = nlo::ordered_json{
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

void LoadData(const nlo::ordered_json& data, ccl::semantic::RSModel& model) {
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

} // namespace

namespace ccl {

namespace semantic {

void to_json(nlo::ordered_json& object, const RSModel& model) {
	object = {
		{"type", "rsmodel"},
		{"title", model.title},
		{"alias", model.alias},
		{"comment", model.comment},
		{"items", model.Core()}
	};
	object["data"] = ExtractData(model);
}

void from_json(const nlo::ordered_json& object, RSModel& model) {
	object.at("title").get_to<std::string>(model.title);
	object.at("alias").get_to<std::string>(model.alias);
	object.at("comment").get_to<std::string>(model.comment);
	object.at("items").get_to<RSCore>(model.LoadCore());
	model.FinalizeLoadingCore();
	LoadData(object.at("data"), model);
}

void to_json(nlo::ordered_json& object, const RSForm& schema) {
	object = {
		{"type", "rsform"},
		{"title", schema.title},
		{"alias", schema.alias},
		{"comment", schema.comment},
		{"items", schema.Core()}
	};
	object["tracking"] = nlo::ordered_json::array();
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

void from_json(const nlo::ordered_json& object, RSForm& schema) {
	object.at("title").get_to(schema.title);
	object.at("alias").get_to(schema.alias);
	object.at("comment").get_to(schema.comment);
	object.at("items").get_to(schema.LoadCore());
	const auto& tracking = object.at("tracking");
	for (auto it = begin(tracking); it != end(tracking); ++it) {
		const auto uid = it->at("entityUID").get<EntityUID>();
		schema.Mods().Track(uid, it->at("flags").get<TrackingFlags>());
	}
}

void to_json(nlo::ordered_json& object, const RSCore& core) {
	object = nlo::ordered_json::array();
	for (const auto uid : core.List()) {
		const auto& parse = core.GetParse(uid);
		const auto* typification = core.GetParse(uid).Typification();
		const auto typeStr = typification != nullptr ? typification->ToString() : std::string{};
		const auto* exprTree = core.RSLang().ASTContext()(core.GetRS(uid).alias);
		nlo::ordered_json cstJSON(core.AsRecord(uid));
		cstJSON["parse"] = {
			{"status", parse.status},
			{"valueClass", parse.valueClass},
			{"typification", typeStr},
			{"syntaxTree", exprTree ? ccl::rslang::AST2String::Apply(*exprTree) : ""}
		};
		object += std::move(cstJSON);
	}
}

void from_json(const nlo::ordered_json& object, RSCore& core) {
	for (auto it = begin(object); it != end(object); ++it) {
		core.Load(it->get<ConceptRecord>());
	}
	core.UpdateState();
}

void to_json(nlo::ordered_json& object, const TextInterpretation& text) {
	object = nlo::ordered_json::array();
	for (const auto& textElement : text) {
		object += textElement.second;
	}
}

void from_json(const nlo::ordered_json& object, TextInterpretation& text) {
	for (auto it = begin(object); it != end(object); ++it) {
		text.PushBack(it->get<std::string>());
	}
}

void to_json(nlo::ordered_json& object, const ConceptRecord& record) {
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

void from_json(const nlo::ordered_json& object, ConceptRecord& record) {
	object.at("entityUID").get_to(record.uid);
	object.at("cstType").get_to(record.type);
	object.at("alias").get_to(record.alias);
	object.at("convention").get_to(record.convention);
	object.at("term").get_to(record.term);
	object.at("definition").at("formal").get_to(record.rs);
	object.at("definition").at("text").get_to(record.definition);
}

void to_json(nlo::ordered_json& object, const TrackingFlags& mods) {
	object = {
		{"mutable", mods.allowEdit},
		{"editTerm", mods.term},
		{"editDefinition", mods.definition},
		{"editConvention", mods.convention}
	};
}

void from_json(const nlo::ordered_json& object, TrackingFlags& mods) {
	object.at("mutable").get_to(mods.allowEdit);
	object.at("editTerm").get_to(mods.term);
	object.at("editDefinition").get_to(mods.definition);
	object.at("editConvention").get_to(mods.convention);
}

} // namespace ccl::semantic


namespace lang {

void to_json(nlo::ordered_json& object, const LexicalTerm& term) {
	object = term.Text();
	object["forms"] = nlo::ordered_json::array();
	for (const auto& [form, text] : term.GetAllManual()) {
		object["forms"] += nlo::ordered_json{
			{"text", text},
			{"tags", form.ToString()}
		};
	}
}

void from_json(const nlo::ordered_json& object, LexicalTerm& term) {
	term = LexicalTerm{ object.get<ManagedText>() };
	const auto& forms = object.at("forms");
	for (auto it = begin(forms); it != end(forms); ++it) {
		const auto morpho = Morphology{ it->at("tags").get<std::string>() };
		term.SetForm(morpho, it->at("text").get<std::string>());
	}
}

void to_json(nlo::ordered_json& object, const ManagedText& text) {
	object = {
		{"raw", text.Raw()},
		{"resolved", text.Str()},
	};
}

void from_json(const nlo::ordered_json& object, ManagedText& text) {
	text = ManagedText{
		object.at("raw").get<std::string>(),
		object.at("resolved").get<std::string>()
	};
}

} // namespace ccl::lang


namespace src {

void to_json(nlo::ordered_json& object, const Handle& srcHandle) {
	object = {
		{"resourceID", u8to_string(srcHandle.desc.name)},
		{"resourceType", srcHandle.desc.type},
		{"coreHash", srcHandle.coreHash},
		{"fullHash", srcHandle.fullHash}
	};
}

void from_json(const nlo::ordered_json& object, Handle& srcHandle) {
	srcHandle.desc.name = to_u8string(object.at("resourceID").get<std::string>());
	object.at("resourceType").get_to(srcHandle.desc.type);
	object.at("coreHash").get_to(srcHandle.coreHash);
	object.at("fullHash").get_to(srcHandle.fullHash);
}

} // namespace ccl::src


namespace oss {

void to_json(nlo::ordered_json& object, const OSSchema& oss) {
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

void from_json(const nlo::ordered_json& object, OSSchema& oss) {
	object.at("title").get_to(oss.title);
	object.at("comment").get_to(oss.comment);
	oss.Src().ossDomain = to_u8string(object.at("sourceDomain").get<std::string>());
	LoadPicts(object.at("items"), oss);
	const auto connections = object.at("connections").get<std::vector<std::pair<PictID, PictID>>>();
	for (const auto& [child, parent] : connections) {
		oss.Graph().LoadParent(child, parent);
	}
}

void to_json(nlohmann::ordered_json& object, const Pict& pict) {
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

void from_json(const nlohmann::ordered_json& object, Pict& pict) {
	object.at("pictUID").get_to(pict.uid);
	object.at("dataType").get_to(pict.dataType);
	object.at("title").get_to(pict.title);
	object.at("alias").get_to(pict.alias);
	object.at("comment").get_to(pict.comment);
	if (object.contains("link")) {
		object.at("link").get_to(pict.lnk);
	}
}

void to_json(nlo::ordered_json& object, const MediaLink& link) {
	object = {
		{"address", link.address},
		{"subAddr", link.subAddr}
	};
}

void from_json(const nlo::ordered_json& object, MediaLink& link) {
	object.at("address").get_to(link.address);
	object.at("subAddr").get_to(link.subAddr);
}

void to_json(nlo::ordered_json& object, const OperationHandle& operation) {
	object = {
		{"operationType", operation.type},
		{"isBroken", operation.broken },
		{"isOutdated", operation.outdated}
	};
	const auto* options = dynamic_cast<const ops::EquationOptions*>(operation.options.get());
	if (options != nullptr) {
		object["options"] = nlo::ordered_json{
			{"type", "equations"},
			{"data", *options}
		};
	}
	if (operation.translations != nullptr) {
		object["translations"] = *operation.translations;
	}
}

void from_json(const nlo::ordered_json& object, OperationHandle& operation) {
	object.at("operationType").get_to(operation.type);
	object.at("isBroken").get_to(operation.broken);
	object.at("isOutdated").get_to(operation.outdated);
	if (object.contains("options")) {
		auto opts = std::make_unique<ops::EquationOptions>();
		object.at("options").at("data").get_to(*opts.get());
		operation.options = meta::UniqueCPPtr<ops::Options>{ std::move(opts) };
	}
	if (object.contains("translations")) {
		operation.translations = std::make_unique<ops::TranslationData>();
		object.at("translations").get_to(*operation.translations.get());
	}
}

void to_json(nlo::ordered_json& object, const Grid& grid) {
	object = nlo::ordered_json::array();
	for (const auto& [position, pid] : grid) {
		auto newItem = nlo::ordered_json(position);
		newItem["pictUID"] = pid;
		object += std::move(newItem);
	}
}

void from_json(const nlo::ordered_json& object, Grid& grid) {
	for (auto it = begin(object); it != end(object); ++it) {
		grid.emplace(it->get<GridPosition>(), it->at("pictUID").get<PictID>());
	}
}

void to_json(nlo::ordered_json& object, const GridPosition& position) {
	object = {
		{"row", position.row},
		{"column", position.column}
	};
}

void from_json(const nlo::ordered_json& object, GridPosition& position) {
	object.at("row").get_to(position.row);
	object.at("column").get_to(position.column);
}

} // namespace ccl::oss


namespace ops {

void to_json(nlo::ordered_json& object, const TranslationData& translations) {
	object = nlo::ordered_json(translations);
}

void from_json(const nlo::ordered_json& object, TranslationData& translations) {
	object.get_to(translations);
}

void to_json(nlo::ordered_json& object, const EquationOptions& equations) {
	object = nlo::ordered_json::array();
	for (const auto& [key, value] : equations) {
		object += nlo::ordered_json{
			{"operand1", key},
			{"operand2", value},
			{"parameters", equations.PropsFor(key)},
		};
	}
}

void from_json(const nlo::ordered_json& object, EquationOptions& equations) {
	for (auto it = begin(object); it != end(object); ++it) {
		equations.Insert(
			it->at("operand1").get<EntityUID>(),
			it->at("operand2").get<EntityUID>(),
			it->at("parameters").get<ops::Equation>()
		);
	}
}

void to_json(nlo::ordered_json& object, const Equation& equation) {
	object = {
		{"equationType", equation.mode},
		{"newTerm", equation.arg}
	};
}

void from_json(const nlo::ordered_json& object, Equation& equation) {
	object.at("equationType").get_to(equation.mode);
	object.at("newTerm").get_to(equation.arg);
}

} // namespace ccl::ops

void to_json(nlo::ordered_json& object, const EntityTranslation& translation) {
	object = nlo::ordered_json::array();
	for (const auto& [uid1, uid2] : translation) {
		object += {uid1, uid2};
	}
}

void from_json(const nlo::ordered_json& object, EntityTranslation& translation) {
	for (auto it = begin(object); it != end(object); ++it) {
		auto key = begin(*it)->get<EntityUID>();
		auto value = next(begin(*it))->get<EntityUID>();
		translation.Insert(key, value);
	}
}

} // namespace ccl