#pragma once

#include "ccl/tools/EnumJSON.hpp"

#include "ccl/semantic/RSModel.h"
#include "ccl/semantic/RSForm.h"
#include "ccl/oss/OSSchema.h"
#include "ccl/ops/RSOperations.h"
#include "ccl/rslang/SDataCompact.h"

namespace ccl {

namespace semantic {

void to_json(nlohmann::ordered_json& object, const RSModel& model);
void from_json(const nlohmann::ordered_json& object, RSModel& model);

void to_json(nlohmann::ordered_json& object, const RSForm& schema);
void from_json(const nlohmann::ordered_json& object, RSForm& schema);

void to_json(nlohmann::ordered_json& object, const RSCore& core);
void from_json(const nlohmann::ordered_json& object, RSCore& core);

void to_json(nlohmann::ordered_json& object, const TextInterpretation& text);
void from_json(const nlohmann::ordered_json& object, TextInterpretation& text);

void to_json(nlohmann::ordered_json& object, const ConceptRecord& record);
void from_json(const nlohmann::ordered_json& object, ConceptRecord& record);

void to_json(nlohmann::ordered_json& object, const TrackingFlags& mods);
void from_json(const nlohmann::ordered_json& object, TrackingFlags& mods);

} // namespace ccl::semantic

namespace rslang {

void to_json(nlohmann::ordered_json& object, const ExpressionType& type);
void to_json(nlohmann::ordered_json& object, const Typification& typif);
void to_json(nlohmann::ordered_json& object, const FunctionArguments& args);
void to_json(nlohmann::ordered_json& object, const TokenData& data);
void to_json(nlohmann::ordered_json& object, const SyntaxTree& ast);
void to_json(nlohmann::ordered_json& object, const Error& error);

} // namespace ccl::rslang

namespace lang {

void to_json(nlohmann::ordered_json& object, const LexicalTerm& term);
void from_json(const nlohmann::ordered_json& object, LexicalTerm& term);

void to_json(nlohmann::ordered_json& object, const ManagedText& text);
void from_json(const nlohmann::ordered_json& object, ManagedText& text);

} // namespace ccl::lang

namespace src {

void to_json(nlohmann::ordered_json& object, const Handle& srcHandle);
void from_json(const nlohmann::ordered_json& object, Handle& srcHandle);

} // namespace ccl::src

namespace oss {

void to_json(nlohmann::ordered_json& object, const OSSchema& oss);
void from_json(const nlohmann::ordered_json& object, OSSchema& oss);

void to_json(nlohmann::ordered_json& object, const Pict& pict);
void from_json(const nlohmann::ordered_json& object, Pict& pict);

void to_json(nlohmann::ordered_json& object, const MediaLink& link);
void from_json(const nlohmann::ordered_json& object, MediaLink& link);

void to_json(nlohmann::ordered_json& object, const OperationHandle& operation);
void from_json(const nlohmann::ordered_json& object, OperationHandle& operation);

void to_json(nlohmann::ordered_json& object, const Grid& grid);
void from_json(const nlohmann::ordered_json& object, Grid& grid);

void to_json(nlohmann::ordered_json& object, const GridPosition& position);
void from_json(const nlohmann::ordered_json& object, GridPosition& position);

} // namespace ccl::oss

namespace ops {

void to_json(nlohmann::ordered_json& object, const TranslationData& translations);
void from_json(const nlohmann::ordered_json& object, TranslationData& translations);

void to_json(nlohmann::ordered_json& object, const EquationOptions& equations);
void from_json(const nlohmann::ordered_json& object, EquationOptions& equations);

void to_json(nlohmann::ordered_json& object, const Equation& equation);
void from_json(const nlohmann::ordered_json& object, Equation& equation);


} // namespace ccl::ops

void to_json(nlohmann::ordered_json& object, const EntityTranslation& translation);
void from_json(const nlohmann::ordered_json& object, EntityTranslation& translation);

} // namespace ccl