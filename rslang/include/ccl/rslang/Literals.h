#pragma once

#include "ccl/rslang/Typification.h"

#include <string>

namespace ccl::rslang {

Typification operator""_t(const char* input, size_t size);
std::string operator "" _rs(const char* input, size_t size);

} // namespace ccl::rslang