#pragma once

#include "ccl/rslang/Structure.hpp"
#include "ccl/rslang/RSToken.h"

#include <memory>
#include <unordered_map>

namespace ccl::rslang {

class Typification;

//! BaseSet echelon
struct EchelonBase {
  std::string baseID{};

  explicit EchelonBase(std::string setID) noexcept
    : baseID{ std::move(setID) } {}

  [[nodiscard]] bool operator==(const EchelonBase& s2) const noexcept { return s2.baseID == baseID; }
  [[nodiscard]] bool operator!=(const EchelonBase& s2) const noexcept { return !(*this == s2); }

  [[nodiscard]] std::string ToString() const { return baseID; }
};

//! Tuple echelon
class EchelonTuple {
  std::vector<Typification> factors;

public:
  explicit EchelonTuple(std::vector<Typification> factors) noexcept;

public:
  [[nodiscard]] auto begin() const noexcept { return std::begin(factors); }
  [[nodiscard]] auto end() const noexcept { return std::end(factors); }

  [[nodiscard]] bool TestIndex(Index index) const noexcept;
  [[nodiscard]] const Typification& Component(Index index) const;
  [[nodiscard]] Typification& Component(Index index);
  [[nodiscard]] Index Arity() const noexcept;
  void AddComponent(const Typification& pr);

  [[nodiscard]] bool operator==(const EchelonTuple& s2) const;
  [[nodiscard]] bool operator!=(const EchelonTuple& s2) const { return !(*this == s2); }

  [[nodiscard]] std::string ToString() const;
};

//! Boolean echelon
class EchelonBool {
  std::unique_ptr<Typification> boolBase;

public:
  ~EchelonBool() noexcept = default;
  EchelonBool(const EchelonBool& s2);
  EchelonBool& operator=(const EchelonBool& s2);
  EchelonBool(EchelonBool&&) noexcept = default;
  EchelonBool& operator=(EchelonBool&&) noexcept = default;
  
  explicit EchelonBool(const Typification& base);

public:
  [[nodiscard]] const Typification& Base() const noexcept;
  [[nodiscard]] Typification& Base() noexcept;

  [[nodiscard]] bool operator==(const EchelonBool& s2) const noexcept;
  [[nodiscard]] bool operator!=(const EchelonBool& s2) const noexcept { return !(*this == s2); }

  [[nodiscard]] std::string ToString() const;
};

//! Typification
class Typification : public Structured<EchelonBase, EchelonTuple, EchelonBool> {
public:
  static constexpr Index PR_START = 1;
  static constexpr std::string_view integerTypeName = "Z";

private:
  using Structured::Structured;

public:
  explicit Typification(std::string baseTag) noexcept
    : Structured(EchelonBase(std::move(baseTag))) {}

  explicit Typification(std::vector<Typification> factors) noexcept
    : Structured(EchelonTuple(std::move(factors))) {}

  [[nodiscard]] static Typification Tuple(std::vector<Typification> factors);
  [[nodiscard]] static const Typification& Integer();

public:
  using Substitutes = std::unordered_map<std::string, Typification>;

  Typification& ApplyBool();
  [[nodiscard]] Typification Bool() const;

  [[nodiscard]] bool operator==(const Typification& t2) const noexcept { return state == t2.state; }
  [[nodiscard]] bool operator!=(const Typification& t2) const noexcept { return !(*this == t2); }

  [[nodiscard]] std::string ToString() const noexcept(false);

  void SubstituteBase(const Substitutes& substitutes);

  template<typename VisitFunc>
  void ConstVisit(VisitFunc& visitor) const {
    visitor(*this);
    switch (Structure()) {
    case StructureType::basic: 
      return;
    case StructureType::collection: {
      B().Base().ConstVisit(visitor);
      return;
    }
    case StructureType::tuple: {
      for (const auto& component : T()) {
        component.ConstVisit(visitor);
      }
      return;
    }
    }
  }
};

} // namespace ccl::rslang