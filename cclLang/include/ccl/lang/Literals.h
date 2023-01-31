#pragma once

#include "ccl/lang/Morphology.h"
#include "ccl/lang/Reference.h"

namespace ccl::lang {

//! Morphology from string of comma separated tags
Morphology operator""_form(const char* input, size_t /*size*/);

//! Reference from string
Reference operator""_ref(const char* input, size_t /*size*/);

} // namespace ccl::lang