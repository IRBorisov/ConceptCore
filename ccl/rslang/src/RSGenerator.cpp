#include "ccl/rslang/RSGenerator.h"

#include "GeneratorImplAST.h"

#include "ccl/Substitutes.hpp"
#include "ccl/rslang/RSExpr.h"
#include "ccl/rslang/Parser.h"

#include <string_view>

namespace ccl::rslang {

namespace {

using Description = Generator::StructureDescription;

[[nodiscard]] size_t FindArgumentSectionEnd(const std::string& input) {
  auto counter = 0;
  for (auto iter = UTF8Iterator(input); iter != UTF8End(input); ++iter) {
    if (*iter == '[') {
      ++counter;
    } else if (*iter == ']') {
      if (counter == 1) {
        return iter.BytePosition() + 1;
      } else {
        --counter;
      }
    }
  }
  return std::string::npos;
}

[[nodiscard]] std::string SubstituteInPrefix(
  const StrSubstitutes& substitutes,
  const std::vector<std::string>& args,
  const TypeContext& env
) {
  FunctionArguments argsTypes{};
  for (const auto& argName : args) {
    const auto* type = env.TypeFor(argName);
    if (type == nullptr) {
      return {};
    }
    const auto& newName = substitutes.at(argName);
    argsTypes.emplace_back(TypedID{ newName, std::get<Typification>(*type) });
  }
  return Generator::CreatePrefix(argsTypes);
}

class StructureGenerator final {
private:
  Description structure{};

public:
  [[nodiscard]] static Description GenerateFor(
    const std::string& cstName,
    const Typification& type
  );

private:
  void GenerateRecursive(const std::string& currentStr, const Typification& type);
  void GenerateReduce(const std::string& currentStr, const Typification& type);
  void GenerateProjection(const std::string& currentStr, const Typification& type);
  void CallRecursionForToken(
    const std::string& tokenText,
    const std::string& currentStr,
    const Typification& type
  );
};

Description StructureGenerator::GenerateFor(
  const std::string& cstName,
  const Typification& type
) {
  static StructureGenerator generator{};
  generator.structure.clear();
  generator.GenerateRecursive(cstName, type);
  return generator.structure;
}

void StructureGenerator::GenerateRecursive(const std::string& currentStr, const Typification& type) {
  if (!empty(currentStr) && type.IsCollection()) {
    const auto& elementType = type.B().Base();
    switch (elementType.Structure()) {
    case rslang::StructureType::basic: return;
    case rslang::StructureType::collection: GenerateReduce(currentStr, elementType); break;
    case rslang::StructureType::tuple: GenerateProjection(currentStr, elementType); break;
    }
  }
}

void StructureGenerator::GenerateReduce(const std::string& currentStr, const Typification& type) {
  CallRecursionForToken(rslang::Token::Str(rslang::TokenID::REDUCE), currentStr, type);
}

void StructureGenerator::GenerateProjection(const std::string& currentStr, const Typification& type) {
  for (Index i = 0; i < type.T().Arity(); ++i) {
    Typification component = type.T().Component(Typification::PR_START + i); // NOLINT
    const auto tokenText = rslang::Token::Str(rslang::TokenID::BIGPR) + std::to_string(Typification::PR_START + i);
    CallRecursionForToken(tokenText, currentStr, component.ApplyBool());
  }
}

void StructureGenerator::CallRecursionForToken(
  const std::string& tokenText,
  const std::string& currentStr,
  const Typification& type
) {
  std::string newStr = tokenText;
  newStr += '(';
  newStr += currentStr;
  newStr += ')';
  structure.emplace_back(newStr, type);
  GenerateRecursive(newStr, type);
}

[[nodiscard]] StrSubstitutes UniqueSubstitutes(
  const std::vector<std::string>& args,
  const std::unordered_set<std::string>& takenNames
) {
  static constexpr auto LOCAL_ID_BASE = 1;
  static const std::string_view LOCAL_NAME_BASE = "arg";
  StrSubstitutes substitutes{};
  auto id = LOCAL_ID_BASE;
  for (const auto& argName : args) {
    std::string localName{};
    do {
      localName = LOCAL_NAME_BASE;
      localName += std::to_string(id++);
    } while (takenNames.contains(localName));
    substitutes.insert({ argName, localName });
  }
  return substitutes;
}

} // namespace

std::string Generator::FromTree(const SyntaxTree& ast, const Syntax syntax) {
  return GeneratorImplAST::FromTree(ast, syntax);
}

std::string Generator::GlobalDefinition(std::string globalID, std::string expr, const bool isStruct) {
  std::string result = std::move(globalID);
  result += Token::Str(isStruct ? TokenID::PUNC_STRUCT : TokenID::PUNC_DEFINE, Syntax::MATH);
  result += expr;
  return result;
}

std::string Generator::CreatePrefix(const FunctionArguments& args) {
  std::string prefix{};
  prefix += '[';
  for (auto iter = begin(args); iter != end(args); ++iter) {
    if (iter != begin(args)) {
      prefix += R"(, )";
    }
    prefix += iter->name;
    prefix += Token::Str(TokenID::IN) + iter->type.ToString();
  }
  prefix += ']';
  return prefix;
}

std::string Generator::ExtractPrefix(const std::string& declaration) {
  const auto prefixEnd = FindArgumentSectionEnd(declaration);
  if (prefixEnd == std::string::npos) {
    return {};
  } else {
    return declaration.substr(0, prefixEnd);
  }
}

std::string Generator::FunctionFromExpr(
  const std::vector<std::string>& args, 
  const std::string& expression
) const {
  if (empty(args) || empty(expression)) {
    return expression;
  }
  
  const auto substitutes = UniqueSubstitutes(args, rslang::ExtractULocals(expression));
  std::string funcExpression = expression;
  SubstituteGlobals(funcExpression, substitutes);
  const auto funcPrefix = SubstituteInPrefix(substitutes, args, environment);
  if (std::empty(funcPrefix)) {
    return {};
  } else {
    return funcPrefix + " " + funcExpression;
  }
}

std::string Generator::CreateCall(const std::string& funcName, const std::vector<std::string>& args) {
  std::string argString{};
  if (!empty(args)) {
    argString = std::accumulate(next(begin(args)), end(args), *begin(args),
      [](std::string result, const std::string& arg) {
        result += ", ";
        result += arg;
        return result;
      }
    );
  }
  return funcName + '[' + argString + ']';
}

std::string Generator::TermFromFunction(
  const std::string& funcName, 
  const std::string& expression,
  const std::vector<std::string>& args
) const {
  const auto* argsTypes = environment.FunctionArgsFor(funcName);
  if (argsTypes == nullptr || ssize(*argsTypes) != ssize(args)) {
    return {};
  }
  const auto prefixEnd = FindArgumentSectionEnd(expression);
  if (prefixEnd == std::string::npos || prefixEnd == std::size(expression)) {
    return {};
  }

  std::string result = expression;
  result.erase(0, prefixEnd);
  if (result.at(0) == ' ') {
    result.erase(0, 1);
  }
  StrSubstitutes replMap{};
  for (auto i = 0U; i < size(args); ++i) {
    replMap.insert({ argsTypes->at(i).name, args.at(i) });
  }
  TranslateRS(result, TFFactory::FilterIdentifiers(), CreateTranslator(replMap));
  return result;
}

Generator::StructureDescription Generator::StructureFor(
  const std::string& globalName, 
  const Typification& type
) {
  return StructureGenerator::GenerateFor(globalName, type);
}

std::string ConvertTo(const std::string& input, const Syntax targetSyntax) {
  Parser parser{};
  const auto initialSyntax = targetSyntax == Syntax::MATH ? Syntax::ASCII : Syntax::MATH;
  if (!parser.Parse(input, initialSyntax)) {
    return input;
  } else {
    return Generator::FromTree(parser.AST(), targetSyntax);
  }
}

} // namespace ccl::rslang