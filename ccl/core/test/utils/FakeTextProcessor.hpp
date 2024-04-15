#pragma once

#include "ccl/lang/TextEnvironment.h"

struct FakeTextProcessor : ccl::lang::TextProcessor {
  static inline std::string x1Ref = "@{X1|nomn|sing}";
};
