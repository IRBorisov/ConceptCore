#pragma once

#include "ccl/semantic/RSCore.h"
#include "ccl/rslang/Typification.h"

namespace ccl::semantic {

template<typename Filter>
inline VectorOfEntities FilterCst(const RSCore& core,  Filter filter) {
  VectorOfEntities result{};
  for (const auto uid : core.List()) {
    if (filter(uid)) {
      result.push_back(uid);
    }
  }
  return result;
}

inline VectorOfEntities FilterCst(const RSCore& core, CstType type) {
  const auto filter = [&core, &type](const EntityUID uid) {
    return core.GetRS(uid).type == type;
  };
  return FilterCst(core, filter);
}

inline VectorOfEntities FilterCst(const RSCore& core, const rslang::ExpressionType& type) {
  const auto filter = [&core, &type](const EntityUID uid) {
    const auto& cstType = core.GetParse(uid).exprType;
    return cstType.has_value() && type == cstType.value();
  };
  return FilterCst(core, filter);
}

inline VectorOfEntities FilterCst(const RSCore& core, const std::string& txt) {
  const auto filter = [&core, &txt](const EntityUID uid) {
    return core.GetRS(uid).MatchStr(txt) || core.GetText(uid).MatchStr(txt);
  };
  return FilterCst(core, filter);
}

inline VectorOfEntities FilterEmpty(const RSCore& core) {
  const auto filter = [&core](const EntityUID uid) {
    return core.GetRS(uid).IsEmpty() && core.GetText(uid).IsEmpty() ? uid : std::optional<EntityUID>{};
  };
  return FilterCst(core, filter);
}

} // namespace ccl::semantic
