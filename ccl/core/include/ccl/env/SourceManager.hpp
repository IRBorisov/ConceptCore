#pragma once

#include "ccl/Source.hpp"

namespace ccl {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

//! Abstract source manager
class SourceManager : public src::CallbackManager, public types::Observable {
public:
  SourceManager() = default;
  virtual ~SourceManager() noexcept = default;

  SourceManager(const SourceManager&) = delete;
  SourceManager& operator=(const SourceManager&) = delete;

public:
  struct SrcMessage : types::Message {
    explicit SrcMessage(src::Descriptor srcID) noexcept 
      : srcID{ std::move(srcID) } {}

    [[nodiscard]] uint32_t Type() const noexcept override { return srcCode; }
    src::Descriptor srcID;
  };

  struct SrcOpened : SrcMessage { using SrcMessage::SrcMessage; };
  struct SrcClosed : SrcMessage { using SrcMessage::SrcMessage; };
  struct SrcChanged : SrcMessage { using SrcMessage::SrcMessage; };

public:
  void OnSourceOpen(const src::Source& src) { Notify(SrcOpened{ GetDescriptor(src) }); }
  void OnSourceClose(const src::Source& src) { Notify(SrcClosed{ GetDescriptor(src) }); }
  void OnSourceChange(const src::Source& src) { Notify(SrcChanged{ GetDescriptor(src) }); }

  [[nodiscard]] virtual bool TestDomain(const src::Descriptor& /*global*/, 
                                        const std::u8string& /*domain*/) const {
    return false;
  }
  [[nodiscard]] virtual src::Descriptor Convert2Local(const src::Descriptor& /*global*/, 
                                                      const std::u8string& /*domain*/) const {
    return src::Descriptor{};
  }
  [[nodiscard]] virtual src::Descriptor Convert2Global(const src::Descriptor& /*local*/, 
                                                       const std::u8string& /*domain*/) const {
    return src::Descriptor{};
  }
  [[nodiscard]] virtual src::Descriptor CreateLocalDesc(src::SrcType type, std::u8string localName) const {
    return src::Descriptor{ type, localName };
  }
  [[nodiscard]] virtual src::Source* Find(const src::Descriptor& /*desc*/) {
    return nullptr;
  }
  [[nodiscard]] virtual src::Descriptor GetDescriptor(const src::Source& /*src*/) const 
  { return src::Descriptor{};
  }
  virtual bool ChangeDescriptor(const src::Descriptor& /*desc*/, const src::Descriptor& /*newDesc*/) {
    return false;
  }
  [[nodiscard]] virtual src::Source* CreateNew(const src::Descriptor& /*desc*/) {
    return nullptr;
  }
  [[nodiscard]] virtual src::Source* Open(const src::Descriptor& /*desc*/) {
    return nullptr;
  }
  virtual void Close(src::Source& /*src*/) {}
  virtual bool SaveState(src::Source& /*src*/) {
    return false;
  }
  virtual void Discard(const src::Descriptor& /*desc*/) {}
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl