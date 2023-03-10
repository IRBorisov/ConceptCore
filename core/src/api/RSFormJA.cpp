#include "ccl/api/RSFormJA.h"

#include "ccl/tools/JSON.h"

using JSON = nlohmann::ordered_json;

namespace ccl::api {

const semantic::RSForm& RSFormJA::data() const noexcept {
  return *schema.get();
}

semantic::RSForm& RSFormJA::data() noexcept {
  return *schema.get();
}

RSFormJA RSFormJA::FromData(semantic::RSForm&& data) {
  RSFormJA result{};
  result.schema = std::make_unique<semantic::RSForm>(std::move(data));
  return result;
}

RSFormJA RSFormJA::FromJSON(const std::string_view json) {
  RSFormJA result{};
  auto object = JSON::parse(json);
  result.schema = std::make_unique<semantic::RSForm>();
  object.get_to(result.data());
  return result;
}

std::string RSFormJA::ToJSON() const {
  return JSON(data()).dump(JSON_IDENT);
}

std::string RSFormJA::ToMinimalJSON() const {
	JSON object = {
    {"type", "rsform"},
    {"title", schema->title},
    {"alias", schema->alias},
    {"comment", schema->comment}
  };
	object["items"] = JSON::array();
  for (const auto uid : schema->Core().List()) {
		object["items"] += schema->Core().AsRecord(uid);
  }
  return object.dump(JSON_IDENT);
}

std::string RSFormJA::ParseExpression(const std::string& text) const {
	JSON result{};

	auto analyser = schema->Core().RSLang().MakeAuditor();
	const auto typeOK = analyser->CheckType(text);
	const auto valueOK = typeOK && analyser->CheckValue();

	result["parseResult"] = typeOK;
	if (typeOK) {
		result["typification"] = analyser->GetType();
	} else {
		result["typification"] = "N/A";
	}
	if (valueOK) {
		result["valueClass"] = analyser->GetValueClass();
	} else {
		result["valueClass"] = ccl::rslang::ValueClass::invalid;
	}

	result["errors"] = JSON::array();
	for (const auto& error : analyser->Errors().All()) {
		result["errors"] += error;
	}

	if (analyser->isParsed) {
		result["astText"] = rslang::AST2String::Apply(analyser->AST());
		result["ast"] = analyser->AST();
	} else {
		result["astText"] = "";
		result["ast"] = "";
	}

  return result.dump(JSON_IDENT);
}

} // namespace ccl::api
