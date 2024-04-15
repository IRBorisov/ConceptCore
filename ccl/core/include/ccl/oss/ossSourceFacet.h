#pragma once

#include "ccl/oss/Pict.hpp"
#include "ccl/Source.hpp"
#include "ccl/cclMeta.hpp"

namespace ccl::oss {

class OSSchema;
class ossOperationsFacet;

//! OSS source management facet
class ossSourceFacet final : public meta::MutatorFacet<OSSchema> {
  friend class OSSchema;
  friend class ossOperationsFacet;

public:
  std::u8string ossDomain{};

private:
  using Container = std::unordered_map<PictID, src::Handle>;
  Container sources{};
  bool disableImport{ false };

  static constexpr bool loadData = true;

public:
  ~ossSourceFacet() noexcept;
  ossSourceFacet(const ossSourceFacet&) = delete;
  ossSourceFacet& operator=(const ossSourceFacet&) = delete;

  explicit ossSourceFacet(OSSchema& oss);

public:
  [[nodiscard]] const src::Handle* operator()(PictID pid) const;
  [[nodiscard]] bool IsConnectedWith(const src::Source& src) const;
  [[nodiscard]] bool IsAssociatedWith(const src::Descriptor& localDesc) const;
  [[nodiscard]] std::optional<PictID> Src2PID(const src::Source& src) const;

  [[nodiscard]] src::Source* OpenSrc(PictID pid);
  [[nodiscard]] src::Source* ActiveSrc(PictID pid);

  bool Rename(PictID pid, const std::u8string& newLocalName);
  void Discard(PictID pid);

  bool ConnectPict2Src(PictID pid, src::Source& src);
  bool ConnectSrc2Pict(PictID pid, src::Source& src);

  [[nodiscard]] const src::DataStream* DataFor(PictID pid);

  void ReconnectAll();

private:
  void Erase(PictID pid);
  void CloseAll() noexcept;

  bool UpdateSync(PictID pid);
  bool InputData(PictID pid, meta::UniqueCPPtr<src::DataStream> data);

  bool Import(src::Source& src);
  void UpdateOnSrcChange(PictID pid);
  void Disconnect(src::Source& src);

  bool ForceConnection(PictID pid, src::Source& src, bool importFromSrc);
  void ConnectInternal(PictID pid, src::Source& src, bool importFromSrc);
  void SyncPict(PictID pid, bool importFromSrc);
  void UpdateHashes(PictID pid);

  void SyncData(PictID pid, src::Handle& handle, bool importFromSrc);

  static void EnableTracking(src::DataStream& data);
  [[nodiscard]] static std::optional<src::DataStream::ValueType>
    ReadAttribute(src::DataStream::Attribute attr, const src::DataStream& data) noexcept;
  static bool WriteAttribute(const src::DataStream::AttrValue& val, src::DataStream& data);

  [[nodiscard]] src::Descriptor GlobalDesc(const src::Descriptor& local) const;
};

} // namespace ccl::oss