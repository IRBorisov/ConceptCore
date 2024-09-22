#pragma once

#include "ccl/tools/EntityGenerator.h"

#include "ossGraphFacet.h"
#include "ossGridFacet.h"
#include "ossOperationsFacet.h"
#include "ossSourceFacet.h"

namespace ccl::oss {

class IteratorPict;
struct GridPosition;
struct OperationHandle;

//! Operational synthesis schema
class OSSchema final : public change::ObservableMods, public types::Observer {
  friend class IteratorPict;
  friend class ossGridFacet;
  friend class ossSourceFacet;
  friend class ossOperationsFacet;

public:
  std::string title{};
  std::string comment{};

private:
  using Storage = std::unordered_map<PictID, Pict>;
  Storage storage{};
  tools::EntityGenerator idGen{};

  std::unique_ptr<ossGridFacet> grid;
  std::unique_ptr<ossGraphFacet> graph;
  std::unique_ptr<ossSourceFacet> sources;
  std::unique_ptr<ossOperationsFacet> ops;

  class Callback;
  std::unique_ptr<Callback> callback{};

public:
  ~OSSchema() noexcept override;
  OSSchema();
  OSSchema(const OSSchema&) = delete;
  OSSchema& operator=(const OSSchema&) = delete;

public:
  struct ErasePictMod : change::ModMessage {
    PictID pid;
    explicit ErasePictMod(const PictID pid) noexcept
      : pid{ pid } {}
  };
  struct SrcStatusMod : change::ModMessage {};
  struct SrcStateMod : change::ModMessage {};

public:
  using size_type = Storage::size_type;

  void OnObserve(const types::Message& msg) override;

  [[nodiscard]] ossGridFacet& Grid() noexcept;
  [[nodiscard]] const ossGridFacet& Grid() const noexcept;
  [[nodiscard]] ossGraphFacet& Graph() noexcept;
  [[nodiscard]] const ossGraphFacet& Graph() const noexcept;
  [[nodiscard]] ossSourceFacet& Src() noexcept;
  [[nodiscard]] const ossSourceFacet& Src() const noexcept;
  [[nodiscard]] ossOperationsFacet& Ops() noexcept;
  [[nodiscard]] const ossOperationsFacet& Ops() const noexcept;

  [[nodiscard]] size_type size() const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] bool Contains(PictID pict) const;

  [[nodiscard]] IteratorPict begin() const noexcept;
  [[nodiscard]] IteratorPict end() const noexcept;

  [[nodiscard]] PictPtr operator()(PictID pid) const;

  PictPtr InsertBase();
  PictPtr InsertOperation(PictID operand1, PictID operand2);

  bool Erase(PictID target);

  void SetPictTitle(PictID target, const std::string& newValue);
  void SetPictAlias(PictID target, const std::string& newValue);
  void SetPictComment(PictID target, const std::string& newValue);
  void SetPictLink(PictID target, const MediaLink& lnk);

  const Pict& LoadPict(Pict&& pict, GridPosition pos, const src::Handle& handle, std::unique_ptr<OperationHandle> params);

private:
  Pict* Access(PictID pid);
  void InsertInternal(Pict&& pict, GridPosition pos,
                      const src::Handle& srcHandle,
                      std::unique_ptr<OperationHandle> opHandle);

  void OnCoreChange(PictID changedPict);

  //! OSS callback
  class Callback : public src::SourceCallback {
    OSSchema& oss;

  public:
    explicit Callback(OSSchema& oss) noexcept
      : oss{ oss } {}

  public:
    [[nodiscard]] change::Usage QueryEntityIsUsed(const src::Source& from, EntityUID entity) const override;
    [[nodiscard]] bool IsConnectedWith(const src::Source& src) const override;
  };
};

//! OSS iterator
class IteratorPict {
  friend class OSSchema;
  using Container = OSSchema::Storage;
  Container::const_iterator iterator;

private:
  explicit IteratorPict(Container::const_iterator iter) noexcept;

public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = Pict;
  using difference_type = std::ptrdiff_t;
  using pointer = const Pict*;
  using reference = const Pict&;

  [[nodiscard]] bool operator==(const IteratorPict& iter2) const noexcept;
  [[nodiscard]] bool operator!=(const IteratorPict& iter2) const noexcept { return !(*this == iter2); }

  IteratorPict& operator++() noexcept;

  [[nodiscard]] pointer operator->() const noexcept;
  [[nodiscard]] reference operator*() const noexcept;
};

} // namespace ccl::oss