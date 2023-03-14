#include "pyconcept.h"

#include "ccl/api/RSFormJA.h"

using ccl::api::RSFormJA;

std::string CheckSchema(const std::string& jSchema) {
	// TODO: consider try-catch if need to process invalid input gracefully
	auto schema = RSFormJA::FromJSON(jSchema);
	return schema.ToJSON();
}

std::string ParseExpression(const std::string& jSchema, const std::string& expression) {
	auto schema = RSFormJA::FromJSON(jSchema);
	return schema.ParseExpression(expression);
}