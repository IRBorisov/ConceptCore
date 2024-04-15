#pragma once

#include "ccl/Source.hpp"
#include "ccl/semantic/Schema.h"
#include "ccl/semantic/CstType.hpp"
#include "ccl/ops/EquationOptions.h"
#include "ccl/lang/Morphology.h"
#include "ccl/rslang/ValueClass.hpp"

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 4051 4060 )
  #pragma warning( disable : 26416 26433 26434 26438 26440 26443 26444 26446 26447 )
  #pragma warning( disable : 26454 26456 26462 26473 26475 26476 )
  #pragma warning( disable : 26481 26482 26493 26494 26495 26496 26800 26819 28020 )
#endif

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wsign-conversion"
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic push
#endif

#include "nlohmann/json.hpp"

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 26495 ) // Note: Disable warnings because of lambdas in macros
#endif

namespace ccl::rslang {

NLOHMANN_JSON_SERIALIZE_ENUM(
  ValueClass,
  {
    { ValueClass::invalid, "invalid" },
    { ValueClass::value, "value" },
    { ValueClass::props, "property" },
  }
)

NLOHMANN_JSON_SERIALIZE_ENUM(
  Syntax,
  {
    { Syntax::UNDEF, "undefined" },
    { Syntax::ASCII, "ascii" },
    { Syntax::MATH, "math" },
  }
)

} // namespace ccl::rslang

namespace ccl::semantic {

NLOHMANN_JSON_SERIALIZE_ENUM(
  ParsingStatus,
  {
    { ParsingStatus::UNKNOWN, "undefined" },
    { ParsingStatus::VERIFIED, "verified" },
    { ParsingStatus::INCORRECT, "incorrect" },
  }
)

NLOHMANN_JSON_SERIALIZE_ENUM(
  CstType,
  {
    { CstType::base, "basic" },
    { CstType::constant, "constant" },
    { CstType::structured, "structure" },
    { CstType::axiom, "axiom" },
    { CstType::term, "term" },
    { CstType::function, "function" },
    { CstType::theorem, "theorem" },
    { CstType::predicate, "predicate" },
  }
)

} // namespace ccl::semantic

namespace ccl::src {

NLOHMANN_JSON_SERIALIZE_ENUM(
  DataType,
  {
    { DataType::tba, "tba" },
    { DataType::rsSchema, "schema" },
  }
)

NLOHMANN_JSON_SERIALIZE_ENUM(
  SrcType,
  {
    { SrcType::tba, "tba" },
    { SrcType::rsDoc, "rsDocument" },
  }
)

} // namespace ccl::src

namespace ccl::ops {

NLOHMANN_JSON_SERIALIZE_ENUM(
  Equation::Mode,
  {
    { Equation::Mode::keepHier, "keepSecond" },
    { Equation::Mode::keepDel, "keepFirst" },
    { Equation::Mode::createNew, "createNew" },
  }
)

NLOHMANN_JSON_SERIALIZE_ENUM(
  Type,
  {
    { Type::tba, "tba" },
    { Type::rsMerge, "rsMerge"},
    { Type::rsSynt, "rsSynt" },
  }
)

} // namespace ccl::ops

#ifdef _MSC_VER
  #pragma warning( pop )
#endif