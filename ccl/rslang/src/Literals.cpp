#include "ccl/rslang/Literals.h"

#include "ccl/rslang/RSGenerator.h"
#include "ccl/rslang/TypeAuditor.h"
#include "ccl/rslang/AsciiLexer.h"
#include "ccl/rslang/RSParser.h"

#include <unordered_map>

namespace ccl::rslang {

namespace {

//! Helper typeContext that echoes any global name as type
class EchoTypeEnvironment final : public TypeContext {
public:
  [[nodiscard]] const ExpressionType* TypeFor(const std::string& globalName) const override {
    if (!types.contains(globalName)) {
      types[globalName] = Typification(globalName).ApplyBool();
    }
    return &types.at(globalName);
  }
  [[nodiscard]] const FunctionArguments* FunctionArgsFor(const std::string& /*globalName*/) const noexcept override {
    return nullptr;
  }
  [[nodiscard]] std::optional<TypeTraits> TraitsFor(const Typification& /*type*/) const noexcept override {
    return {};
  }
  void Clear() noexcept {
    types.clear();
  }

private:
  mutable std::unordered_map<std::string, ExpressionType> types{};
};


} // unnamed namespace

Typification operator""_t(const char* input, const size_t /*size*/) {
  static detail::AsciiLexer lexer{};
  static detail::RSParser parser{};
  static EchoTypeEnvironment env{};
  static TypeAuditor analyser{ env };
  analyser.SetExepectTypification();

  env.Clear();
  if (!parser.Parse(lexer(input).Stream())) {
    assert(false);
    return Typification::Integer();
  } 
  if (!analyser.CheckType(parser.AST())) {
    assert(false);
    return Typification::Integer();
  } 

  const auto& result = analyser.GetType();
  if (!std::holds_alternative<Typification>(result)) {
    assert(false);
    return Typification::Integer();
  } 

  const auto& type = std::get<Typification>(result); 
  if (!type.IsCollection()) {
    assert(false);
    return Typification::Integer();
  }
  return type.B().Base();
}

std::string operator"" _rs(const char* input, const size_t size) {
  return ConvertTo(std::string(input, size), Syntax::MATH);
}

} // namespace ccl::rslang