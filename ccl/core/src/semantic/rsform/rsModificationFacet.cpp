#include "ccl/semantic/rsModificationFacet.h"

#include "ccl/semantic/RSForm.h"

namespace ccl::semantic {

void rsModificationFacet::LoadFrom(const rsModificationFacet& f2) {
  cvs = f2.cvs;
}

void rsModificationFacet::Clear() noexcept {
  cvs.clear();
}

void rsModificationFacet::Erase(const EntityUID target) noexcept {
  cvs.erase(target);
}

bool rsModificationFacet::IsTracking(const EntityUID entity) const {
  return cvs.contains(entity);
}

void rsModificationFacet::Track(const EntityUID entity, const TrackingFlags& modOpts) {
  if (core.Contains(entity)) {
    cvs[entity] = modOpts;
  }
}

void rsModificationFacet::StopTracking(const EntityUID entity) noexcept {
  cvs.erase(entity);
}

const TrackingFlags* rsModificationFacet::operator()(const EntityUID entity) const {
  return IsTracking(entity) ? &cvs.at(entity) : nullptr;
}

TrackingFlags* rsModificationFacet::operator()(const EntityUID entity) {
  return IsTracking(entity) ? &cvs[entity] : nullptr;
}

void rsModificationFacet::ResetAll() {
  Clear();
  core.NotifyModification();
}

void rsModificationFacet::OnTermChange(const EntityUID entity) {
  if (IsTracking(entity)) {
    cvs.at(entity).term = true;
  }
}

void rsModificationFacet::OnDefenitionChange(const EntityUID entity) {
  if (IsTracking(entity)) {
    cvs.at(entity).definition = true;
  }
}

void rsModificationFacet::OnConventionChange(const EntityUID entity) {
  if (IsTracking(entity)) {
    cvs.at(entity).convention = true;
  }
}

} // namespace ccl::semantic
