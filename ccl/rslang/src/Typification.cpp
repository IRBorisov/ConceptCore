#include "ccl/rslang/Typification.h"

#include "ccl/cclMeta.hpp"
#include "ccl/rslang/RSToken.h"

#include <algorithm>

namespace ccl::rslang {

bool Typification::IsAnyType() const noexcept {
  return IsElement() && E().baseID == anyTypificationName;
}

Typification& Typification::ApplyBool() {
  state = EchelonBool(*this);
  return *this;
}

Typification Typification::Bool() const {
  Typification newType{ *this };
  return newType.ApplyBool();
}

Typification Typification::Tuple(std::vector<Typification> factors) {
  assert(size(factors) > 0);
  if (size(factors) == 1) {
    return *begin(factors);
  } else {
    return Typification(std::move(factors));
  }
}

const Typification& Typification::Integer() {
  static const Typification integers{ std::string{integerTypeName} };
  return integers;
}

const Typification& Typification::EmptySet() {
  static const auto empty = Typification{ std::string{anyTypificationName} }.Bool();
  return empty;
}

std::string Typification::ToString() const noexcept(false) {
  std::string result{};
  std::visit(meta::Overloads{ [&](const auto& val) { result = val.ToString(); }},
             state);
  return result;
}

void Typification::SubstituteBase(const Substitutes& substitutes) {
  switch (Structure()) {
  case StructureType::basic: {
    if (substitutes.contains(E().baseID)) {
      state = substitutes.at(E().baseID).state;
    }
    return;
  }
  case StructureType::collection: {
    B().Base().SubstituteBase(substitutes);
    return;
  }
  case StructureType::tuple: {
    for (auto index = PR_START; index < T().Arity() + PR_START; ++index) {
      T().Component(index).SubstituteBase(substitutes);
    }
    return;
  }
  }
}

EchelonTuple::EchelonTuple(std::vector<Typification> factors) noexcept
  : factors{ std::move(factors) } {}

const Typification& EchelonTuple::Component(const Index index) const {
  return factors.at(static_cast<size_t>(index) - Typification::PR_START);
}

Typification& EchelonTuple::Component(const Index index) {
  return factors.at(static_cast<size_t>(index) - Typification::PR_START);
}

Index EchelonTuple::Arity() const noexcept {
  return static_cast<Index>(ssize(factors));
}

bool EchelonTuple::TestIndex(const Index index) const noexcept {
  return index - Typification::PR_START < Arity() && index >= Typification::PR_START;
}

void EchelonTuple::AddComponent(const Typification& pr) {
  factors.emplace_back(pr);
}

bool EchelonTuple::operator==(const EchelonTuple& s2) const {
  if (Arity() != s2.Arity()) {
    return false;
  } else {
    for (auto i = 0U; i < size(factors); ++i) {
      if (factors.at(i) != s2.Component(static_cast<Index>(i + Typification::PR_START))) {
        return false;
      }
    }
    return true;
  }
}

std::string EchelonTuple::ToString() const {
  std::string res{};
  for (size_t i = 0U; i < size(factors); ++i) {
    if (i != 0) {
      res += Token::Str(TokenID::DECART);
    }
    if (factors.at(i).IsTuple()) {
      res += '(';
    }
    res += factors.at(i).ToString();
    if (factors.at(i).IsTuple()) {
      res += ')';
    }
  }
  return res;
}

EchelonBool::EchelonBool(const Typification& base) {
  boolBase = std::make_unique<Typification>(base);
}

EchelonBool::EchelonBool(const EchelonBool& s2) {
  *this = s2;
}

EchelonBool& EchelonBool::operator=(const EchelonBool& s2) {
  if (this != &s2) {
    boolBase = std::make_unique<Typification>(*s2.boolBase);
  }
  return *this;
}

const Typification& EchelonBool::Base() const noexcept {
  return *boolBase;
}

Typification& EchelonBool::Base() noexcept {
  return *boolBase;
}

bool EchelonBool::operator==(const EchelonBool& s2) const noexcept {
  return Base() == s2.Base();
}

std::string EchelonBool::ToString() const {
  if (const auto res = boolBase->ToString(); boolBase->IsCollection()) {
    return Token::Str(TokenID::BOOLEAN) + res;
  } else {
    return Token::Str(TokenID::BOOLEAN) + '(' + res + ')';
  }
}

} //namespace ccl::rslang