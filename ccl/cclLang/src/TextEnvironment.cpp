#include "ccl/lang/TextEnvironment.h"

namespace ccl::lang {

void TextEnvironment::SetProcessor(std::unique_ptr<TextProcessor> newProcessor) noexcept {
  Instance().processor = std::move(newProcessor);
}

TextProcessor& TextEnvironment::Processor() noexcept {
  return *Instance().processor;
}

TextEnvironment& TextEnvironment::Instance() noexcept {
  static TextEnvironment instance{};
  return instance;
}

} // namespace ccl::lang