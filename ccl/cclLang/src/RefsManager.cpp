#include "ccl/lang/RefsManager.h"

#include "ccl/lang/LexicalTerm.h"

namespace ccl::lang {

namespace {

void ShiftAllAfter(std::vector<Reference>& refs, std::vector<Reference>::iterator pos, StrPos shift) {
  if (pos != end(refs)) {
    for (auto shiftIt = next(pos); shiftIt != end(refs); ++shiftIt) {
      shiftIt->position.Shift(shift);
    }
  }
}

} // namespace

const Reference* RefsManager::FirstIn(const StrRange range) const {
  for (auto it = begin(refs); it != end(refs); ++it) {
    if (range.IsAfter(it->position)) {
      continue;
    } else if (range.IsBefore(it->position)) {
      return nullptr;
    } else {
      return &*it;
    }
  }
  return nullptr;
}

RefsManager::RefsManager(const EntityTermContext& cntxt) noexcept :
  context{ &cntxt } {}

void RefsManager::SetContext(const EntityTermContext& cntxt) noexcept {
  context = &cntxt;
}

std::string RefsManager::Resolve(std::string_view text) {
  refs = Reference::ExtractAll(text);
  ResolveAll();
  return GenerateResolved(text);
}

void RefsManager::ResolveAll() {
  for (auto it = begin(refs); it != end(refs); ++it) {
    if (it->IsEntity()) {
      it->ResolveEntity(*context);
    }
  }
  for (auto it = begin(refs); it != end(refs); ++it) {
    if (it->IsCollaboration()) {
      const auto* master = FindMaster(it, it->GetOffset());
      it->ResolveCollaboration(master);
    }
  }
}

std::string RefsManager::GenerateResolved(std::string_view text) {
  std::string resolvedString{};
  StrPos difLen = 0;
  auto current = UTF8Begin(text);
  for (auto it = begin(refs); it != end(refs); ++it) {
    if (current.Position() != it->position.start) {
      resolvedString += text.substr(current.BytePosition(),
        UTF8Iterator(text, it->position.start).BytePosition() - current.BytePosition());
    }
    current = UTF8Iterator(text, it->position.finish);
    const auto resolvedRefText = it->resolvedText;
    resolvedString += resolvedRefText;
    const auto unresolvedLength = it->position.length();
    const auto resolvedLength = SizeInCodePoints(resolvedRefText);
    it->position = StrRange::FromLength(it->position.start + difLen, resolvedLength);
    difLen += resolvedLength - unresolvedLength;
  }
  if (current != UTF8End(text)) {
    resolvedString += text.substr(current.BytePosition(), size(text) - current.BytePosition());
  }
  return resolvedString;
}

const Reference* RefsManager::Insert(Reference newRef, const StrPos insWhere) {
  assert(context != nullptr);
  auto it = std::find_if(begin(refs), end(refs),
                         [&](const Reference& hold)
                         { return hold.position.start >= insWhere; });
  if (it != end(refs) && 
    (it->position.Contains(insWhere) ||  it->position.finish == insWhere)) {
    return nullptr;
  } else if (it != begin(refs) && !empty(refs) &&
    (prev(it)->position.Contains(insWhere) ||  prev(it)->position.finish == insWhere)) {
    return nullptr;
  }

  it = refs.emplace(it, std::move(newRef));
  ResolveIt(it);
  const auto resultLen = SizeInCodePoints(it->resolvedText);
  it->position = StrRange::FromLength(insWhere, resultLen);
  ShiftAllAfter(refs, it, resultLen);
  return &*it;
}

std::optional<StrRange> RefsManager::EraseIn(StrRange range, const bool expandRefs) {
  bool checkFinish = false;

  auto eraseStart = end(refs);
  auto refIt = begin(refs);
  
  for (; refIt != end(refs); ++refIt) {
    const auto& refPosition = refIt->position;
    if (refPosition.Meets(range)) {
      checkFinish = true;
      continue;
    } else if (refPosition.IsBefore(range)) {
      continue;
    } 
    
    if (range.Meets(refPosition)) {
      if (checkFinish) {
        return std::nullopt;
      } else {
        break;
      }
    } else if (refPosition.IsAfter(range)) {
      break;
    }

    if (refPosition.Contains(range) && expandRefs) {
      range = refPosition;
    }
    
    if (range.Contains(refPosition)) {
      if (eraseStart != end(refs)) {
        continue;
      } else {
        eraseStart = refIt;
      }
    } else {
      return std::nullopt;
    }
  }
  ShiftAllAfter(refs, refIt, -range.length());
  if (refIt != end(refs)) {
    refIt->position.Shift(-range.length());
  }
  if (eraseStart != end(refs)) {
    refs.erase(eraseStart, refIt);
  }

  return range;
}

std::string RefsManager::OutputRefs(const std::string& normStr) const {
  return OutputRefs(normStr, StrRange{ 0, SizeInCodePoints(normStr) });
}

std::string RefsManager::OutputRefs(const std::string& normStr, const StrRange subRange) const {
  std::string result{};
  auto curPos = subRange.start;
  auto finish = subRange.finish;
  for (const auto& ref : refs) {
    const auto intersection = subRange.Intersect(ref.position);
    if (!intersection.has_value() || intersection->empty()) {
      if (subRange.IsBefore(ref.position)) {
        break;
      }
    } else if (intersection->length() * 2 < ref.position.length()) {
      if (ref.position.finish >= finish) {
        finish = std::min(finish, ref.position.start);
        break;
      } else {
        curPos = std::max(curPos, ref.position.finish);
      }
    } else {
      if (curPos < ref.position.start) {
        result += Substr(normStr, StrRange{ curPos, ref.position.start });
      }
      result += ref.ToString();
      curPos = ref.position.finish;
    }
  }
  if (curPos < finish) {
    result += Substr(normStr, StrRange{ curPos, finish });
  }
  return result;
}

void RefsManager::ResolveIt(RefIter target) {
  if (target->IsEntity()) {
    target->ResolveEntity(*context);
  } else {
    const auto* master = FindMaster(target, target->GetOffset());
    target->ResolveCollaboration(master);
  }
}

const Reference* RefsManager::FindMaster(const RefIter base, const int16_t offset) const {
  if (offset == 0) {
    return nullptr;
  }
  auto termCounter = abs(offset);
  const auto boundary = offset > 0 ? prev(end(refs)) : begin(refs);

#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 26496 ) // Note: false positive const for iterator
#endif
  const auto sign = offset > 0 ? 1 : -1;
  for (auto it = base; it != boundary; ) {
    std::advance(it, sign);
    if (it->IsEntity() && (--termCounter == 0)) {
      return &*it;
    }
  }
#ifdef _MSC_VER
#pragma warning( pop )
#endif

  return nullptr;
}

} // namespace ccl::lang