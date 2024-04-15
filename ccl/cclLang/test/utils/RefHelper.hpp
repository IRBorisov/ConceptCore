#pragma once

#include "ccl/lang/Reference.h"

namespace tccl {

inline std::string EntityRef(const std::string& cstName, 
                             const ccl::lang::Morphology form = ccl::lang::Morphology{ ccl::lang::Grammem::nomn, ccl::lang::Grammem::sing }) {
  const ccl::lang::EntityRef ref{ cstName, form };
  return ref.ToString();
}

} // namespace tccl