#pragma once

#include "ccl/Entity.hpp"
#include "ccl/Operation.hpp"

namespace ccl::oss {

//! Abstract OSS operations processor
class OperationsProcessor {
public:
  virtual ~OperationsProcessor() noexcept = default;
  OperationsProcessor() = default;

protected:
  OperationsProcessor(const OperationsProcessor&) = default;
  OperationsProcessor& operator=(const OperationsProcessor&) = default;
  OperationsProcessor(OperationsProcessor&&) noexcept = default;
  OperationsProcessor& operator=(OperationsProcessor&&) noexcept = default;

public:
  [[nodiscard]] virtual std::optional<src::DataType>
    ResultingTypeFor(ops::Type /*operation*/, const ops::ArgsInfo& /*args*/) const = 0;

  [[nodiscard]] virtual bool CheckCall(const ops::Call& /*call*/) const = 0;

  [[nodiscard]] virtual std::optional<ops::Result> Execute(const ops::Call& /*call*/) const = 0;

  [[nodiscard]] virtual EntityTranslation
    CreateVersionTranslation(ops::Type /*operation*/,
                             const ops::TranslationData& /*oldT*/,
                             const ops::TranslationData& /*newT*/) const = 0;

  [[nodiscard]] virtual bool CheckTranslations(const ops::TranslationData& /*translations*/,
                                               const src::DataStream& /*data*/) const = 0;

  [[nodiscard]] bool CanProcess(ops::Type operation,
                                const ops::ArgsInfo& args) const {
    return ResultingTypeFor(operation, args).has_value();
  }
  [[nodiscard]] bool CanProcess(const ops::Call& def) const {
    return CanProcess(def.type, def.Info());
  }
};

} // namespace ccl::oss