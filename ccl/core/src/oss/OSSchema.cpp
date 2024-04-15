#include "ccl/oss/OSSchema.h"

#include "ccl/env/cclEnvironment.h"
#include "ccl/semantic/RSForm.h"

namespace ccl::oss {

ccl::change::Usage OSSchema::Callback::QueryEntityIsUsed(const src::Source& from, const EntityUID entity) const {
  return oss.Ops().QueryEntityUsage(from, entity);
}

bool OSSchema::Callback::IsConnectedWith(const src::Source& src) const {
  return oss.Src().IsConnectedWith(src);
}

OSSchema::OSSchema() {
  callback = std::make_unique<Callback>(*this);
  grid = std::make_unique<ossGridFacet>(*this);
  graph = std::make_unique<ossGraphFacet>(*this);
  sources = std::make_unique<ossSourceFacet>(*this);
  ops = std::make_unique<ossOperationsFacet>(*this);

  Environment::Sources().AddCallback(*callback);
  Environment::Sources().AddObserver(*this);
}

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 26447 ) // Note: do not warn if default implementation might be noexcept
#endif

OSSchema::~OSSchema() noexcept {
  try {
    Environment::Sources().RemoveCallback(*callback);
    Environment::Sources().RemoveObserver(*this);
  }  catch (...) {
    // Note: nothing we can do about it
  }
}

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

void OSSchema::OnObserve(const types::Message& msg) {
  if (msg.Type() == types::Message::srcCode) {
    auto* source = Environment::Sources().Find(dynamic_cast<const SourceManager::SrcMessage*>(&msg)->srcID);
    if (dynamic_cast<const SourceManager::SrcOpened*>(&msg) != nullptr) {
      assert(source != nullptr);
      if (Src().Import(*source)) {
        Notify(SrcStatusMod{});
      }
    } else if (dynamic_cast<const SourceManager::SrcClosed*>(&msg) != nullptr) {
      if (source != nullptr) {
        Src().Disconnect(*source);
        Notify(SrcStatusMod{});
      }
    } else if (dynamic_cast<const SourceManager::SrcChanged*>(&msg) != nullptr) {
      assert(source != nullptr);
      if (const auto pid = Src().Src2PID(*source); pid.has_value()) {
        Src().UpdateOnSrcChange(pid.value());
        Notify(SrcStateMod{});
      }
    }
  }
}

ossGridFacet& OSSchema::Grid() noexcept { return *grid; }
const ossGridFacet& OSSchema::Grid() const noexcept { return *grid; }
ossGraphFacet& OSSchema::Graph() noexcept { return *graph; }
const ossGraphFacet& OSSchema::Graph() const noexcept { return *graph; }
ossSourceFacet& OSSchema::Src() noexcept { return *sources; }
const ossSourceFacet& OSSchema::Src() const noexcept { return *sources; }
ossOperationsFacet& OSSchema::Ops() noexcept { return *ops; }
const ossOperationsFacet& OSSchema::Ops() const noexcept { return *ops; }

OSSchema::size_type OSSchema::size() const noexcept {
  return std::size(storage);
}

bool OSSchema::empty() const noexcept {
  return std::empty(storage);
}

bool OSSchema::Contains(const PictID pict) const {
  return storage.contains(pict);
}

IteratorPict OSSchema::begin() const noexcept {
  return IteratorPict(std::begin(storage));
}

IteratorPict OSSchema::end() const noexcept {
  return IteratorPict(std::end(storage));
}

PictPtr OSSchema:: operator()(const PictID pid) const {
  return Contains(pid) ? &storage.at(pid) : nullptr;
}

void OSSchema::SetPictAlias(const PictID target, const std::string& newValue) {
  if (auto* pict = Access(target); pict != nullptr && pict->alias != newValue) {
    pict->alias = newValue;
    NotifyModification();
  }
}

void OSSchema::SetPictTitle(const PictID target, const std::string& newValue) {
  if (auto* pict = Access(target); pict != nullptr && pict->title != newValue) {
    pict->title = newValue;
    NotifyModification();
  }
}

void OSSchema::SetPictComment(const PictID target, const std::string& newValue) {
  if (auto* pict = Access(target); pict != nullptr && pict->comment != newValue) {
    pict->comment = newValue;
    NotifyModification();
  }
}

void OSSchema::SetPictLink(const PictID target, const MediaLink& lnk) {
  if (auto* pict = Access(target); pict != nullptr && pict->lnk != lnk) {
    pict->lnk = lnk;
    NotifyModification();
  }
}

const Pict& OSSchema::LoadPict(Pict&& pict, GridPosition pos, const src::Handle& handle, 
                           std::unique_ptr<OperationHandle> params) {
  if (Grid()(pos).has_value()) {
    pos = Grid().ClosestFreePos(pos);
  }
  if (idGen.IsTaken(pict.uid)) {
    pict.uid = idGen.NewUID();
  }
  const auto pid = pict.uid;
  InsertInternal(pict, pos, handle, std::move(params));
  return storage.at(pid);
}

PictPtr OSSchema::InsertOperation(const PictID operand1, const PictID operand2) {
  if (operand1 == operand2) {
    return nullptr;
  } else if (!Contains(operand1) || !Contains(operand2)) {
    return nullptr;
  } else {
    const auto newID = idGen.NewUID();
    graph->AddItem(newID, { operand1, operand2 });

    const auto newPos = Grid().ChildPosFor(operand1, operand2);
    InsertInternal(Pict{ newID },
                   newPos,
                   src::Handle{ src::SrcType::rsDoc },
                   std::make_unique<OperationHandle>());

    NotifyModification();
    return &storage.at(newID);
  }
}

PictPtr OSSchema::InsertBase() {
  const auto newPos = grid->ClosestFreePos();
  const auto newID = idGen.NewUID();
  InsertInternal(Pict{ newID }, newPos, src::Handle{ src::SrcType::rsDoc }, nullptr);

  NotifyModification();
  return &storage.at(newID);
}

void OSSchema::InsertInternal(const Pict& pict, GridPosition pos,
                              const src::Handle& srcHandle,
                              std::unique_ptr<OperationHandle> opHandle) {
  const auto pid = pict.uid;

  idGen.AddUID(pid);
  storage.emplace(pid, pict);
  Grid().SetPosFor(pid, pos);
  Src().sources.emplace(pid, srcHandle);
  if (opHandle != nullptr) {
    Ops().operations[pid] = std::move(opHandle);
  }
}

bool OSSchema::Erase(const PictID target) {
  if (!Contains(target)) {
    return false;
  } else if (!std::empty(graph->ChildrenOf(target))) {
    return false;
  } else {
    graph->Erase(target);
    grid->Erase(target);
    sources->Erase(target);
    ops->Erase(target);
    idGen.FreeUID(target);
    storage.erase(target);

    Notify(ErasePictMod{ target });
    return true;
  }
}

Pict* OSSchema::Access(const PictID pid) {
  return !storage.contains(pid) ? nullptr : &storage.at(pid);
}

void OSSchema::OnCoreChange(const PictID changedPict) {
  for (const auto& pict : Graph().ChildrenOf(changedPict)) {
    if (auto* operation = Ops().Access(pict);
        Src()(pict) != nullptr) {
      Ops().CheckOperation(pict);
      operation->outdated = true;
    }
  }
}

IteratorPict::IteratorPict(Container::const_iterator iter) noexcept
  :  iterator{ std::move(iter) } {}

IteratorPict& IteratorPict::operator++() noexcept {
  ++iterator;
  return *this;
}

IteratorPict::pointer IteratorPict::operator->() const noexcept {
  return &iterator->second;
}

IteratorPict::reference IteratorPict::operator*() const noexcept {
  return iterator->second;
}

bool IteratorPict::operator==(const IteratorPict& iter2) const noexcept {
  return iterator == iter2.iterator;
}

} // namespace ccl::oss