#include "pyconcept.h"

#include "ccl/rslang/RSGenerator.h"
#include "ccl/api/RSFormJA.h"
#include "ccl/lang/TextEnvironment.h"

using ccl::api::RSFormJA;

std::string CheckSchema(const std::string& jSchema) {
	// TODO: consider try-catch if need to process invalid input gracefully
	ccl::lang::TextEnvironment::Instance().skipResolving = true;
	auto schema = RSFormJA::FromJSON(jSchema);
	return schema.ToJSON();
}

std::string ResetAliases(const std::string& jSchema) {
	ccl::lang::TextEnvironment::Instance().skipResolving = true;
	auto schema = RSFormJA::FromJSON(jSchema);
	schema.data().ResetAliases();
	return schema.ToJSON();
}

std::string ConvertToASCII(const std::string& expression) {
	return ccl::rslang::ConvertTo(expression, ccl::rslang::Syntax::ASCII);
}

std::string ConvertToMath(const std::string& expression) {
	return ccl::rslang::ConvertTo(expression, ccl::rslang::Syntax::MATH);
}

std::string ParseExpression(const std::string& expression) {
	return ccl::api::ParseExpression(expression);
}

std::string CheckExpression(const std::string& jSchema, const std::string& expression) {
	auto schema = RSFormJA::FromJSON(jSchema);
	return schema.CheckExpression(expression);
}