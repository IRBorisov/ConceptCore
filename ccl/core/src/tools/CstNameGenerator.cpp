#include "ccl/tools/CstNameGenerator.h"

namespace ccl::tools {

namespace {

[[nodiscard]] constexpr char FirstLetterOf(const semantic::CstType type) noexcept {
  using semantic::CstType;
  switch (type) {
  default:
  case CstType::base: return 'X';
  case CstType::constant: return 'C';
  case CstType::structured: return 'S';
  case CstType::axiom: return 'A';
  case CstType::term: return 'D';
  case CstType::function: return 'F';
  case CstType::theorem: return 'T';
  case CstType::predicate: return 'P';
  }
}

[[nodiscard]] std::string CstNameFrom(CstNameGenerator::NameElements params) {
  return FirstLetterOf(params.type) + std::to_string(params.index);
}

} // namespace

void CstNameGenerator::Clear() noexcept {
  names.clear();
}

std::string CstNameGenerator::NewUID() {
  return NewNameFor(semantic::CstType::base);
}

void CstNameGenerator::AddUID(const std::string& newUID) {
  names.emplace(newUID);
}

void CstNameGenerator::FreeUID(const std::string& returnUID) noexcept {
  names.erase(returnUID);
}

bool CstNameGenerator::IsTaken(const std::string& name) const {
  return names.contains(name);
}

std::string CstNameGenerator::NewNameFor(const semantic::CstType type) {
  static constexpr auto startIndex = 1U;
  NameElements params{type, startIndex };
  while (IsTaken(CstNameFrom(params))) {
    ++params.index;
  }
  auto result = CstNameFrom(params);
  AddUID(result);
  return result;
}

#ifdef _MSC_VER
#pragma warning( push )
  #pragma warning( disable : 26446 ) // Note: do not warn about indexes
#endif

bool CstNameGenerator::IsNameCorrect(const std::string& name) noexcept {
  if (size(name) < 2) {
    return false;
  }
  switch (name[0]) {
  default:
    return false;
  case 'X': case 'C': case 'S': case 'A':
  case 'D': case 'T': case 'F': case 'P':
    break;
  }
  for (auto i = 1U; i < size(name); ++i) {
    if (iswdigit(static_cast<wint_t>(name[i])) == 0) {
      return false;
    }
  }
  return true;
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

std::optional<semantic::CstType> CstNameGenerator::GetTypeForName(const std::string& name) {
  using semantic::CstType;
  if (!IsNameCorrect(name)) {
    return std::nullopt;
  } else {
    switch (name.at(0)) {
    default:
    case 'X': return CstType::base;
    case 'C': return CstType::constant;
    case 'S': return CstType::structured;
    case 'A': return CstType::axiom;
    case 'D': return CstType::term;
    case 'F': return CstType::function;
    case 'T': return CstType::theorem;
    case 'P': return CstType::predicate;
    }
  }
}

} // namespace ccl::tools