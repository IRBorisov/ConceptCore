#pragma once

#include "ccl/oss/OperationProcessor.hpp"

#include "ccl/oss/Pict.hpp"
#include "ccl/Source.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::oss {

class ossSourceFacet;
class OSSchema;

//! OSS operation handle
struct OperationHandle final {
  ops::Type type{ ops::Type::tba };
  meta::UniqueCPPtr<ops::Options> options{ nullptr };
  meta::UniqueCPPtr<ops::TranslationData> translations{ nullptr };

  bool broken{ false };
  bool outdated{ false };

  void Reset() noexcept;
  void InitOperation(ops::Type operation,
                     meta::UniqueCPPtr<ops::Options> newOptions) noexcept;
};

//! OSS operations facet
class ossOperationsFacet final : public meta::MutatorFacet<OSSchema> {
  friend class OSSchema;

public:
  std::string ossPath{};

private:
  using Operations = std::unordered_map<PictID, std::unique_ptr<OperationHandle>>;
  using Processors = std::vector<std::unique_ptr<OperationsProcessor>>;
  using ProcessorMapping = std::unordered_map<ops::Type, OperationsProcessor*>;

  Operations operations{};
  Processors procs;
  ProcessorMapping opProcs;

public:
  explicit ossOperationsFacet(OSSchema& oss);

public:
  [[nodiscard]] const OperationHandle* operator()(PictID pid) const;
  [[nodiscard]] ops::Status StatusOf(PictID pid) const;
  [[nodiscard]] bool HasOperation(PictID pid) const { return operator()(pid) != nullptr; }

  [[nodiscard]] std::unordered_set<ops::Type> VariantsFor(PictID pid) const;
  [[nodiscard]] bool IsExecutable(PictID pid);
  [[nodiscard]] bool IsTranslatable(PictID pid);

  bool InitFor(PictID pid, ops::Type operation,
               std::unique_ptr<ops::Options> newOptions = nullptr);
  bool Execute(PictID pid, bool autoDiscard = false);
  void ExecuteAll();

  [[nodiscard]] change::Usage QueryEntityUsage(const src::Source& src, EntityUID cstID) const;

private:
  OperationHandle* Access(PictID pid);
  [[nodiscard]] bool IsOperable(PictID pid) const;
  [[nodiscard]] bool CheckTranslations(PictID pid);
  void Erase(PictID pid) noexcept;
  bool CheckOperation(PictID pid);

  [[nodiscard]] ops::Signature SignaFor(PictID pid) const;
  [[nodiscard]] ops::Call CallFor(PictID pid) const;

  bool RunOperation(PictID pid, bool autoDiscard);
  bool PrepareParents(PictID pid);
  [[nodiscard]] ops::Result CreateNewResult(PictID pid);
  [[nodiscard]] std::optional<EntityTranslation> AggregateVersions(PictID pid, ops::Result& opResult) const;

  bool SaveOperationResult(PictID pid, const EntityTranslation& old2New, ops::Result&& opResult);

  [[nodiscard]] change::Usage CstUsage(EntityUID cstID, PictID pid) const;

  void UpdateChild(PictID child, size_t childIndex, const EntityTranslation& oldToNew);
  void UpdateTranslations(PictID child, size_t childIndex, const EntityTranslation& oldToNew);
  void UpdateOptions(PictID child, size_t childIndex, const EntityTranslation& oldToNew);
};

} // namespace ccl::oss