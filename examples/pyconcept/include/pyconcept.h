#pragma once

#include <string>

#include <pybind11/pybind11.h>

std::string CheckSchema(const std::string& jSchema);
std::string ParseExpression(const std::string& jSchema, const std::string& expression);

PYBIND11_MODULE(pyconcept, m) {
  m.def("check_schema", &CheckSchema, R"pbdoc(Check schema definition.)pbdoc");
  m.def("parse_expression", &ParseExpression, R"pbdoc(Parse expression.)pbdoc");
}