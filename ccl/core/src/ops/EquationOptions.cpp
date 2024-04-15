#include "ccl/ops/EquationOptions.h"

namespace ccl::ops {

EquationOptions::EquationOptions(const EntityUID key, const EntityUID val, const Equation& params) {
  Insert(key, val, params);
}

bool EquationOptions::IsEqualTo(const Options& opt2) const {
  if (const auto* other = dynamic_cast<const EquationOptions*>(&opt2); other == nullptr) {
    return false;
  } else {
    return translation == other->translation && properties == other->properties;
  }
}

bool EquationOptions::SwapKeyVal(const EntityUID key) {
  if (!ContainsKey(key)) {
    return false;
  } else if (const auto val = translation(key); ContainsKey(translation(key))) {
    return false;
  } else {
    Equation props = properties[key];
    if (props.mode != Equation::Mode::createNew) {
      props.mode = props.mode == Equation::Mode::keepHier ? Equation::Mode::keepDel : Equation::Mode::keepHier;
    }
    Erase(key);
    Insert(val, key, props);
    return true;
  }
}

const Equation& EquationOptions::PropsFor(const EntityUID key) const {
  return properties.at(key);
}

bool EquationOptions::ContainsValue(const EntityUID value) const {
  return translation.ContainsValue(value);
}

bool EquationOptions::ContainsKey(const EntityUID key) const {
  return translation.ContainsKey(key);
}

void EquationOptions::Insert(const EntityUID key, const EntityUID val, const Equation& params) {
  translation.Insert(key, val);
  properties.insert({ key, params });
}

void EquationOptions::Erase(const EntityUID key) noexcept {
  translation.Erase(key); properties.erase(key);
}

void EquationOptions::SubstituteValues(const EntityTranslation& idSubstitutes) {
  translation.SubstituteValues(idSubstitutes);
}

} // namespace ccl::ops
