#pragma once

#include "ccl/Operation.hpp"
#include "ccl/Entity.hpp"

namespace ccl::ops {

//! Options for single entity equation
struct Equation {
  enum class Mode : uint8_t {
    keepHier = 1,
    keepDel = 2,
    createNew = 3,
  };

  Equation::Mode mode{ Equation::Mode::keepHier };
  std::string arg{};

  Equation() = default;
  explicit Equation(Equation::Mode mode, std::string arg) noexcept
    : mode{ mode }, arg{ std::move(arg) } {}
  
  bool operator==(const Equation& rhs) const noexcept {
    return mode == rhs.mode && arg == rhs.arg;
  }
  bool operator!=(const Equation& rhs) const noexcept {
    return mode != rhs.mode || arg != rhs.arg;
  }
};

//! Options for equation operation
class EquationOptions : public Options {
public:
  using Props = std::unordered_map<EntityUID, Equation>;

private:
  EntityTranslation  translation{};
  Props properties{};

public:
  EquationOptions() = default;

  explicit EquationOptions(EntityUID key, EntityUID val, const Equation& params = {});

public:
  using ConstIter = EntityTranslation::ConstIter;
  using size_type = EntityTranslation::size_type;

  [[nodiscard]] bool IsEqualTo(const Options& opt2) const override;

  [[nodiscard]] ConstIter begin() const noexcept { return std::begin(translation); }
  [[nodiscard]] ConstIter end() const noexcept { return std::end(translation); }
  [[nodiscard]] size_type size() const noexcept { return std::size(translation); }
  [[nodiscard]] bool empty() const noexcept { return std::empty(translation); }

  [[nodiscard]] const Equation& PropsFor(EntityUID key) const;

  [[nodiscard]] bool ContainsValue(EntityUID value) const;
  [[nodiscard]] bool ContainsKey(EntityUID key) const;

  void Insert(EntityUID key, EntityUID val, const Equation& params = {});
  void Erase(EntityUID key) noexcept;
  bool SwapKeyVal(EntityUID key);
  void SubstituteValues(const EntityTranslation& idSubstitutes);
};

} // namespace ccl::ops