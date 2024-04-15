#pragma once

#include "ccl/Entity.hpp"
#include "ccl/cclMeta.hpp"
#include "ccl/Source.hpp"

namespace ccl::ops {

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 26440 ) // Note: do not warn if default implementation might be noexcept
#endif

//! Operation status enumeration
enum class Status : uint32_t {
  undefined = 2,
  defined = 3,
  done = 4,
  outdated = 5,  // Требуется пересинтез
  broken = 7,    // Требуется переопределение операции
};

//! Operation type enumeration
enum class Type : uint32_t {
  tba = 0,
  rsMerge = 4, // Слияние двух РС-форм в одну
  rsSynt = 8,  // Обобщенный синтез
};

static constexpr bool HasOptions(Type type) noexcept { return type == Type::rsSynt; }

//! Abstract options of operation
class Options {
public:
  virtual ~Options() noexcept = default;

protected:
  Options() = default;
  Options(const Options&) = default;
  Options& operator=(const Options&) = default;
  Options(Options&&) noexcept = default;
  Options& operator=(Options&&) noexcept = default;

public:
  [[nodiscard]] virtual bool IsEqualTo(const Options& /*opt2*/) const = 0;
};

//! Abstract translation data
using TranslationData = std::vector<EntityTranslation>;

using Args = std::vector<const src::DataStream*>;
using ArgsInfo = std::vector<src::DataType>;

//! Operation signature
struct Signature {
  src::DataType result{};
  ArgsInfo args{};
};

//! Operation call
struct Call {
  Type type{ Type::tba };
  const Options* params{ nullptr };
  Args args{};

  [[nodiscard]] ArgsInfo Info() const {
    auto result = ArgsInfo{};
    for (const auto& arg : args) {
      result.emplace_back(arg == nullptr ? src::DataType::tba : arg->Type());
    }
    return result;
  }
};

//! Operation result
struct Result {
  meta::UniqueCPPtr<src::DataStream> value{ nullptr };
  meta::UniqueCPPtr<TranslationData> translation{ nullptr };
};

#ifdef _MSC_VER
  #pragma warning( pop )
#endif

} // namespace ccl::ops