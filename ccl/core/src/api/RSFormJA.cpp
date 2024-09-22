#include "ccl/api/RSFormJA.h"

#include "ccl/tools/JSON.h"
#include "ccl/lang/TextEnvironment.h"

using JSON = nlohmann::ordered_json;

namespace ccl::api {

std::string ParseExpression(const std::string& expression, const rslang::Syntax syntaxHint) {
  using rslang::Syntax;
  rslang::Parser parser{};

  JSON result{};
  const auto isCorrect = parser.Parse(expression, syntaxHint);
  result["parseResult"] = isCorrect;
  result["syntax"] = parser.syntax;
  result["errors"] = JSON::array();
  for (const auto& error : parser.Errors().All()) {
    result["errors"] += error;
  }

  if (isCorrect) {
    result["astText"] = rslang::AST2String::Apply(parser.AST());
    result["ast"] = parser.AST();
  } else {
    result["astText"] = "";
    result["ast"] = JSON::array();
  }
  return result.dump(JSON_IDENT);
}

const semantic::RSForm& RSFormJA::data() const noexcept {
  return *schema;
}

semantic::RSForm& RSFormJA::data() noexcept {
  return *schema;
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

std::string RSFormJA::CheckExpression(const std::string& text, const rslang::Syntax syntaxHint) const {
  JSON result{};

  auto analyser = schema->Core().RSLang().MakeAuditor();
  const auto typeOK = analyser->CheckExpression(text, syntaxHint);
  const auto valueOK = typeOK && analyser->CheckValue();

  result["parseResult"] = typeOK;
  result["syntax"] = analyser->GetSyntax();
  result["prefixLen"] = 0;
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
  result["args"] = analyser->GetDeclarationArgs();

  result["errors"] = JSON::array();
  for (const auto& error : analyser->Errors().All()) {
    result["errors"] += error;
  }

  if (analyser->IsParsed()) {
    result["astText"] = rslang::AST2String::Apply(analyser->AST());
    result["ast"] = analyser->AST();
  } else {
    result["astText"] = "";
    result["ast"] = JSON::array();
  }

  return result.dump(JSON_IDENT);
}

std::string RSFormJA::CheckConstituenta(
  const std::string& alias,
  const std::string& definition,
  semantic::CstType targetType
) const {
  JSON result{};

  auto analyser = schema->Core().RSLang().MakeAuditor();
  const auto typeOK = analyser->CheckConstituenta(alias, definition, targetType);
  const auto valueOK = typeOK && analyser->CheckValue();

  result["parseResult"] = typeOK;
  result["syntax"] = analyser->GetSyntax();
  result["prefixLen"] = analyser->prefixLen;
  if (typeOK) {
    result["typification"] = analyser->GetType();
  }
  else {
    result["typification"] = "N/A";
  }
  if (valueOK) {
    result["valueClass"] = analyser->GetValueClass();
  }
  else {
    result["valueClass"] = ccl::rslang::ValueClass::invalid;
  }
  result["args"] = analyser->GetDeclarationArgs();

  result["errors"] = JSON::array();
  for (const auto& error : analyser->Errors().All()) {
    result["errors"] += error;
  }

  if (typeOK) {
    result["astText"] = rslang::AST2String::Apply(analyser->AST());
    result["ast"] = analyser->AST();
  }
  else {
    result["astText"] = "";
    result["ast"] = JSON::array();
  }

  return result.dump(JSON_IDENT);
}

} // namespace ccl::api
