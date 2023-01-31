#pragma once

#include "ccl/rslang/StructuredData.h"

#include <string>
#include <functional>
#include <optional>

namespace ccl::rslang {

//! Evaluation context for structured data
using DataContext = std::function<std::optional<object::StructuredData>(const std::string&)>;

} // namespace ccl::rslang