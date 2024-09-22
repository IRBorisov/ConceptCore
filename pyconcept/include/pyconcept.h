#pragma once

#include <string>

#include <pybind11/pybind11.h>

// ======== Schema manipulations =======
std::string CheckSchema(const std::string& jSchema);
std::string ResetAliases(const std::string& jSchema);

// ======== Syntax conversion ==========
std::string ConvertToASCII(const std::string& expression);
std::string ConvertToMath(const std::string& expression);

// ======= Expression parse =========
std::string ParseExpression(const std::string& expression);
std::string CheckExpression(const std::string& jSchema, const std::string& expression);
std::string CheckConstituenta(
  const std::string& jSchema,
  const std::string& alias,
  const std::string& expression,
  const std::string& cstType
);

PYBIND11_MODULE(pyconcept, m) {
  m.def("check_schema", &CheckSchema, R"pbdoc(Check schema definition.)pbdoc");
  m.def("reset_aliases", &ResetAliases, R"pbdoc(Reset constituents aliases.)pbdoc");

  m.def("convert_to_ascii", &ConvertToASCII, R"pbdoc(Convert expression syntax to ASCII.)pbdoc");
  m.def("convert_to_math", &ConvertToMath, R"pbdoc(Convert expression syntax to Math.)pbdoc");

  m.def("parse_expression", &ParseExpression, R"pbdoc(Parse expression and create syntax tree.)pbdoc");
  m.def(
    "check_expression",
    &CheckExpression,
    R"pbdoc(Validate expression against given schema and calculate typification.)pbdoc"
  );
  m.def(
    "check_constituenta",
    &CheckConstituenta,
    R"pbdoc(Validate constituenta expression against given schema and calculate typification.)pbdoc"
  );
}