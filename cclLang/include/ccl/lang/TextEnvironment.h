#pragma once

#include "ccl/lang/TextProcessor.h"

#include <memory>

namespace ccl::lang {

class TextEnvironment final {
	std::unique_ptr<TextProcessor> processor{ std::make_unique<TextProcessor>() };

public:
	static void SetProcessor(std::unique_ptr<TextProcessor> newProcessor) noexcept;
	[[nodiscard]] static TextProcessor& Processor() noexcept;
	[[nodiscard]] static TextEnvironment& Instance() noexcept;
};

} // namespace ccl::lang