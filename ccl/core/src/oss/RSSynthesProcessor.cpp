#include "ccl/oss/RSSynthesProcessor.h"

#include "ccl/ops/RSOperations.h"
#include "ccl/semantic/RSForm.h"

namespace ccl::oss {

std::optional<src::DataType> 
RSSProcessor::ResultingTypeFor(ops::Type /*operation*/, const ops::ArgsInfo& args) const {
  assert(ssize(args) == 2);

  if (std::any_of(begin(args), end(args),
                  [](const auto& arg)
                  { return arg != src::DataType::rsSchema; })) {
    return std::nullopt;
  } else {
    return src::DataType::rsSchema;
  }
}

bool RSSProcessor::CheckCall(const ops::Call& call) const {
  if (!CanProcess(call)) {
    return false;
  } else if (call.type == ops::Type::rsMerge) {
    const auto* params = dynamic_cast<const ops::EquationOptions*>(call.params);
    return params == nullptr || std::empty(*params);
  } else {
    const auto* operand1 = src::CastData<semantic::RSForm>(call.args.at(0));
    const auto* operand2 = src::CastData<semantic::RSForm>(call.args.at(1));
    const auto* params = dynamic_cast<const ops::EquationOptions*>(call.params);
    return ops::BinarySynthes(*operand1, *operand2, *params).IsCorrectlyDefined();
  }
}

bool RSSProcessor::CheckTranslations(const ops::TranslationData& translations, const src::DataStream& data) const {
  const auto* schema = src::CastData<semantic::RSForm>(&data);
  for (const auto& translation : translations) {
    for (const auto& element : translation) {
      if (!schema->Contains(element.second)) {
        return false;
      }
    }
  }
  return true;
}

std::optional<ops::Result> RSSProcessor::Execute(const ops::Call& call) const {
  const auto* operand1 = src::CastData<semantic::RSForm>(call.args.at(0));
  const auto* operand2 = src::CastData<semantic::RSForm>(call.args.at(1));
  assert(operand1 != nullptr);
  assert(operand2 != nullptr);

  const auto* paramsPtr = dynamic_cast<const ops::EquationOptions*>(call.params);
  const auto params = paramsPtr == nullptr ? ops::EquationOptions{} : *paramsPtr;
  auto operation = ops::BinarySynthes(*operand1, *operand2, params);
  auto schema = operation.Execute();
  return ops::Result{ 
    meta::UniqueCPPtr<src::DataStream>{ std::move(schema) },
    std::make_unique<ops::TranslationData>(operation.Translations()) 
  };
}

EntityTranslation RSSProcessor::CreateVersionTranslation(ops::Type /*operation*/,
  const ops::TranslationData& oldT, const ops::TranslationData& newT) const {
  EntityTranslation result{};
  for (auto i = 0U; i < size(oldT); ++i) {
    const auto& oldTrans = oldT.at(i);
    const auto& newTrans = newT.at(i);
    for (const auto& [key, value] :oldTrans) {
      if (newTrans.ContainsKey(key)) {
        result.Insert(value, newTrans(key));
      }
    }
  }
  return result;
}

} // namespace ccl::oss