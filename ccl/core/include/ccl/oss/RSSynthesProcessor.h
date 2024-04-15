#pragma once

#include "ccl/oss/OperationProcessor.hpp"

namespace ccl::oss {

//! RSForm synthesis processor for OSS
struct RSSProcessor : OperationsProcessor {
  [[nodiscard]] std::optional<src::DataType> 
    ResultingTypeFor(ops::Type /*operation*/,
                     const ops::ArgsInfo& args) const override;

  [[nodiscard]] bool CheckCall(const ops::Call& call) const override;

  [[nodiscard]] std::optional<ops::Result>
    Execute(const ops::Call& call) const override;

  [[nodiscard]] EntityTranslation
    CreateVersionTranslation(ops::Type /*operation*/,
                             const ops::TranslationData& oldT,
                             const ops::TranslationData& newT) const override;

  [[nodiscard]] bool CheckTranslations(const ops::TranslationData& translations,
                                       const src::DataStream& data) const override;
};

} // namespace ccl::oss