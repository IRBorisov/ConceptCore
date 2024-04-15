#pragma once

#include "ccl/lang/LexicalTerm.h"
#include "ccl/lang/EntityTermContext.hpp"

class FakeContext : public ccl::lang::EntityTermContext {
  std::unordered_map<std::string, ccl::lang::LexicalTerm> terms{};

public:
  void Clear() noexcept {
    terms.clear();
  }
  void Insert(const std::string& entity, const ccl::lang::LexicalTerm& term) {
    terms.emplace(std::pair{ entity , term });
  }
  [[nodiscard]] bool Contains(const std::string& entity) const override {
    return terms.contains(entity);
  }
  [[nodiscard]] const ccl::lang::LexicalTerm* At(const std::string& entity) const override {
    if (!Contains(entity)) {
      return nullptr;
    } else {
      return &terms.at(entity);
    }
  }
};