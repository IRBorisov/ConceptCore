#pragma once

#include "ccl/rslang/RSToken.h"
#include "ccl/rslang/Literals.h"
#include "ccl/lang/Reference.h"

using ccl::rslang::operator""_rs;
using ccl::rslang::operator""_t;

namespace tccl {

inline std::string EntityRef(const std::string& cstName, const ccl::lang::Morphology form = ccl::lang::Morphology{ ccl::lang::Grammem::nomn, ccl::lang::Grammem::sing }) {
  const ccl::lang::EntityRef ref{ cstName, form };
  return ref.ToString();
}

} // namespace tccl