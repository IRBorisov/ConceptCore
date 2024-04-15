#include "ccl/semantic/Schema.h"

#include "ccl/semantic/RSCore.h"
#include "ccl/rslang/RSGenerator.h"

namespace ccl::semantic {

void ParsingInfo::Reset() noexcept {
  *this = {};
}

const rslang::ExpressionType* ParsingInfo::TypePtr() const {
  if (exprType.has_value()) {
    return &exprType.value();
  } else {
    return nullptr;
  }
}

const rslang::Typification* ParsingInfo::Typification() const {
  return std::get_if<ccl::rslang::Typification>(TypePtr());
}

ParsingInfo::ParsingInfo(const ParsingInfo& info2) 
  : status{ info2.status }, exprType{ info2.exprType }, 
  arguments{ info2.arguments }, valueClass{ info2.valueClass } {
  if (info2.ast != nullptr) {
    ast = std::make_unique<rslang::SyntaxTree>(*info2.ast);
  } else {
    ast = nullptr;
  }
}

ParsingInfo& ParsingInfo::operator=(const ParsingInfo& info2) {
  if (&info2 != this) {
    status = info2.status;
    if (info2.ast != nullptr) {
      ast = std::make_unique<rslang::SyntaxTree>(*info2.ast);
    } else {
      ast = nullptr;
    }
    exprType = info2.exprType;
    arguments = info2.arguments;
    valueClass = info2.valueClass;
  }
  return *this;
}

ParsingInfo::ParsingInfo(ParsingInfo&& info2) noexcept 
  : status{ info2.status }, ast{ std::move(info2.ast) }, exprType { std::move(info2.exprType) },
  arguments{ std::move(info2.arguments) }, valueClass{info2.valueClass} {}

ParsingInfo& ParsingInfo::operator=(ParsingInfo&& info2) noexcept {
  if (&info2 != this) {
    status = info2.status;
    ast = std::move(info2.ast);
    exprType = std::move(info2.exprType);
    arguments = std::move(info2.arguments);
    valueClass = info2.valueClass;
  }
  return *this;
}

Schema::Schema()
  : graph{ [&schema = *this] (const EntityUID entity) {
      auto globalIDs = rslang::ExtractUGlobals(schema.At(entity).definition);
      SetOfEntities uids{};
      for (const auto& name : globalIDs) {
        if (const auto uid = schema.FindAlias(name); uid.has_value()) {
          uids.emplace(uid.value());
        }
      }
      return uids;
    }
} {}

Schema::Schema(const Schema& rhs) : Schema() {
  storage = rhs.storage; // NOLINT
  info = rhs.info; // NOLINT
  graph.Invalidate();
}

Schema& Schema::operator=(const Schema& rhs) {
  if (&rhs != this) {
    storage = rhs.storage;
    info = rhs.info;
    graph.Clear();
    graph.Invalidate();
  }
  return *this;
}

// TODO: fix this noexcept
Schema::Schema(Schema&& rhs) noexcept // NOLINT(bugprone-exception-escape)
  : Schema() {
  storage = std::move(rhs.storage); // NOLINT
  info = std::move(rhs.info); // NOLINT
  graph.Invalidate();
}

Schema& Schema::operator=(Schema&& rhs) noexcept {
  if (&rhs != this) {
    storage = std::move(rhs.storage);
    info = std::move(rhs.info);
    graph.Clear();
    graph.Invalidate();
  }
  return *this;
}

bool Schema::Contains(const EntityUID entity) const {
  return storage.contains(entity);
}

const RSConcept& Schema::At(const EntityUID entity) const {
  return storage.at(entity);
}

const ParsingInfo& Schema::InfoFor(const EntityUID entity) const {
  return info.at(entity);
}

std::optional<EntityUID> Schema::FindAlias(const std::string& alias) const {
  auto result = std::find_if(begin(), end(),
                             [&alias](const auto& record) noexcept { return record.alias == alias; });
  if (result != end()) {
    return result->uid;
  } else {
    return std::nullopt;
  }
}

std::optional<EntityUID> Schema::FindExpr(const std::string& expr) const {
  const auto exprTree = rslang::GenerateAST(expr);
  for (const auto& cst : *this) {
    if (cst.definition == expr) {
      return cst.uid;
    } else if (exprTree != nullptr) {
      const auto secondTree = rslang::GenerateAST(cst.definition);
      if (secondTree != nullptr && *secondTree == *exprTree) {
        return cst.uid;
      }
    }
  }
  return std::nullopt;
}

size_t Schema::Count(const CstType type) const {
  return static_cast<size_t>(std::count_if(begin(), end(), [&type](const auto& cst) { return type == cst.type; }));
}

const graph::CGraph& Schema::Graph() const {
  if (graph.IsBroken()) {
    graph.Clear();
    graph.SetValid();
    for (const auto& cst : storage) {
      graph.UpdateFor(cst.first);
    }
  }
  return graph;
}

bool Schema::SetAliasFor(const EntityUID target, const std::string& newValue, const bool substitueMentions) {
  if (!Contains(target)) {
    return false;
  } else if (At(target).alias == newValue) {
    return false;
  }
  graph.Invalidate();
  if (!substitueMentions) {
    storage.at(target).alias = newValue;
    UpdateState();
  } else {
    const std::string oldValue = At(target).alias;
    storage.at(target).alias = newValue;
    TranslateAll(CreateTranslator({ { oldValue, newValue } }));
  }
  return true;
}

bool Schema::SetTypeFor(const EntityUID target, CstType newValue) {
  if (!Contains(target)) {
    return false;
  } else if (At(target).type == newValue) {
    return false;
  }
  storage.at(target).type = newValue;
  UpdateState();
  return true;
}

bool Schema::SetDefinitionFor(const EntityUID target, const std::string& expression) {
  if (!Contains(target)) {
    return false;
  } else if (expression == At(target).definition) {
    return false;
  } else {
    auto& cst = storage.at(target);
    const auto copyID = FindExpr(expression);
    const bool realChange = !copyID.has_value() || copyID.value() != cst.uid;
    cst.definition = expression;
    if (realChange) {
      graph.UpdateFor(target);
      TriggerParse(target);
    }
    return realChange;
  }
}

bool Schema::SetConventionFor(const EntityUID target, const std::string& convention) {
  if (!Contains(target)) {
    return false;
  } else if (At(target).convention == convention) {
    return false;
  } else {
    storage.at(target).convention = convention;
    return true;
  }
}

void Schema::SubstitueAliases(const StrTranslator& old2New) {
  for (auto& cst : storage) {
    auto& name = cst.second.alias;
    if (const auto newName = old2New(name); newName.has_value()) {
      name = newName.value();
    }
  }
  graph.Invalidate();
  TranslateAll(old2New);
}

void Schema::Translate(const EntityUID target, const StrTranslator& old2New) {
  storage.at(target).Translate(old2New);
  graph.UpdateFor(target);
  TriggerParse(target);
}

void Schema::TranslateAll(const StrTranslator& old2New) {
  for (const auto& cst : *this) {
    storage.at(cst.uid).Translate(old2New);
    graph.UpdateFor(cst.uid);
  }
  UpdateState();
}

bool Schema::Emplace(const EntityUID newID, std::string newAlias, const CstType type,
                     std::string definition, std::string convention) {
  if (info.emplace(std::pair{ newID, ParsingInfo{} }).second) {
    storage.emplace(std::pair{ newID, 
                    RSConcept{ newID, std::move(newAlias), type, std::move(definition), std::move(convention) } });
    graph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

bool Schema::Insert(RSConcept&& cst) {
  if (info.emplace(std::pair{ cst.uid, ParsingInfo{} }).second) {
    storage.emplace(std::pair{ cst.uid, std::move(cst) });
    graph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

bool Schema::InsertCopy(const RSConcept& cst) {
  if (info.emplace(std::pair{ cst.uid, ParsingInfo{} }).second) {
    storage.emplace(std::pair{ cst.uid, cst });
    graph.Invalidate();
    UpdateState();
    return true;
  } else {
    return false;
  }
}

void Schema::Load(RSConcept&& cst) {
  info[cst.uid] = ParsingInfo{};
  storage[cst.uid] = std::move(cst);
  graph.Invalidate();
  ResetInfo();
}

void Schema::Erase(const EntityUID target)  {  
  if (!Contains(target)) {
    return;
  }
  for (const auto uid : Graph().ExpandOutputs({ target })) {
    info.at(uid).Reset();
  }
  info.erase(target);
  graph.EraseItem(target);
  storage.erase(target);
  UpdateState();
}

const rslang::ExpressionType*
Schema::TypeFor(const std::string& globalName) const {
  if (const auto uid = FindAlias(globalName); 
      !uid.has_value() || !info.at(uid.value()).exprType.has_value()) {
    return nullptr;
  } else {
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): this is false positive. Access is checked above
    return &info.at(uid.value()).exprType.value();
  }
}
const rslang::FunctionArguments*
Schema::FunctionArgsFor(const std::string& globalName) const {
  if (const auto uid = FindAlias(globalName); !uid.has_value() || !info.at(uid.value()).arguments.has_value()) {
    return nullptr;
  } else {
    // NOLINTNEXTLINE(bugprone-unchecked-optional-access): this is false positive. Access is checked above
    return &info.at(uid.value()).arguments.value();
  }
}

std::optional<rslang::TypeTraits>
Schema::TraitsFor(const rslang::Typification& type) const {
  if (!type.IsElement()) {
    return std::nullopt;
  } else if (type == rslang::Typification::Integer()) {
    return rslang::TraitsIntegral;
  } else if (const auto baseUID = FindAlias(type.E().baseID); !baseUID.has_value()) {
    return std::nullopt;
  } else if (const auto baseType = At(baseUID.value()).type; baseType == CstType::base) {
    return rslang::TraitsNominal;
  } else if (baseType == CstType::constant) {
    return rslang::TraitsIntegral;
  } else {
    return std::nullopt;
  }
}

rslang::SyntaxTreeContext Schema::ASTContext() const {
  return [&schema = *this](const std::string& name) -> const rslang::SyntaxTree* {
    if (const auto uid = schema.FindAlias(name); !uid.has_value()) {
      return nullptr;
    } else {
      return schema.info.at(uid.value()).ast.get();
    }
  };
}

rslang::ValueClassContext Schema::VCContext() const {
  return [&schema = *this](const std::string& name) -> rslang::ValueClass {
    if (const auto uid = schema.FindAlias(name); !uid.has_value()) {
      return rslang::ValueClass::invalid;
    } else {
      return schema.info.at(uid.value()).valueClass;
    }
  };
}

std::optional<rslang::ExpressionType>
Schema::Evaluate(const std::string& input) const {
  if (!auditor->CheckType(input, rslang::Syntax::MATH)) {
    return std::nullopt;
  } else {
    return auditor->GetType();
  }
}

[[nodiscard]] std::unique_ptr<rslang::Auditor>
Schema::MakeAuditor() const {
  return std::make_unique<rslang::Auditor>(*this, VCContext(), ASTContext());
}

void Schema::TriggerParse(const EntityUID target) {
  ParseCst(target);
  const auto expansion = Graph().ExpandOutputs({ target });
  const auto orderedList = Graph().Sort(expansion);
  for (const auto dependant : orderedList) {
    if (dependant != target) {
      ParseCst(dependant);
    }
  }
}

void Schema::ParseCst(const EntityUID target) {
  using rslang::Generator;
  const auto& cst = At(target);
  const auto expr = Generator::GlobalDefinition(cst.alias, cst.definition, cst.type == CstType::structured);
  const auto isValid = 
    auditor->CheckType(expr, rslang::Syntax::MATH) 
    && CheckTypeConstistency(auditor->GetType(), cst.type)
    && (IsBaseSet(cst.type) == std::empty(cst.definition));
  
  info.at(target).Reset();
  info.at(target).status = isValid ? ParsingStatus::VERIFIED : ParsingStatus::INCORRECT;
  if (isValid) {
    SaveInfoTo(info.at(target));
  }
}

bool Schema::CheckTypeConstistency(const rslang::ExpressionType& type, const CstType cstType) noexcept {
  switch (cstType) {
  default: return false;
  case CstType::base:
  case CstType::constant:
  case CstType::structured:
  case CstType::function:
  case CstType::term: {
    return std::holds_alternative<rslang::Typification>(type);
  }
  case CstType::predicate:
  case CstType::axiom:
  case CstType::theorem: {
    return std::holds_alternative<rslang::LogicT>(type);
  }
  }
}

void Schema::SaveInfoTo(ParsingInfo& out) {
  out.exprType = auditor->GetType();
  if (const auto& funcArgs = auditor->GetDeclarationArgs(); !std::empty(funcArgs)) {
    out.arguments = funcArgs;
  }
  if (auditor->CheckValue()) {
    out.valueClass = auditor->GetValueClass();
  }
  if (auditor->isParsed) {
    out.ast = auditor->parser.ExtractAST();
  }
}

void Schema::ResetInfo() noexcept {
  for (auto& it : info) {
    it.second.Reset();
  }
}

void Schema::UpdateState() {
  ResetInfo();
  for (const auto entity : Graph().TopologicalOrder()) {
    ParseCst(entity);
  }
}

RSIterator Schema::begin() const noexcept {
  return RSIterator(std::begin(storage));
}

RSIterator Schema::end() const noexcept {
  return RSIterator(std::end(storage));
}

size_t Schema::size() const noexcept {
  return std::size(storage);
}

RSIterator::RSIterator(Container::const_iterator iter) noexcept
  : iterator{ std::move(iter) } {}

RSIterator& RSIterator::operator++() noexcept {
  ++iterator;
  return *this;
}

RSIterator::pointer RSIterator::operator->() const noexcept {
  return &iterator->second;
}

RSIterator::reference RSIterator::operator*() const noexcept {
  return iterator->second;
}

bool RSIterator::operator==(const RSIterator& iter2) const noexcept {
  return iterator == iter2.iterator;
}

} // namespace ccl::semantic
