#include "ccl/lang/Reference.h"

// TODO: Use format library for GCC when available (GCC13+)
#if !defined(__GNUC__) && !defined(__clang__)
#include <format>
#endif

#include "ccl/lang/TextEnvironment.h"
#include "ccl/lang/LexicalTerm.h"

namespace ccl::lang {

namespace {

[[nodiscard]] const std::string& EmptyRefCheck(const std::string& result) {
  static const std::string emptyOut{ R"(!Empty reference!)" };
  if (empty(result)) {
    return emptyOut;
  } else {
    return result;
  }
}

[[nodiscard]] std::vector<std::string_view> SplitReference(std::string_view ref) {
  static constexpr auto prefixLen = 2U; // prefix string: "@{"
  static constexpr auto suffixLen = 1U; // suffix string: "}"
  if (ref.size() <= prefixLen + suffixLen) {
    return {};
  } else {
    return SplitBySymbol(ref.substr(prefixLen, ref.length() - prefixLen - suffixLen), '|');
  }
}

[[nodiscard]] ReferenceType DeduceRefType(const std::vector<std::string_view>& tokens) noexcept {
  if (size(tokens) < EntityRef::fieldCount || size(tokens) > EntityRef::fieldCount + 2) {
    return ReferenceType::invalid;
  }
  const auto firstToken = tokens.at(0);
  if (empty(firstToken)) {
    return ReferenceType::invalid;
  }
  const auto& firstSymbol = firstToken.at(0);
  if (isalpha(firstSymbol)) {
    return ReferenceType::entity;
  } else if (IsInteger(firstToken) && size(tokens) == CollaborationRef::fieldCount) {
    return ReferenceType::collaboration;
  }
  return ReferenceType::invalid;
}

[[nodiscard]] Morphology ExtractMorpho(const std::vector<std::string_view>& tokens) noexcept {
  if (size(tokens) == EntityRef::fieldCount) {
    return Morphology{ tokens.at(EntityRef::TR_TAGS) };
  } else {
    auto tags = tokens;
    tags.erase(begin(tags));
    if (std::isdigit(tags.rbegin()->at(0))) {
      tags.erase(prev(end(tags)));
    }
    return Morphology{ tags };
  }
}

[[nodiscard]] UTF8Iterator ReferenceStart(const std::string_view refStr, const StrPos start) noexcept {
  for (auto iter = UTF8Iterator(refStr, start); iter != UTF8End(refStr); ++iter) {
    if (*iter == '@') {
      ++iter;
      if (iter == UTF8End(refStr) || *iter == '{') {
        return iter;
      }
    }
  }
  return UTF8End(refStr);
}

[[nodiscard]] UTF8Iterator ReferenceEnd(const std::string_view refStr, const UTF8Iterator start) noexcept {
  auto bracketCount = 0;
  for (auto iter = start; iter != UTF8End(refStr); ++iter) {
    if (*iter == '{') {
      ++bracketCount;
    } else if (*iter == '}') {
      --bracketCount;
    }
    if (bracketCount == 0) {
      return iter;
    }
  }
  return UTF8End(refStr);
}

[[nodiscard]] std::optional<StrRange>
NextReference(const std::string_view refStr, const StrPos start = 0) noexcept {
  if (const auto refStart = ReferenceStart(refStr, start); refStart == UTF8End(refStr)) {
    return std::nullopt;
  } else if (const auto refEnd = ReferenceEnd(refStr, refStart); refEnd == UTF8End(refStr)) {
    return std::nullopt;
  } else {
    return StrRange{ refStart.Position() - 1, refEnd.Position() + 1 };
  }
}

} // namespace

std::string EntityRef::ToString() const {
#if !defined(__GNUC__) && !defined(__clang__)
  return std::format("@{{{}|{}}}", entity, form.ToString());
#else
  return "@{" + entity + '|' + form.ToString() + '}';
#endif
}

std::string CollaborationRef::ToString() const {
#if !defined(__GNUC__) && !defined(__clang__)
  return std::format("@{{{}|{}}}", offset, nominal);
#else
  return "@{" + std::to_string(offset) + '|' + nominal + '}';
#endif
}

Reference Reference::Parse(std::string_view refStr) {
  const auto tokens = SplitReference(refStr);
  const auto type = DeduceRefType(tokens);
  switch (type) {
  case ReferenceType::entity: {
    auto form = ExtractMorpho(tokens);
    if (std::empty(form)) {
      return {};
    }
    return Reference{ EntityRef{ std::string{ tokens.at(EntityRef::TR_ENTITY) }, std::move(form) } };
  }
  case ReferenceType::collaboration: {
    return Reference{ 
      CollaborationRef{ std::string{ tokens.at(CollaborationRef::CR_TEXT) },
      static_cast<int16_t>(stoi(std::string{ tokens.at(CollaborationRef::CR_OFFSET) })) } 
    };
  }
  default:
  case ReferenceType::invalid: return {};
  }
}

std::vector<Reference> Reference::ExtractAll(const std::string_view text) {
  std::vector<Reference> result{};
  for (auto position = NextReference(text); position.has_value();
    position = NextReference(text, position->finish)) {
    if (auto ref = Reference::Parse(Substr(text, position.value())); ref.IsValid()) {
      ref.position = position.value();
      result.emplace_back(std::move(ref));
    }
  }
  return result;
}

ReferenceType Reference::GetType() const noexcept { return type; }
bool Reference::IsValid() const noexcept { return GetType() != ReferenceType::invalid; }
bool Reference::IsEntity() const noexcept { return GetType() == ReferenceType::entity; }
bool Reference::IsCollaboration() const noexcept { return GetType() == ReferenceType::collaboration; }
std::string_view Reference::GetEntity() const { return std::get<EntityRef>(data).entity; }
const Morphology& Reference::GetForm() const { return std::get<EntityRef>(data).form; }

bool Reference::TranslateEntity(const StrTranslator& old2New) {
  auto& refData = std::get<EntityRef>(data);
  const auto newID = old2New(refData.entity);
  if (!newID.has_value() || newID.value() == refData.entity) {
    return false;
  }
  refData.entity = newID.value();
  return true;
}

int16_t Reference::GetOffset() const { return std::get<CollaborationRef>(data).offset; }
const std::string& Reference::GetNominal() const { return std::get<CollaborationRef>(data).nominal; }

std::string Reference::ToString() const {
  switch (type) {
  case ReferenceType::entity:  return std::get<EntityRef>(data).ToString();
  case ReferenceType::collaboration: return std::get<CollaborationRef>(data).ToString();
  default:
  case ReferenceType::invalid:  return "";
  }
}

void Reference::ResolveCollaboration(const Reference* master) {
  const auto& refData = std::get<CollaborationRef>(data);
  if (empty(refData.nominal)) {
    resolvedText = EmptyRefCheck(refData.nominal);
  } else if(master == nullptr) {
#if !defined(__GNUC__) && !defined(__clang__)
    resolvedText = std::format("!Invalid offset for {}: '{}'!", refData.nominal, refData.offset);
#else
    resolvedText = "!Invalid offset for " + refData.nominal + ": '" + std::to_string(refData.offset) + "'!";
#endif
  } else {
    resolvedText = EmptyRefCheck(TextEnvironment::Processor().InflectDependant(refData.nominal, master->resolvedText));
  }
}

void Reference::ResolveEntity(const EntityTermContext& context) {
  auto& refData = std::get<EntityRef>(data);
  if (empty(refData.entity)) {
    resolvedText = EmptyRefCheck(refData.entity);
  } else if (const auto* entity = context.At(refData.entity); entity == nullptr) {
#if !defined(__GNUC__) && !defined(__clang__)
    resolvedText = std::format("!Cannot find entity: '{}'!", refData.entity);
#else
    resolvedText = "!Cannot find entity: '" + refData.entity +"'!";
#endif
  } else if (auto result = entity->GetForm(refData.form); std::empty(result)) {
    resolvedText = EmptyRefCheck(entity->Nominal());
  } else {
    resolvedText = EmptyRefCheck(result);
  }
}

} // namespace ccl::lang