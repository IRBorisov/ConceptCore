#pragma once

#include "ccl/lang/TextProcessor.h"

namespace tccl {

using ccl::lang::Morphology;
using ccl::lang::TextProcessor;

struct MockTextProcessor : TextProcessor {
  enum CallType : uint8_t { 
    INFLECT,
    DEPENDANT
  };

  struct CallEntry {
    CallType type;
    std::string targetText;
    std::optional<Morphology> form{};
    std::optional<std::string> mainText{};
  };

  using InflectDB = std::vector<std::pair<std::pair<std::string, Morphology>, std::string>>;
  using DependantDB = std::vector<std::pair<std::pair<std::string, std::string>, std::string>>;

  mutable std::vector<CallEntry> log{};
  mutable InflectDB inflections{};
  mutable DependantDB substitutes{};

  void ClearLog() noexcept {
    log.clear();
  }

  void ClearAll() noexcept {
    inflections.clear();
    substitutes.clear();
    log.clear();
  }

  void AddInflection(std::string target,Morphology morpho, std::string result) {
    inflections.emplace_back(std::pair<std::string, Morphology>(std::move(target), std::move(morpho)), std::move(result));
  }

  void AddCollaboration(std::string dependant, std::string main, std::string result) {
    substitutes.emplace_back(std::pair<std::string, std::string>(std::move(dependant), std::move(main)), std::move(result));
  }

  [[nodiscard]] std::string Inflect(const std::string& target, const Morphology& form) const override {
    log.emplace_back(CallEntry{INFLECT, target, form, {}});
    const auto itr =
      std::find_if(begin(inflections), end(inflections),
        [&target, &form](const auto& v) { return v.first.first == target && v.first.second == form; });
    if (itr != end(inflections)) {
      return itr->second;
    } else {
      return target;
    }
  }

  [[nodiscard]] std::string InflectDependant(const std::string& dependant, const std::string& main) const override {
    log.emplace_back(CallEntry{DEPENDANT, dependant, std::nullopt, main});
    const auto itr =
      std::find_if(begin(substitutes), end(substitutes),
        [&dependant, &main](const auto& v) { return v.first.first == dependant && v.first.second == main; });
    if (itr != end(substitutes)) {
      return itr->second;
    } else {
      return dependant;
    }
  }
  //std::string ApplyForm(const std::string& nominal, ccl::lang::Termform termForm) const override {
  //  callLog.emplace_back(CallEntry{ APPLYFORM, nominal, termForm });
  //  if (std::empty(nominal)) {
  //    return nominal;
  //  } else {
  //    return "RESOLVED_TERM";
  //  }
  //}
  //std::string ApplyFormGender(const std::string& word,
  //                             ccl::lang::Termform form,
  //                             ccl::lang::Gender gender) const override {
  //  callLog.emplace_back(CallEntry{ APPLYGENDER, word, form, gender });
  //  if (std::empty(word)) {
  //    return word;
  //  } else {
  //    return "RESOLVED_COMPANION";
  //  }
  //}
};

} // namespace tccl 
