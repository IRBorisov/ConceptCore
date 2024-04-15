#pragma once

#include "ccl/semantic/RSForm.h"
#include "ccl/oss/OSSchema.h"
#include "ccl/env/cclEnvironment.h"

#include <list>
#include <memory>

class FakeTRS : public ccl::src::Source, public ccl::types::Observer {
  using SrcType = ccl::src::SrcType;
  using DataStream = ccl::src::DataStream;
  using RSForm = ccl::semantic::RSForm;

public:
  RSForm schema{};
  std::u8string fullName{};
  bool unsavable{ false };
  bool unwritable{ false };

private:
  bool saved{ true };
  bool open{ true };

public:
  ~FakeTRS() {
    schema.RemoveObserver(*this);
  }
  FakeTRS() {
    schema.AddObserver(*this);
  }
  FakeTRS(const FakeTRS&) = delete;
  FakeTRS& operator=(const FakeTRS&) = delete;

  FakeTRS(FakeTRS&& predecessor) noexcept {
    schema = predecessor.schema;
    fullName = predecessor.fullName;
    open = predecessor.open;
    saved = predecessor.saved;
    unsavable = predecessor.unsavable;

    schema.RemoveObserver(predecessor);
    schema.AddObserver(*this);
  }

  FakeTRS& operator=(FakeTRS&& predecessor) noexcept {
    schema = predecessor.schema;
    fullName = predecessor.fullName;
    open = predecessor.open;
    saved = predecessor.saved;
    unsavable = predecessor.unsavable;

    schema.RemoveObserver(predecessor);
    schema.AddObserver(*this);
    return *this;
  }

public:
  [[nodiscard]] bool IsOpened() const { return open; }

  void OpenNoTrigger() { open = true; }

  void TriggerOpen() {
    open = true;
    ccl::Environment::Sources().OnSourceOpen(*this);
  };

  void TriggerClose() { 
    ccl::Environment::Sources().OnSourceClose(*this); 
    open = false; 
    saved = true; 
  };

  void TriggerSave() {
    if (!saved) {
      ccl::Environment::Sources().OnSourceChange(*this);
      saved = true;
    }
  };

  void OnObserve(const ccl::types::Message& /*msg*/) override {
    saved = false;
  }

  [[nodiscard]] ccl::change::Hash CoreHash() const override { return schema.CoreHash(); }
  [[nodiscard]] ccl::change::Hash FullHash() const override { return schema.FullHash(); }
  [[nodiscard]] bool WriteData(ccl::meta::UniqueCPPtr<DataStream> data) override {
    if (unwritable) {
      return false;
    } else {
      const auto* rsData = dynamic_cast<const RSForm*>(data.get());
      if (rsData == nullptr) {
        return false;
      } else {
        schema = *rsData;
        return true;
      }
    }
  }
  [[nodiscard]] const DataStream* ReadData() const override { return &schema; }
  [[nodiscard]] DataStream* AccessData() override { return &schema; }
  [[nodiscard]] SrcType Type() const noexcept override { return SrcType::rsDoc; }
};

class FakeSourceManager final : public ccl::SourceManager {
  using Container = std::list<FakeTRS>;

  using SrcType = ccl::src::SrcType;
  using Source = ccl::src::Source;
  using Descriptor = ccl::src::Descriptor;

public:
  bool rejectDomain{ false };

private:
  Container sources{};

public:
  const FakeTRS& DummyCast(const Source& src) const { return dynamic_cast<const FakeTRS&>(src); }
  FakeTRS& DummyCast(Source& src) { return dynamic_cast<FakeTRS&>(src); }

  void DestroySource(FakeTRS& src) {
    for (auto it = std::begin(sources); it != std::end(sources); ++it) {
      if (&*it == &src) {
        if (src.IsOpened()) {
          SourceManager::OnSourceClose(src);
        }
        sources.erase(it);
        return;
      }
    }
  }

  void ReplaceSourceData(FakeTRS& src, const ccl::semantic::RSForm& newSchema) {
    for (auto it = std::begin(sources); it != std::end(sources); ++it) {
      if (&*it == &src) {
        if (src.IsOpened()) {
          SourceManager::OnSourceClose(src);
        }
        it->schema = newSchema;
        return;
      }
    }
  }

  FakeTRS& CreateNewRS() {
    static auto uid = 0;
    ++uid;
    auto& result = DummyCast(*CreateNew(Descriptor{ SrcType::rsDoc, ccl::to_u8string(uid) }));
    result.fullName = ccl::to_u8string(uid) + u8".trs";
    return result;
  }

public:
  [[nodiscard]] bool TestDomain(const Descriptor& global, const std::u8string& domain) const override {
    return !rejectDomain && 
      (std::empty(domain) || global.name.find(domain) == 0);
  }

  [[nodiscard]] Descriptor Convert2Local(const Descriptor& global, const std::u8string& domain) const override {
    auto local = global;
    if (!std::empty(domain)) {
      local.name.erase(0, domain.length());
    }
    return local;
  }

  [[nodiscard]] Descriptor Convert2Global(const Descriptor& local, const std::u8string& domain) const override {
    return Descriptor{ local.type, domain + local.name };
  }


  [[nodiscard]] Source* Find(const Descriptor& desc) override {
    if (desc.type != SrcType::rsDoc) {
      return SourceManager::Find(desc);
    } else {
      for (auto& src : sources) {
        if (src.fullName == desc.name && src.IsOpened()) {
          return &src;
        }
      }
      return nullptr;
    }
  }

  [[nodiscard]] Descriptor CreateLocalDesc(SrcType type, std::u8string localName) const override {
    if (type != SrcType::rsDoc) {
      return SourceManager::CreateLocalDesc(type, localName);
    } else {
      if (std::empty(localName)) {
        static auto i = 0;
        localName = u8"local" + ccl::to_u8string(++i); // Note: making new local name always different
      }
      localName += u8".trs";
      return Descriptor{ type, localName };
    }
  }

  [[nodiscard]] Descriptor GetDescriptor(const Source& src) const override {
    if (const auto* srcPtr = dynamic_cast<const FakeTRS*>(&src); srcPtr != nullptr) {
      return Descriptor{ SrcType::rsDoc, srcPtr->fullName };
    } else {
      return Descriptor{};
    }
  }

  [[nodiscard]] Source* CreateNew(const Descriptor& desc) override {
    if (Find(desc) != nullptr) {
      return nullptr;
    } else if (desc.type != SrcType::rsDoc) {
      return SourceManager::CreateNew(desc);
    } else {
      sources.emplace_back(FakeTRS{});
      sources.back().fullName = desc.name;
      return &sources.back();
    }
  }

  [[nodiscard]] Source* Open(const Descriptor& desc) override {
    if (desc.type == SrcType::rsDoc) {
      for (auto& src : sources) {
        if (src.fullName == desc.name) {
          DummyCast(src).TriggerOpen();
          return &src;
        }
      }
    }
    return nullptr;
  }

  void Close(Source& src) override {
    SourceManager::OnSourceChange(src);
    SourceManager::OnSourceClose(src);
    DummyCast(src).TriggerSave();
    DummyCast(src).TriggerClose();
  }

  [[nodiscard]] bool ChangeDescriptor(const Descriptor& desc, const Descriptor& newDesc) override {
    if (desc.type != newDesc.type ||
        desc.type != SrcType::rsDoc) {
      return false;
    } else if (auto* targetSrc = Find(desc); targetSrc == nullptr || Find(newDesc) != nullptr) {
      return false;
    } else {
      for (const auto& src : sources) {
        if (src.fullName == newDesc.name) {
          return false;
        }
      }
      DummyCast(*targetSrc).fullName = newDesc.name;
      return true;
    }
  }

  [[nodiscard]] bool SaveState(Source& src) override {
    if (DummyCast(src).IsOpened() && !DummyCast(src).unsavable) {
      DummyCast(src).TriggerSave();
      return true;
    } else {
      return false;
    }
  }

  void Discard(const Descriptor& desc) override {
    if (auto* src = Open(desc); src != nullptr) {
      src->ReleaseClaim();
      Close(*src);
    }
  }
};