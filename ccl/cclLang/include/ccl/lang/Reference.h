#pragma once

#include "ccl/Strings.hpp"
#include "ccl/cclMeta.hpp"
#include "ccl/Substitutes.hpp"
#include "ccl/lang/EntityTermContext.hpp"

#include <memory>
#include <variant>
#include <string_view>

namespace ccl::lang {

//! Reference types
enum class ReferenceType : uint8_t {
  invalid = 0,
  entity,
  collaboration
};

//! Reference to entitity in a specific form
struct EntityRef {
  std::string entity;
  Morphology form;

  explicit EntityRef(std::string entity, Morphology form) noexcept
    : entity{ std::move(entity) }, form{ std::move(form)} {}

  enum Fields : uint8_t {
    TR_ENTITY = 0,
    TR_TAGS = 1
  };
  static constexpr uint8_t fieldCount = 2;

  [[nodiscard]] std::string ToString() const;
};

//! Reference to a text in collaboration with another reference
struct CollaborationRef {
  std::string nominal;
  int16_t offset;

  explicit CollaborationRef(std::string text, int16_t offset) noexcept
    : nominal{ std::move(text) }, offset{ offset } {}

  enum Fields : uint8_t {
    CR_OFFSET = 0,
    CR_TEXT = 1
  };
  static constexpr uint8_t fieldCount = 2;

  [[nodiscard]] std::string ToString() const;
};

//! Reference facade
class Reference {
  using ReferenceData = std::variant<std::monostate, EntityRef, CollaborationRef>;
  ReferenceData data{};
  ReferenceType type{ ReferenceType::invalid };

public:
  StrRange position{};
  std::string resolvedText{};

public:
  Reference() noexcept = default;
  explicit Reference(EntityRef data, StrRange position_ = {}) noexcept
    : data{ std::move(data) }, type{ ReferenceType::entity }, position{ position_ } {}
  explicit Reference(CollaborationRef data, StrRange position_ = {}) noexcept
    : data{ std::move(data) }, type{ ReferenceType::collaboration }, position{ position_ } {}

  [[nodiscard]] static Reference Parse(std::string_view refStr);
  [[nodiscard]] static std::vector<Reference> ExtractAll(std::string_view text);

  //! Common API
  [[nodiscard]] ReferenceType GetType() const noexcept;
  [[nodiscard]] bool IsValid() const noexcept;
  [[nodiscard]] bool IsEntity() const noexcept;
  [[nodiscard]] bool IsCollaboration() const noexcept;
  [[nodiscard]] std::string ToString() const;

  //! Entity specific API
  [[nodiscard]] std::string_view GetEntity() const;
  [[nodiscard]] const Morphology& GetForm() const;
  [[nodiscard]] bool TranslateEntity(const StrTranslator& old2New);
  void ResolveEntity(const EntityTermContext& context);

  //! Collaboration specific API
  [[nodiscard]] int16_t GetOffset() const;
  [[nodiscard]] const std::string& GetNominal() const;
  void ResolveCollaboration(const Reference* master);
};

} // namespace ccl::lang