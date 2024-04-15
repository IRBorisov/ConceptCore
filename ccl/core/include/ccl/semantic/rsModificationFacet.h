#pragma once

#include "ccl/cclMeta.hpp"
#include "ccl/Entity.hpp"

namespace ccl::semantic {

class RSForm;
class Thesaurus;

//! Constituent modification options
struct TrackingFlags {
  bool allowEdit{ false };

  bool term{ false };
  bool definition{ false };
  bool convention{ false };

  [[nodiscard]] bool operator==(const TrackingFlags& rhs) const noexcept {
    return allowEdit == rhs.allowEdit &&
      term == rhs.term &&
      definition == rhs.definition &&
      convention == rhs.convention;
  }
  [[nodiscard]] bool operator!=(const TrackingFlags& rhs) const noexcept { 
    return !(*this == rhs); 
  }
};

//! Modifications tracking facet of RSForm
class rsModificationFacet final : public meta::MutatorFacet<RSForm> {
  friend class RSForm;
  friend class Thesaurus;

  using Container = std::unordered_map<EntityUID, TrackingFlags>;
  Container cvs{};

public:
  explicit rsModificationFacet(RSForm& core)
    : MutatorFacet<RSForm>(core) {}

public:
  [[nodiscard]] bool IsTracking(EntityUID entity) const;
  [[nodiscard]] const TrackingFlags* operator()(EntityUID entity) const;
  [[nodiscard]] TrackingFlags* operator()(EntityUID entity);

  void Track(EntityUID entity, const TrackingFlags& modOpts = {});
  void StopTracking(EntityUID entity) noexcept;
  void ResetAll();

private:
  void LoadFrom(const rsModificationFacet& f2);
  void Clear() noexcept;
  void Erase(EntityUID target) noexcept;

  void OnTermChange(EntityUID entity);
  void OnDefenitionChange(EntityUID entity);
  void OnConventionChange(EntityUID entity);
};

} // namespace ccl::semantic
