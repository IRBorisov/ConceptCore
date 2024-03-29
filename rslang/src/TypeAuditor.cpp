#include "ccl/rslang/TypeAuditor.h"

namespace ccl::rslang {

namespace {

// Helper functions
[[nodiscard]] std::string ToString(const ExpressionType& type) noexcept(false);
[[nodiscard]] constexpr bool IsSubset(TokenID token) noexcept;
[[nodiscard]] bool IsEchelon(SyntaxTree::Cursor iter);
[[nodiscard]] bool IsEchelon(SyntaxTree::Cursor iter, Index index);
void MangleRadicals(const std::string& funcName, Typification& type);

std::string ToString(const ExpressionType& type) noexcept(false) {
	return std::visit(meta::Overloads{
		[](const Typification& t) { return t.ToString();	},
		[](const LogicT& /*t*/) { return std::string{"LOGIC"}; },
										}, type);
}

constexpr bool IsSubset(const TokenID token) noexcept {
	return token == TokenID::SUBSET
		|| token == TokenID::SUBSET_OR_EQ
		|| token == TokenID::NOTSUBSET;
}

bool IsEchelon(SyntaxTree::Cursor iter) {
	switch (iter->id) {
	default: return false;

	case TokenID::LIT_INTSET:
	case TokenID::ID_GLOBAL:
	case TokenID::BOOLEAN:
	case TokenID::DECART:
		break;
	}
	for (Index i = 0; i < iter.ChildrenCount(); ++i) {
		if (!IsEchelon(iter, i)) {
			return false;
		}
	}
	return true;
}

bool IsEchelon(SyntaxTree::Cursor iter, const Index index) {
	iter.MoveToChild(index);
	return IsEchelon(iter);
}

bool IsRadical(const std::string& globalName) {
	return !empty(globalName) && globalName.at(0) == 'R';
}

void MangleRadicals(const std::string& funcName, Typification& type) {
	switch (type.Structure()) {
	case StructureType::basic: {
		if (IsRadical(type.E().baseID)) {
			type.E().baseID += funcName;
		}
		return;
	}
	case StructureType::collection: {
		MangleRadicals(funcName, type.B().Base());
		return;
	}
	case StructureType::tuple: {
		for (auto index = Typification::PR_START; index < type.T().Arity() + Typification::PR_START; ++index) {
			MangleRadicals(funcName, type.T().Component(index));
		}
		return;
	}
	}
}

} // unnamed namespace

namespace details {

bool TypeEnv::IsArithmetic(const Typification& type) const {
	const auto traits = context.TraitsFor(type);
	return traits.has_value() && traits.value().isOperable;
}

bool TypeEnv::IsOrdered(const Typification& type) const {
	const auto traits = context.TraitsFor(type);
	return traits.has_value() && traits.value().isOrdered;
}

const Typification* TypeEnv::CommonType(const Typification& type1, const Typification& type2) const {
	if (type1 == Typification::Integer()) {
		const auto traits = context.TraitsFor(type2);
		if (!traits.has_value() || !traits.value().convertsFromInt) {
			return nullptr;
		} else {
			return &type2;
		}
	} else if (type2 == Typification::Integer()) {
		const auto traits = context.TraitsFor(type1);
		if (!traits.has_value() || !traits.value().convertsFromInt) {
			return nullptr;
		} else {
			return &type1;
		}
	} else {
		return nullptr;
	}
}

bool TypeEnv::AreCompatible(const ExpressionType& type1, const ExpressionType& type2) const {
	if (std::holds_alternative<LogicT>(type1)) {
		return std::holds_alternative<LogicT>(type2);
	} else {
		return AreCompatible(std::get<Typification>(type1), std::get<Typification>(type2));
	}
}

bool TypeEnv::AreCompatible(const Typification& type1, const Typification& type2) const {
	if (type1 == type2) {
		return true;
	} else if (type1.Structure() != type2.Structure()) {
		return false;
	}
	switch (type1.Structure()) {
	default:
	case rslang::StructureType::basic: return CommonType(type1, type2) != nullptr;
	case rslang::StructureType::collection:	return AreCompatible(type1.B().Base(), type2.B().Base());
	case rslang::StructureType::tuple: {
		if (type1.T().Arity() != type2.T().Arity()) {
			return false;
		} else {
			for (auto index = Typification::PR_START; index < type1.T().Arity() + Typification::PR_START; ++index) {
				if (!AreCompatible(type1.T().Component(index), type2.T().Component(index))) {
					return false;
				}
			}
			return true;
		}
	}
	}
}

std::optional<Typification> TypeEnv::Merge(const Typification& type1, const Typification& type2) const {
	if (type1 == type2) {
		return type1;
	} else if (type1.Structure() != type2.Structure()) {
		return std::nullopt;
	}
	switch (type1.Structure()) {
	default:
	case rslang::StructureType::basic: {
		if (const auto* type = CommonType(type1, type2); type == nullptr) {
			return std::nullopt;
		} else {
			return *type;
		}
	}
	case rslang::StructureType::collection: {
		if (auto base = Merge(type1.B().Base(), type2.B().Base()); !base.has_value()) {
			return std::nullopt;
		} else {
			return base.value().ApplyBool();
		}
	}
	case rslang::StructureType::tuple: {
		if (type1.T().Arity() != type2.T().Arity()) {
			return std::nullopt;
		} else {
			std::vector<Typification> components{};
			for (auto index = Typification::PR_START; index < type1.T().Arity() + Typification::PR_START; ++index) {
				if (auto component = Merge(type1.T().Component(index), type2.T().Component(index));
						!component.has_value()) {
					return std::nullopt;
				} else {
					components.emplace_back(std::move(component.value()));
				}
			}
			return Typification::Tuple(components);
		}
	}
	}
}

bool TypeEnv::CompareTemplated(Typification::Substitutes& substitutes,
															 const Typification& arg,
															 const Typification& value) {
	if (arg == value) {
		return true;
	}
	if (arg.IsElement() && IsRadical(arg.E().baseID)) {
		if (const auto& base = arg.E().baseID; !substitutes.contains(base)) {
			substitutes.insert({ base, value });
			return true;
		} else if (auto mergeType = Merge(substitutes.at(base), value);
							 !mergeType.has_value()) {
			return false;
		} else {
			substitutes.at(base) = std::move(mergeType.value());
			return true;
		}
	} else if (arg.Structure() != value.Structure()) {
		return false;
	} else {
		switch (arg.Structure()) {
		default:
		case rslang::StructureType::basic: return CommonType(arg, value) != nullptr;
		case rslang::StructureType::collection:	return CompareTemplated(substitutes, arg.B().Base(), value.B().Base());
		case rslang::StructureType::tuple: {
			if (arg.T().Arity() != value.T().Arity()) {
				return false;
			} else {
				for (auto index = Typification::PR_START; index < arg.T().Arity() + Typification::PR_START; ++index) {
					if (!CompareTemplated(substitutes, arg.T().Component(index), value.T().Component(index))) {
						return false;
					}
				}
				return true;
			}
		}
		}
	}
}

} // namespace details

bool TypeAuditor::CheckType(const rslang::SyntaxTree& tree) {
	Clear();
	return tree.Root().DispatchVisit(*this);
}

const ExpressionType& TypeAuditor::GetType() const noexcept {
	return currentType;
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position) {
	if (reporter.has_value()) {
		reporter.value()(Error{ static_cast<uint32_t>(eid), position });
	}
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position, std::string param) {
	if (reporter.has_value()) {
		reporter.value()(Error{ static_cast<uint32_t>(eid), position, { std::move(param) } });
	}
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position, std::vector<std::string> params) {
	if (reporter.has_value()) {
		reporter.value()(Error{ static_cast<uint32_t>(eid), position, std::move(params) });
	}
}

void TypeAuditor::OnError(const SemanticEID eid, const StrPos position,
							const ExpressionType& expected, const ExpressionType& actual) {
	if (reporter.has_value()) {
		reporter.value()(Error{ static_cast<uint32_t>(eid), position, { ToString(expected), ToString(actual) } });
	}
}

void TypeAuditor::Clear() noexcept {
	localVars.clear();
	functionArgs.clear();
	functionArgsID.clear();
	currentType = {};
}

bool TypeAuditor::ViGlobalDefinition(Cursor iter) {
	const auto childrenCount = iter.ChildrenCount();
	if (iter->id == TokenID::PUNC_STRUCT) {
		if (childrenCount != 2 || !IsEchelon(iter, 1)) {
			OnError(SemanticEID::globalStructure, iter(0).pos.finish);
			return false;
		} else if (const auto maybeType = ChildType(iter, 1); !maybeType.has_value()) {
			return false;
		} else if (const auto& type = std::get<Typification>(maybeType.value()); !type.IsCollection()) {
			return false;
		} else {
			return VisitAndReturn(type.B().Base());
		}
	} else {
		assert(iter->id == TokenID::PUNC_DEFINE);
		if (childrenCount == 1) {
			return VisitAndReturn(Typification{ iter(0).data.ToText() }.ApplyBool());
		} else if (const auto type = ChildType(iter, 1); !type.has_value()) {
				return false;
		} else {
			return VisitAndReturn(type.value());
		}
	}
}

bool TypeAuditor::ViFunctionDefinition(Cursor iter) {
	if (const auto guard = isFuncDeclaration.CreateGuard(); !VisitChild(iter, 0)) {
		return false;
	}
	for (auto n : functionArgsID) {
		functionArgs.emplace_back(localVars.at(n).arg);
	}
	if (const auto type = ChildType(iter, 1); !type.has_value()) {
		return false;
	} else {
		return VisitAndReturn(type.value());
	}
}

bool TypeAuditor::ViFunctionCall(Cursor iter) {
	const auto& funcName = iter(0).data.ToText();
	const auto* funcType = env.context.TypeFor(funcName);
	if (funcType == nullptr) {
		OnError(SemanticEID::globalNotTyped, iter->pos.start, funcName);
		return false;
	}
	const auto substitutes = CheckFuncArguments(iter, funcName);
	if (!substitutes.has_value()) {
		return false;
	}
	if (!std::holds_alternative<Typification>(*funcType)) {
		return VisitAndReturn(*funcType);
	} else {
		Typification fixedType = std::get<Typification>(*funcType);
		MangleRadicals(funcName, fixedType);
		if (!empty(substitutes.value())) {
			fixedType.SubstituteBase(substitutes.value());
		}
		return VisitAndReturn(fixedType);
	}
}

std::optional<Typification::Substitutes> TypeAuditor::CheckFuncArguments(Cursor iter, const std::string& funcName) {
	const auto* args = env.context.FunctionArgsFor(funcName);
	if (args == nullptr) {
		OnError(SemanticEID::globalFuncMissing, iter(0).pos.start, iter(0).data.ToText());
		return std::nullopt;
	}
	if (const auto argCount = static_cast<Index>(ssize(*args)); argCount != iter.ChildrenCount() - 1) {
		OnError(SemanticEID::invalidArgsArtity, iter(1).pos.start,
						{ std::to_string(iter.ChildrenCount()), std::to_string(argCount) });
		return std::nullopt;
	}

	Typification::Substitutes substitutes{};
	for (Index child = 1; child < iter.ChildrenCount(); ++child) {
		const auto childType = ChildType(iter, child);
		if (!childType.has_value() || !std::holds_alternative<Typification>(childType.value())) {
			return std::nullopt;
		}
		Typification argType = args->at(static_cast<size_t>(child) - 1).type;
		MangleRadicals(funcName, argType);
		if (!env.CompareTemplated(substitutes, argType, std::get<Typification>(childType.value()))) {
			OnError(SemanticEID::invalidArgumentType, iter(child).pos.start, argType, childType.value());
			return std::nullopt;
		}
	}
	return substitutes;
}

bool TypeAuditor::ViGlobal(Cursor iter) {
	const auto& globalName = iter->data.ToText();
	if (iter->id == TokenID::ID_RADICAL) {
		if (isFuncDeclaration) {
			return VisitAndReturn(Typification(globalName).ApplyBool());
		} else {
			OnError(SemanticEID::radicalUsage, iter->pos.start, globalName);
			return false;
		}
	} else if (env.context.FunctionArgsFor(globalName) != nullptr) {
		OnError(SemanticEID::globalFuncWithoutArgs, iter->pos.start, globalName);
		return false;
	} else if (const auto* type = env.context.TypeFor(globalName); type == nullptr) {
		OnError(SemanticEID::globalNotTyped, iter->pos.start, globalName);
		return false;
	} else {
		return VisitAndReturn(*type);
	}
}

bool TypeAuditor::ViLocal(Cursor iter) {
	const auto& localName = iter->data.ToText();
	if (isLocalDeclaration || isArgDeclaration) {
		return AddLocalVar(localName, std::get<Typification>(currentType), iter->pos.start);
	} else {
		const auto* local = GetLocalTypification(localName, iter->pos.start);
		if (local != nullptr) {
			return VisitAndReturn(*local);
		} else {
			return false;
		}
	}
}

bool TypeAuditor::ViEmptySet(Cursor iter) {
	OnError(SemanticEID::emptySetUsage, iter->pos.start);
	return false;
}

bool TypeAuditor::ViLocalBind(Cursor iter) {
	assert(isLocalDeclaration || isFuncDeclaration);
	const Typification type = std::get<Typification>(currentType);
	if (!type.IsTuple() || type.T().Arity() != iter.ChildrenCount()) {
		OnError(SemanticEID::ivalidBinding, iter(0).pos.start);
		return false;
	}
	for (Index child = 0; child < iter.ChildrenCount(); ++child) {
		currentType = type.T().Component(static_cast<Index>(child + Typification::PR_START));
		if (!VisitChild(iter, child)) {
			return false;
		}
	}
	return VisitAndReturn(type);
}

bool TypeAuditor::ViArgument(Cursor iter) {
	const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
	if (!domain.has_value()) {
		return false;
	} else {
		const auto guard{ isArgDeclaration.CreateGuard() };
		currentType = domain.value();
		return VisitChild(iter, 0) && VisitAndReturn(LogicT{});
	}
}

bool TypeAuditor::ViCard(Cursor iter) {
	return ChildTypeDebool(iter, 0, SemanticEID::invalidCard).has_value()
		&& VisitAndReturn(Typification::Integer());
}

bool TypeAuditor::ViArithmetic(Cursor iter) {
	if (const auto test1 = ChildType(iter, 0); !test1.has_value()) {
		return false;
	} else if (const auto& type1 = std::get<Typification>(test1.value()); 
						 !env.IsArithmetic(type1)) {
		OnError(SemanticEID::arithmeticNotSupported, iter(0).pos.start, type1.ToString());
		return false;
	} else if (const auto test2 = ChildType(iter, 1); !test2.has_value()) {
		return false;
	} else if (const auto& type2 = std::get<Typification>(test2.value()); 
						 !env.IsArithmetic(type2)) {
		OnError(SemanticEID::arithmeticNotSupported, iter(1).pos.start, type2.ToString());
		return false;
	} else if (const auto result = env.Merge(type1, type2); !result.has_value()) {
		OnError(SemanticEID::typesNotCompatible, iter(1).pos.start, type1, type2);
		return false;
	} else {
		return VisitAndReturn(result.value());
	}
}

bool TypeAuditor::ViOrdering(Cursor iter) {
	if(const auto test1 = ChildType(iter, 0); !test1.has_value()) {
		return false;
	} else if (const auto& type1 = std::get<Typification>(test1.value());
						 !env.IsOrdered(type1)) {
		OnError(SemanticEID::orderingNotSupported, iter(0).pos.start, type1.ToString());
		return false;
	} else if (const auto test2 = ChildType(iter, 1); !test2.has_value()) {
		return false;
	} else if (const auto& type2 = std::get<Typification>(test2.value());
						 !env.IsOrdered(type2)) {
		OnError(SemanticEID::orderingNotSupported, iter(1).pos.start, type2.ToString());
		return false;
	} else if (!env.AreCompatible(type1, type2)) {
		OnError(SemanticEID::typesNotCompatible, iter(1).pos.start, type1, type2);
		return false;
	} else {
		return VisitAndReturn(LogicT{});
	}
}

bool TypeAuditor::ViQuantifier(Cursor iter) {
	StartScope();
	if (const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation); !domain.has_value()) {
		return false;
	} else if (!VisitChildDeclaration(iter, 0, domain.value())) {
		return false;
	} else if (!VisitChild(iter, 2)) {
		return false;
	} else {
		EndScope(iter->pos.start);
		return VisitAndReturn(LogicT{});
	}
}

bool TypeAuditor::ViEquals(Cursor iter) {
	if (iter(1).id == TokenID::LIT_EMPTYSET) {
		const auto type = ChildTypeDebool(iter, 0, SemanticEID::invalidEqualsEmpty);
		return type.has_value() && VisitAndReturn(LogicT{});
	} else {
		const auto test1 = ChildType(iter, 0);
		if (!test1.has_value()) {
			return false;
		}
		const auto& type1 = std::get<Typification>(test1.value());

		const auto test2 = ChildType(iter, 1);
		if (!test2.has_value()) {
			return false;
		}
		const auto& type2 = std::get<Typification>(test2.value());

		if (!env.AreCompatible(type1, type2)) {
			OnError(SemanticEID::typesNotCompatible, iter(1).pos.start, type1, type2);
			return false;
		} else {
			return VisitAndReturn(LogicT{});
		}
	}
}

bool TypeAuditor::ViTypedPredicate(Cursor iter) {
	auto type2 = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
	if (!type2.has_value()) {
		return false;
	}
	const auto isSubset = IsSubset(iter->id);
	if (isSubset) {
		type2.value().ApplyBool();
	}
	const auto type1 = ChildType(iter, 0);
	if (!type1.has_value()) {
		return false;
	}

	if (!env.AreCompatible(type1.value(), type2.value())) {
		if (isSubset) {
			OnError(SemanticEID::typesNotEqual, iter(1).pos.start, type1.value(), type2.value());
		} else {
			OnError(SemanticEID::invalidElementPredicat, iter(1).pos.start,
							{ ToString(type1.value()), iter->ToString(), type2->Bool().ToString() });
		}
		return false;
	} else {
		return VisitAndReturn(LogicT{});
	}
}

bool TypeAuditor::ViDeclarative(Cursor iter) {
	StartScope();
	if (auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation); !domain.has_value()) {
		return false;
	} else if (!VisitChildDeclaration(iter, 0, domain.value())) {
		return false;
	} else if (!VisitChild(iter, 2)) {
		return false;
	} else {
		EndScope(iter->pos.start);
		return VisitAndReturn(domain->ApplyBool());
	}
}

bool TypeAuditor::ViImperative(Cursor iter) {
	StartScope();
	for (Index child = 1; child < iter.ChildrenCount(); ++child) {
		if (!VisitChild(iter, child)) {
			return false;
		}
	}
	if (auto type = ChildType(iter, 0); !type.has_value()) {
		return false;
	} else {
		EndScope(iter->pos.start);
		return VisitAndReturn(std::get<Typification>(type.value()).Bool());
	}
}

bool TypeAuditor::ViImpDeclare(Cursor iter) {
	const auto domain = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
	return domain.has_value() 
		&& VisitChildDeclaration(iter, 0, domain.value());
}

bool TypeAuditor::ViImpAssign(Cursor iter) {
	const auto domain = ChildType(iter, 1);
	return domain.has_value()
		&& VisitChildDeclaration(iter, 0, std::get<Typification>(domain.value()));
}

bool TypeAuditor::ViRecursion(Cursor iter) {
	StartScope();
	auto initType = ChildType(iter, 1);
	if (!initType.has_value()) {
		return false;
	} else if (!VisitChildDeclaration(iter, 0, std::get<Typification>(initType.value()))) {
		return false;
	}

	Index iterationIndex{ 2 };
	if (iter->id == TokenID::NT_RECURSIVE_FULL) {
		iterationIndex = 3;
		if (!VisitChild(iter, 2)) {
			return false;
		}
	}

	const auto iterationType = ChildType(iter, iterationIndex);
	if (!iterationType.has_value()) {
		return false;
	} else if (!env.AreCompatible(iterationType.value(), initType.value())) {
		OnError(SemanticEID::typesNotEqual, iter(iterationIndex).pos.start, 
						iterationType.value(), initType.value());
		return false;
	}
	EndScope(iter->pos.start);
	return VisitAndReturn(iterationType.value());
}

bool TypeAuditor::ViDecart(Cursor iter) {
	std::vector<Typification> factors{};
	for (Index child = 0; child < iter.ChildrenCount(); ++child) {
		const auto type = ChildTypeDebool(iter, child, SemanticEID::invalidDecart);
		if (!type.has_value()) {
			return false;
		} else {
			factors.emplace_back(type.value());
		}
	}
	return VisitAndReturn(Typification::Tuple(factors).ApplyBool());
}

bool TypeAuditor::ViBoolean(Cursor iter) {
	if (auto type = ChildTypeDebool(iter, 0, SemanticEID::invalidBoolean); !type.has_value()) {
		return false;
	} else {
		return VisitAndReturn(type->ApplyBool().ApplyBool());
	}
}

bool TypeAuditor::ViTuple(Cursor iter) {
	std::vector<Typification> components{};
	for (Index child = 0; child < iter.ChildrenCount(); ++child) {
		if (const auto type = ChildType(iter, child); !type.has_value()) {
			return false;
		} else {
			components.emplace_back(std::get<Typification>(type.value()));
		}
	}
	return VisitAndReturn(Typification::Tuple(components));
}

bool TypeAuditor::ViSetEnum(Cursor iter) {
	auto test = ChildType(iter, 0);
	if (!test.has_value()) {
		return false;
	}
	auto& type = std::get<Typification>(test.value());
	for (Index child = 1; child < iter.ChildrenCount(); ++child) {
		if (auto childType = ChildType(iter, child); !childType.has_value()) {
			return false;
		} else if (auto merge = env.Merge(type, std::get<Typification>(childType.value())); 
							 !merge.has_value()) {
			OnError(SemanticEID::invalidEnumeration, iter(child).pos.start, type, childType.value());
			return false;
		} else {
			type = std::move(merge.value());
		}
	}
	return VisitAndReturn(type.Bool());
}

bool TypeAuditor::ViDebool(Cursor iter) {
	if (auto type = ChildTypeDebool(iter, 0, SemanticEID::invalidDebool); type.has_value()) {
		return VisitAndReturn(type.value());
	} else {
		return false;
	}
}

bool TypeAuditor::ViTypedBinary(Cursor iter) {
	auto type1 = ChildTypeDebool(iter, 0, SemanticEID::invalidTypeOperation);
	if (!type1.has_value()) {
		return false;
	}
	auto type2 = ChildTypeDebool(iter, 1, SemanticEID::invalidTypeOperation);
	if (!type2.has_value()) {
		return false;
	}
	if (const auto result = env.Merge(type1.value(), type2.value()); !result.has_value()) {
		OnError(SemanticEID::typesNotEqual, iter(1).pos.start, type1->Bool(), type2->Bool());
		return false;
	} else {
		return VisitAndReturn(result.value().Bool());
	}
}

bool TypeAuditor::ViProjectSet(Cursor iter) {
	// T(Pri(a)) = B(Pi(D(T(a))))
	const auto baseType = ChildTypeDebool(iter, 0, SemanticEID::invalidProjectionSet);
	if (!baseType.has_value()) {
		return false;
	}
	if (!baseType->IsTuple()) {
		OnError(SemanticEID::invalidProjectionSet, iter(0).pos.start, { iter->ToString(), baseType->ToString() });
		return false;
	}

	const auto& indicies = iter->data.ToTuple();
	std::vector<Typification> components{};
	components.reserve(size(indicies));
	for (const auto index : indicies) {
		if (!baseType->T().TestIndex(index)) {
			OnError(SemanticEID::invalidProjectionSet, iter(0).pos.start, { iter->ToString(), baseType->ToString() });
			return false;
		} else {
			components.emplace_back(baseType->T().Component(index));
		}
	}
	currentType = Typification::Tuple(components).ApplyBool();
	return true;
}

bool TypeAuditor::ViProjectTuple(Cursor iter) {
	// T(pri(a)) = Pi(T(a))
	const auto baseType = ChildType(iter, 0);
	if (!baseType.has_value()) {
		return false;
	}
	const auto& base = std::get<Typification>(baseType.value());
	if (!base.IsTuple()) {
		OnError(SemanticEID::invalidProjectionTuple, iter(0).pos.start, { iter->ToString(), base.ToString() });
		return false;
	}
	
	const auto& indicies = iter->data.ToTuple();
	std::vector<Typification> components{};
	components.reserve(size(indicies));
	for (const auto index : indicies) {
		if (!base.T().TestIndex(index)) {
			OnError(SemanticEID::invalidProjectionTuple, iter(0).pos.start, { iter->ToString(), base.ToString() });
			return false;
		} else {
			components.emplace_back(base.T().Component(index));
		}
	}
	return VisitAndReturn(Typification::Tuple(components));
}

bool TypeAuditor::ViFilter(Cursor iter) {
	const auto baseType = ChildType(iter, static_cast<Index>(iter.ChildrenCount() - 1));
	if (!baseType.has_value()) {
		return false;
	} 
	const auto& base = std::get<Typification>(baseType.value());
	if (!base.IsCollection() || !base.B().Base().IsTuple()) {
		OnError(SemanticEID::invalidFilterArgumentType,
						iter(static_cast<Index>(iter.ChildrenCount() - 1)).pos.start,
						{ iter->ToString(), base.ToString() });
		return false;
	}

	const auto& indicies = iter->data.ToTuple();
	if (ssize(indicies) + 1 != iter.ChildrenCount()) {
		OnError(SemanticEID::invalidFilterArity, iter->pos.start);
		return false;
	}
	Index child{ 0 };
	for (const auto index : indicies) {
		if (!base.B().Base().T().TestIndex(index)) {
			OnError(SemanticEID::invalidFilterArgumentType, iter(static_cast<Index>(iter.ChildrenCount() - 1)).pos.start, 
							{ iter->ToString(), base.ToString() });
			return false;
		} 
		const auto param = ChildType(iter, child);
		if(!param.has_value()) {
			return false;
		}
		const auto& paramType = std::get<Typification>(param.value());
		if (!paramType.IsCollection() || 
				!env.AreCompatible(base.B().Base().T().Component(index), paramType.B().Base())) {
			OnError(SemanticEID::typesNotEqual, iter(child).pos.start,
							base.B().Base().T().Component(index).Bool(), paramType);
			return false;
		}
		++child;
	}
	return VisitAndReturn(baseType.value());
}

bool TypeAuditor::ViReduce(Cursor iter) {
	// T(red(a)) = B(DD(T(a)))
	const auto baseType = ChildType(iter, 0);
	if (!baseType.has_value()) {
		return false;
	}
	const auto& base = std::get<Typification>(baseType.value());
	if (!base.IsCollection() || !base.B().Base().IsCollection()) {
		OnError(SemanticEID::invalidReduce, iter(0).pos.start + 1, base.ToString());
		return false;
	} else {
		return VisitAndReturn(base.B().Base());
	}
}

bool TypeAuditor::VisitAndReturn(ExpressionType type) noexcept {
	currentType = std::move(type);
	return true;
}

bool TypeAuditor::VisitAllAndReturn(Cursor iter, const ExpressionType& type) {
	return VisitAllChildren(iter) && VisitAndReturn(type);
}

bool TypeAuditor::VisitChildDeclaration(const Cursor& iter, const Index index, const Typification& domain) {
	currentType = domain;
	if (const auto guard = isLocalDeclaration.CreateGuard(); 
			!VisitChild(iter, index)) {
		return false;
	}
	return VisitAndReturn(LogicT{});
}

std::optional<ExpressionType> TypeAuditor::ChildType(Cursor iter, const Index index) {
	assert(iter.ChildrenCount() > index);
	
	ExpressionType result = currentType;
	iter.MoveToChild(index);
	const auto visitSuccess = iter.DispatchVisit(*this);
	std::swap(result, currentType);
	if (!visitSuccess) {
		return std::nullopt;
	} else {
		return result;
	}
}

std::optional<Typification> TypeAuditor::ChildTypeDebool(Cursor iter, const Index index, const SemanticEID eid) {
	const auto result = ChildType(iter, index);
	if (!result.has_value() || !std::holds_alternative<Typification>(result.value())) {
		return std::nullopt;
	}
	if (!std::get<Typification>(result.value()).IsCollection()) {
		OnError(eid, iter(index).pos.start, ToString(result.value()));
		return std::nullopt;
	} else {
		return std::get<Typification>(result.value()).B().Base();
	}
}

const Typification* TypeAuditor::GetLocalTypification(const std::string& name, const StrPos pos) {
	const auto varIter = std::find_if(begin(localVars), end(localVars),
																		[&](const auto& data) noexcept { return data.arg.name == name; });
	if (varIter == end(localVars)) {
		if (!isArgDeclaration) {
			OnError(SemanticEID::localUndeclared, pos, name);
		}
		return nullptr;
	} else if (!varIter->enabled) {
		OnError(SemanticEID::localOutOfScope, pos, name);
		return nullptr;
	} else {
		++varIter->useCount;
		return &varIter->arg.type;
	}
}

void TypeAuditor::StartScope() noexcept {
	for (auto& var : localVars) {
		++var.level;
	}
}

void TypeAuditor::EndScope(const StrPos pos) {
	for (auto& var : localVars) {
		--var.level;
		if (var.level < 0 && var.enabled) {
			var.enabled = false;
			if (var.useCount == 0) {
				OnError(SemanticEID::localNotUsed, pos, var.arg.name);
			}
		}
	}
}

bool TypeAuditor::AddLocalVar(const std::string& name, const Typification& type, const StrPos pos) {
	auto varIter = std::find_if(begin(localVars), end(localVars),
															[&](const auto& data) noexcept { return data.arg.name == name; });
	if (varIter != end(localVars)) {
		if (varIter->enabled) {
			OnError(SemanticEID::localShadowing, pos, name);
			return false;
		} else {
			OnError(SemanticEID::localDoubleDeclare, pos, name);
			varIter->arg.type = type;
			varIter->enabled = true;
			varIter->level = 0;
			varIter->useCount = 0;
			return true;
		}
	} else {
		localVars.emplace_back(LocalData{ TypedID{name, type}, 0, 0, true });
		if (isArgDeclaration) {
			functionArgsID.emplace_back(localVars.size() - 1U);
		}
		return true;
	}
}

} // namespace ccl::rslang