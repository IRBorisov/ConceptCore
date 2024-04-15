#pragma once

#define GTEST_LANG_CXX11 1

#include "gtest/gtest.h"

#include <windows.h>
#undef min
#undef max
#undef IN

#include "ConceptCore.h"

#include "ccl\rslang\Literals.h"
#include "ccl\Strings.hpp"

using ccl::rslang::operator""_rs;
using ccl::rslang::operator""_t;
using ccl::operator""_c17;

//#include <format>
//#include <exception>
//#include <memory>