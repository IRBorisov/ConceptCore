#pragma once

#include "ccl/rslang/SyntaxTree.h"
#include "ccl/semantic/SchemaAuditor.h"
#include "ccl/semantic/RSConcept.h"
#include "ccl/graph/CGraph.h"

#include <map>

namespace ccl::semantic {

class RSIterator;

//! Parsing status enumeration
enum class ParsingStatus : int8_t {
  UNKNOWN = 0,
  VERIFIED,
  INCORRECT,
};

struct ParsingInfo {
  ParsingStatus status{ ParsingStatus::UNKNOWN };
  meta::UniqueCPPtr<rslang::SyntaxTree> ast{};
  std::optional<rslang::ExpressionType> exprType{};
  std::optional<rslang::FunctionArguments> arguments{};
  rslang::ValueClass valueClass{ rslang::ValueClass::invalid };

  ~ParsingInfo() noexcept = default;
  ParsingInfo() noexcept = default;

  ParsingInfo(const ParsingInfo& info2);
  ParsingInfo& operator=(const ParsingInfo& info2);
  ParsingInfo(ParsingInfo&& info2) noexcept;
  ParsingInfo& operator=(ParsingInfo&& info2) noexcept;

  [[nodiscard]] const rslang::ExpressionType* TypePtr() const;
  [[nodiscard]] const rslang::Typification* Typification() const;
  void Reset() noexcept;
};

//! RS schema for system of concepts
class Schema final : public rslang::TypeContext {
  friend class RSIterator;

  // Note: using map for consistent iteration order
  using Storage = std::map<EntityUID, RSConcept>;
  Storage storage{};

  std::unordered_map<EntityUID, ParsingInfo> info{};
  mutable graph::UpdatableGraph graph;

  // Note: Auditor should be created last because it needs context references
  using RSAuditor = std::unique_ptr<SchemaAuditor>;
  mutable RSAuditor auditor{ std::make_unique<SchemaAuditor>(*this, VCContext(), ASTContext()) };

public:
  ~Schema() noexcept final = default;
  explicit Schema();

  Schema(const Schema& rhs);
  Schema& operator=(const Schema& rhs);
  Schema(Schema&& rhs) noexcept;
  Schema& operator=(Schema&& rhs) noexcept;

public:
  [[nodiscard]] RSIterator begin() const noexcept;
  [[nodiscard]] RSIterator end() const noexcept;
  [[nodiscard]] size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept { return size() == 0; }

  [[nodiscard]] size_t Count(CstType type) const;
  [[nodiscard]] bool Contains(EntityUID entity) const;
  [[nodiscard]] const RSConcept& At(EntityUID entity) const;
  [[nodiscard]] const ParsingInfo& InfoFor(EntityUID entity) const;

  [[nodiscard]] std::optional<EntityUID> FindAlias(const std::string& alias) const;
  [[nodiscard]] std::optional<EntityUID> FindExpr(const std::string& expr) const;

  [[nodiscard]] const graph::CGraph& Graph() const;

  [[nodiscard]] const rslang::ExpressionType* TypeFor(const std::string& globalName) const final;
  [[nodiscard]] const rslang::FunctionArguments* FunctionArgsFor(const std::string& globalName) const final;
  [[nodiscard]] std::optional<rslang::TypeTraits> TraitsFor(const rslang::Typification& type) const final;

  [[nodiscard]] rslang::SyntaxTreeContext ASTContext() const;
  [[nodiscard]] rslang::ValueClassContext VCContext() const;  

  bool Emplace(
    EntityUID newID,
    std::string newAlias,
    CstType type,
    std::string definition = {},
    std::string convention = {}
  );
  bool InsertCopy(const RSConcept& cst);
  bool Insert(RSConcept&& cst);
  void Load(RSConcept&& cst);
  void Erase(EntityUID target);

  bool SetAliasFor(EntityUID target, const std::string& newValue, bool substitueMentions = true);
  bool SetTypeFor(EntityUID target, CstType newValue);
  bool SetDefinitionFor(EntityUID target, const std::string& expression);
  bool SetConventionFor(EntityUID target, const std::string& convention);

  void SubstitueAliases(const StrTranslator& old2New);
  void Translate(EntityUID target, const StrTranslator& old2New);
  void TranslateAll(const StrTranslator& old2New);

  [[nodiscard]] std::unique_ptr<SchemaAuditor> MakeAuditor() const;
  [[nodiscard]] std::optional<rslang::ExpressionType> Evaluate(const std::string& input) const;

  void UpdateState();

private:
  void ResetInfo() noexcept;
  void TriggerParse(EntityUID target);
  void ParseCst(EntityUID target);
  void SaveInfoTo(ParsingInfo& out);
};

//! RS concept iterator
class RSIterator {
  friend class Schema;
  using Container = Schema::Storage;
  Container::const_iterator iterator;

private:
  explicit RSIterator(Container::const_iterator iter) noexcept;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = RSConcept;
  using difference_type = std::ptrdiff_t;
  using pointer = const RSConcept*;
  using reference = const RSConcept&;

  [[nodiscard]] bool operator==(const RSIterator& iter2) const noexcept;
  [[nodiscard]] bool operator!=(const RSIterator& iter2) const noexcept  { return !(*this == iter2); }

  RSIterator& operator++() noexcept;

  [[nodiscard]] pointer operator->() const noexcept;
  [[nodiscard]] reference operator*() const noexcept;
};

} // namespace ccl::semantic
