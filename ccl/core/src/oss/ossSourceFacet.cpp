#include "ccl/oss/ossSourceFacet.h"

#include "ccl/oss/OSSchema.h"
#include "ccl/semantic/RSForm.h"
#include "ccl/env/cclEnvironment.h"

namespace ccl::oss {

ossSourceFacet::ossSourceFacet(OSSchema& oss) 
  : MutatorFacet<OSSchema>(oss) {}

ossSourceFacet::~ossSourceFacet() noexcept {
  CloseAll();
}

bool ossSourceFacet::IsConnectedWith(const src::Source& src) const {
  return Src2PID(src).has_value();
}

bool ossSourceFacet::IsAssociatedWith(const src::Descriptor& localDesc) const {
  return std::any_of(begin(sources), end(sources),
    [&](const auto& srcPair) noexcept { return srcPair.second.desc == localDesc; });
}

std::optional<PictID> ossSourceFacet::Src2PID(const src::Source& src) const {
  for (const auto& [pict, handle] : sources) {
    if (handle.src == &src) {
      return pict;
    }
  }
  return std::nullopt;
}

bool ossSourceFacet::Rename(const PictID pid, const std::u8string& newLocalName) {
  if (std::empty(newLocalName)) {
    return false;
  } else if (!core.Contains(pid)) {
    return false;
  } else if (auto& handle = sources.at(pid);
             handle.desc.name == newLocalName) {
    return false;
  } else if(std::empty(handle)) {
    return false;
  } else {
    src::Descriptor newDescriptor = handle.desc;
    newDescriptor.name = newLocalName;
    if (!IsAssociatedWith(newDescriptor) &&
        Environment::Sources().ChangeDescriptor(GlobalDesc(handle.desc), GlobalDesc(newDescriptor))) {
      handle.desc = newDescriptor;
      core.NotifyModification();
      return true;
    } else {
      return false;
    }
  }
}

void ossSourceFacet::ReconnectAll() {
  for (const auto& [pict, handle] : sources) {
    auto* newSrc = Environment::Sources().Find(GlobalDesc(handle.desc));
    auto* oldSrc = handle.src;
    if (newSrc == oldSrc) {
      if (oldSrc != nullptr) {
        Environment::Sources().SaveState(*oldSrc);
      }
    } else {
      if (oldSrc != nullptr) {
        Environment::Sources().Close(*oldSrc);
      }
      if (newSrc != nullptr) {
        ConnectInternal(pict, *newSrc, loadData);
      }
    }
  }
  core.NotifyModification();
}

void ossSourceFacet::UpdateOnSrcChange(const PictID pid) {
  SyncPict(pid, loadData);
}

bool ossSourceFacet::ConnectPict2Src(const PictID pid, src::Source& src) {
  if (!ForceConnection(pid, src, loadData)) {
    return false;
  } else {
    core.NotifyModification();
    return true;
  }
}

bool ossSourceFacet::ConnectSrc2Pict(const PictID pid, src::Source& src) {
  if (!ForceConnection(pid, src, !loadData)) {
    return false;
  } else {
    core.NotifyModification();
    return true;
  }
}

bool ossSourceFacet::ForceConnection(const PictID pid, src::Source& src, const bool importFromSrc) {
  if (!core.Contains(pid)) {
    return false;
  } else if (auto* oldSrc = sources.at(pid).src; oldSrc == &src) {
    return UpdateSync(pid);
  } else if (IsConnectedWith(src)) {
    return false;
  } else if (!Environment::Sources().SaveState(src)) {
    return false;
  } else {
    if (oldSrc != nullptr) {
      Environment::Sources().Close(*oldSrc);
    }
    ConnectInternal(pid, src, importFromSrc);
    return true;
  }
}

bool ossSourceFacet::Import(src::Source& src) {
  if (disableImport) {
    return false;
  } else if (const auto globalDesc = Environment::Sources().GetDescriptor(src);
             !Environment::Sources().TestDomain(globalDesc, ossDomain)) {
    return false;
  } else {
    const auto localDesc = Environment::Sources().Convert2Local(globalDesc, ossDomain);
    for (const auto& pict : core) {
      if (!std::empty(sources[pict.uid]) &&
          sources[pict.uid].src == nullptr  &&
          sources[pict.uid].desc == localDesc) {
        ConnectInternal(pict.uid, src, loadData);
        return true;
      }
    }
    return false;
  }
}

void ossSourceFacet::Disconnect(src::Source& src) {
  if (const auto maybePid = Src2PID(src); maybePid.has_value()) {
    sources[maybePid.value()].src = nullptr;
    src.ReleaseClaim();
  }
}

bool ossSourceFacet::UpdateSync(const PictID pid) {
  if (auto* src = sources.at(pid).src; src != nullptr) {
    return Environment::Sources().SaveState(*src);
  } else {
    return true;
  }
}

void ossSourceFacet::ConnectInternal(const PictID pid, src::Source& src, bool importFromSrc) {
  if (importFromSrc) {
    Environment::Sources().SaveState(src);
  }
  sources.at(pid).src = &src;
  src.Claim();
  SyncPict(pid, importFromSrc);
}

void ossSourceFacet::SyncPict(const PictID pid, bool importFromSrc) {
  UpdateHashes(pid);
  SyncData(pid, sources[pid], importFromSrc);
}

void ossSourceFacet::EnableTracking(src::DataStream& data) {
  auto* schema = dynamic_cast<semantic::RSForm*>(&data);
  for (const auto uid : schema->Core()) {
    schema->Mods().Track(uid);
  }
}

void ossSourceFacet::UpdateHashes(const PictID pid) {
  const auto oldMain = sources[pid].coreHash;
  sources[pid].UpdateHashes();
  if (oldMain != sources[pid].coreHash && !core.DndStatus()) {
    core.OnCoreChange(pid);
  }
}

void ossSourceFacet::SyncData(const PictID pid, src::Handle& handle, bool importFromSrc) {
  auto* pict = core.Access(pid);
  const auto descriptor = Environment::Sources().GetDescriptor(*handle.src);
  sources[pid].desc = Environment::Sources().Convert2Local(descriptor, ossDomain);
  const auto* data = handle.src->ReadData();
  assert(data != nullptr);

  const auto alias = ReadAttribute(src::DataStream::Attribute::alias, *data).value_or("");
  const auto title = ReadAttribute(src::DataStream::Attribute::title, *data).value_or("");
  const auto comment = ReadAttribute(src::DataStream::Attribute::comment, *data).value_or("");
  if (alias != pict->alias || title != pict->title ||  comment != pict->comment) {
    if (importFromSrc) {
      pict->alias = alias;
      pict->title = title;
      pict->comment = comment;
    } else {
      auto* outputData = handle.src->AccessData();
      assert(outputData != nullptr);
      WriteAttribute({ src::DataStream::Attribute::alias, pict->alias }, *outputData);
      WriteAttribute({ src::DataStream::Attribute::title, pict->title }, *outputData);
      WriteAttribute({ src::DataStream::Attribute::comment, pict->comment }, *outputData);
    }
  }
}

std::optional<src::DataStream::ValueType> 
ossSourceFacet::ReadAttribute(src::DataStream::Attribute attr, const src::DataStream& data) noexcept {
  const auto* schema = src::CastData<semantic::RSForm>(&data);
  if (schema == nullptr) {
    return {};
  }

  switch (attr) {
  default:
  case src::DataStream::Attribute::alias: return schema->alias;
  case src::DataStream::Attribute::title: return schema->title;
  case src::DataStream::Attribute::comment: return schema->comment;
  }
}

bool ossSourceFacet::WriteAttribute(const src::DataStream::AttrValue& val, src::DataStream& data) {
  auto* schema = dynamic_cast<semantic::RSForm*>(&data);
  if (schema == nullptr) {
    return false;
  }

  switch (val.type) {
  default:
  case src::DataStream::Attribute::alias: schema->alias = val.value; return true;
  case src::DataStream::Attribute::title: schema->title = val.value; return true;
  case src::DataStream::Attribute::comment: schema->comment = val.value; return true;
  }
}

void ossSourceFacet::Erase(const PictID pid) {
  Discard(pid);
  sources.erase(pid);
}

src::Descriptor ossSourceFacet::GlobalDesc(const src::Descriptor& local) const {
  return Environment::Sources().Convert2Global(local, ossDomain);
}

void ossSourceFacet::Discard(const PictID pid) {
  if (core.Contains(pid)) {
    auto& handle = sources.at(pid);
    if (auto* src = handle.src; src != nullptr) {
      Environment::Sources().SaveState(*src);
    }

    if (!std::empty(handle)) {
      auto globalDesc = GlobalDesc(handle.desc);
      handle.DiscardSrc();
      Environment::Sources().Discard(globalDesc);
    }
  }
}

const src::Handle* ossSourceFacet::operator()(const PictID pid) const {
  if (!sources.contains(pid)) {
    return nullptr;
  } else {
    return &sources.at(pid);
  }
}

src::Source* ossSourceFacet::ActiveSrc(const PictID pid) {
  if (!sources.contains(pid)) {
    return nullptr;
  } else {
    return sources.at(pid).src;
  }
}

src::Source* ossSourceFacet::OpenSrc(const PictID pid) {
  if (!core.Contains(pid)) {
    return nullptr;
  } else if (std::empty(sources.at(pid))) {
    return nullptr;
  } else {
    auto* src = sources.at(pid).src;
    if (src == nullptr) {
      disableImport = true;
      src = Environment::Sources().Open(GlobalDesc(sources.at(pid).desc));
      disableImport = false;

      if (src != nullptr) {
        ConnectInternal(pid, *src, !loadData);
      }
    }
    return src;
  }
}

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26447 ) // Note: function is declared noexcept and ignores internal exceptions
#endif

void ossSourceFacet::CloseAll() noexcept {
  try {
    for (const auto& it : sources) {
      if (it.second.src != nullptr) {
        Environment::Sources().Close(*it.second.src);
      }
    }
  }
  catch (...) {
    // Note: nothing we can do about exceptions on source close
  }
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

bool ossSourceFacet::InputData(const PictID pid, meta::UniqueCPPtr<src::DataStream> data) {
  auto& handle = sources.at(pid);
  auto* src = handle.src;
  if (src == nullptr) {
    handle.DiscardSrc();
    const auto description = Environment::Sources().CreateLocalDesc(src::SrcType::rsDoc, to_u8string(core(pid)->alias));
    src = Environment::Sources().CreateNew(GlobalDesc(description));
    if (src == nullptr) {
      return false;
    } else {
      handle.desc = Environment::Sources().Convert2Local(Environment::Sources().GetDescriptor(*src), ossDomain);
    }
  }

  if (!src->WriteData(std::move(data))) {
    return false;
  } else {
    ConnectInternal(pid, *src, loadData);
    return true;
  }
}

const src::DataStream* ossSourceFacet::DataFor(const PictID pid) {
  if (const auto* src = core.Src().OpenSrc(pid); src != nullptr) {
    return src->ReadData();
  } else {
    return nullptr;
  }
}

} // namespace ccl::oss