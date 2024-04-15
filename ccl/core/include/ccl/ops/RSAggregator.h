#pragma once

#include "ccl/semantic/RSForm.h"

namespace ccl::ops {

//! Aggregation manager for versions of RSForm object
class RSAggregator {
  semantic::RSForm& output;
  const semantic::RSForm* prevOutput{ nullptr };

  EntityTranslation translation{};
  StrSubstitutes nameSubstitutes{};
  VectorOfEntities insertedCsts{};

public:
  explicit RSAggregator(semantic::RSForm& newShema)
    : output{ newShema } {}

public:
  std::optional<EntityTranslation> Merge(const semantic::RSForm& previous, const EntityTranslation& oldToNew);

private:
  void Clear() noexcept;
  bool PrepareSubstitutes();
  void TransferNew();
  void TransferIheritedData();
  void UpdateReferences();

  [[nodiscard]] std::optional<EntityUID> TransferCst(EntityUID target, semantic::ListIterator insertWhere);
};

} // namespace ccl::ops