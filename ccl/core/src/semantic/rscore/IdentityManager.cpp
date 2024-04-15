#include "ccl/semantic/IdentityManager.h"

#include "ccl/semantic/RSCore.h"

namespace ccl::semantic {

void IdentityManager::Erase(const EntityUID target, const std::string& name) noexcept {
  idGenerator.FreeUID(target);
  aliasGenerator.FreeUID(name);
}

void IdentityManager::Clear() noexcept {
  idGenerator.Clear();
  aliasGenerator.Clear();
}

ConceptID IdentityManager::GenerateNewID(const CstType type) {
  return ConceptID{ idGenerator.NewUID(), aliasGenerator.NewNameFor(type) };
}

ConceptID IdentityManager::RegisterID(const EntityUID uid, const std::string& name, const CstType type) {
  ConceptID result{ uid, name };
  if (NeedNameChangeFor(name, type)) {
    result.alias = aliasGenerator.NewNameFor(type);
  } else {
    aliasGenerator.AddUID(name);
  }
  if (idGenerator.IsTaken(uid)) {
    result.uid = idGenerator.NewUID();
  } else {
    idGenerator.AddUID(uid);
  }
  return result;
}

ConceptID IdentityManager::RegisterEntity(const EntityUID uid, const CstType type) {
  ConceptID result{ uid };
  result.alias = aliasGenerator.NewNameFor(type);
  if (idGenerator.IsTaken(uid)) {
    result.uid = idGenerator.NewUID();
  } else {
    idGenerator.AddUID(uid);
  }
  return result;
}

bool IdentityManager::TryAlias(const std::string& oldValue, const std::string& newValue, const CstType type) {
  if (!aliasGenerator.IsTaken(oldValue) || NeedNameChangeFor(newValue, type)) {
    return false;
  } else {
    aliasGenerator.FreeUID(oldValue);
    aliasGenerator.AddUID(newValue);
    return true;
  }
}

bool IdentityManager::NeedNameChangeFor(const std::string& name, const CstType type) const {
  if (aliasGenerator.IsTaken(name)) {
    return true;
  } else if (const auto nameType = tools::CstNameGenerator::GetTypeForName(name); !nameType.has_value()) {
    return true;
  } else {
    return nameType.value() != type;
  }
}

} // namespace ccl::semantic
