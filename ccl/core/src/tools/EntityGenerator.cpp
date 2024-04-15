#include "ccl/tools/EntityGenerator.h"

#include "ccl/env/cclEnvironment.h"

namespace ccl::tools {

void EntityGenerator::Clear() noexcept {
  entities.clear();
}

EntityUID EntityGenerator::NewUID() {
  EntityUID result{ 0 };
  const auto oldSize = ssize(entities);
  while (ssize(entities) == oldSize) {
    result = static_cast<EntityUID>(distribution(Environment::RNG()));
    entities.emplace(result);
  }
  return result;
}

void EntityGenerator::AddUID(const EntityUID& newUID) {
  entities.emplace(newUID);
}

void EntityGenerator::FreeUID(const EntityUID& returnUID) noexcept {
  entities.erase(returnUID);
}

bool EntityGenerator::IsTaken(const EntityUID& uid) const {
  return entities.contains(uid);
}

} // namespace ccl::tools