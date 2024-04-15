#pragma once

#include <cstdint>
#include <string>
#include <set>

#include "ccl/cclTypes.hpp"

//! Namespace for russian language specification
namespace ccl::lang {

//! All values of morphology attributes
enum class Grammem : uint8_t {
  invalid = 0,
  NOUN, NPRO, INFN, VERB, ADJF, ADJS, PRTF, PRTS, ADVB, GRND, COMP, PRED, NUMR,
  CONJ, INTJ, PRCL, PREP, PNCT,
  pres, past, futr,
  per1, per2, per3,
  sing, plur,
  masc, femn, neut,
  nomn, gent, datv, ablt, accs, loct
  
};

namespace detail {
static constexpr size_t GRAMMEM_LENGTH = 4U;
static constexpr std::array<std::string_view, 36> TAG_NAMES = {
    "UNKN",
    "NOUN", "NPRO", "INFN", "VERB", "ADJF", "ADJS",
    "PRTF", "PRTS", "ADVB", "GRND", "COMP", "PRED",
    "NUMR",  "CONJ",  "INTJ", "PRCL", "PREP", "PNCT",
    "pres", "past", "futr",
    "1per", "2per", "3per",
    "sing", "plur",
    "masc", "femn", "neut",
    "nomn", "gent", "datv", "ablt",  "accs", "loct"
};

static constexpr size_t GRAMMEM_COUNT = 36U;
using MapType = StaticMap<std::string_view, Grammem, GRAMMEM_COUNT>;
using MapValue = MapType::DataType::value_type;
static constexpr MapType TAG_MAP{
  MapType::DataType({
    MapValue{"UNKN", Grammem::invalid},
    MapValue{"NOUN", Grammem::NOUN}, MapValue{"NPRO", Grammem::NPRO}, MapValue{"INFN", Grammem::INFN}, 
    MapValue{"VERB", Grammem::VERB}, MapValue{"ADJF", Grammem::ADJF}, MapValue{"ADJS", Grammem::ADJS},
    MapValue{"PRTF", Grammem::PRTF}, MapValue{"PRTS", Grammem::PRTS}, MapValue{"ADVB", Grammem::ADVB}, 
    MapValue{"GRND", Grammem::GRND}, MapValue{"COMP", Grammem::COMP}, MapValue{"PRED", Grammem::PRED},
    MapValue{"NUMR", Grammem::NUMR}, MapValue{"CONJ", Grammem::CONJ}, MapValue{"INTJ", Grammem::INTJ},
    MapValue{"PRCL", Grammem::PRCL}, MapValue{"PREP", Grammem::PREP}, MapValue{"PNCT", Grammem::PNCT},
    MapValue{"1per", Grammem::per1}, MapValue{"2per", Grammem::per2}, MapValue{"3per", Grammem::per3},
    MapValue{"pres", Grammem::pres}, MapValue{"past", Grammem::past}, MapValue{"futr", Grammem::futr},
    MapValue{"sing", Grammem::sing}, MapValue{"plur", Grammem::plur},
    MapValue{"masc", Grammem::masc}, MapValue{"femn", Grammem::femn}, MapValue{"neut", Grammem::neut},
    MapValue{"nomn", Grammem::nomn}, MapValue{"gent", Grammem::gent}, MapValue{"datv", Grammem::datv}, 
    MapValue{"ablt", Grammem::ablt}, MapValue{"accs", Grammem::accs}, MapValue{"loct", Grammem::loct}
  })
};

} // namespace detail

[[nodiscard]] inline Grammem Str2Grammem(std::string_view text) {
  if (text.length() != detail::GRAMMEM_LENGTH || !detail::TAG_MAP.ContainsKey(text)) {
    return Grammem::invalid;
  } else {
    return detail::TAG_MAP.AtKey(text);
  }
}

[[nodiscard]] inline std::string_view Grammem2Str(Grammem gram) noexcept {
  return detail::TAG_NAMES.at(static_cast<size_t>(gram));
}

//! Morphology parameters for a word or collation of words
struct Morphology {
  std::set<Grammem> tags{};

  Morphology() = default;
  explicit Morphology(std::initializer_list<Grammem> init) noexcept // NOLINT
    :  tags{ init } {}
  explicit Morphology(std::string_view tagsText);
  explicit Morphology(const std::vector<std::string_view>& tags_);

  [[nodiscard]] bool operator==(const Morphology& rval) const noexcept;
  [[nodiscard]] bool operator!=(const Morphology& rval) const noexcept;

  [[nodiscard]] size_t size() const noexcept;
  [[nodiscard]] bool empty() const noexcept;
  [[nodiscard]] bool Contains(Grammem gram) const noexcept;
  
  [[nodiscard]] Grammem GetPOS() const;
  [[nodiscard]] Grammem GetCase() const;
  [[nodiscard]] Grammem GetNumber() const;
  [[nodiscard]] Grammem GetGender() const;
  [[nodiscard]] Grammem GetTense() const;
  [[nodiscard]] Grammem GetPerson() const;

  [[nodiscard]] std::string ToString() const noexcept;
};

//! Часть речи
static constexpr std::array<Grammem, 18> ALL_POS = {
    Grammem::NOUN,  // существительное          | хомяк
    Grammem::NPRO,  // местоимение              | он
    Grammem::INFN,  // глагол (инфинитив)        | говорить
    Grammem::VERB,  // глагол (личная форма)    | говорил
    Grammem::ADJF,  // прилагательное  (полное)  | хороший
    Grammem::ADJS,  // прилагательное (краткое)  | хорош
    Grammem::PRTF,  // причастие (полное)        | прочитавший, прочитанная
    Grammem::PRTS,  // причастие (краткое)      | прочитана
    Grammem::ADVB,  // наречие                  | далеко
    Grammem::GRND,  // деепричастие              | прочитав
    Grammem::COMP,  // компаратив                | лучше, получше, выше
    Grammem::PRED,  // предикатив                | некогда
    Grammem::NUMR,  // числительное              | три
    Grammem::CONJ,  // союз                      | или
    Grammem::INTJ,  // междометие                | ой
    Grammem::PRCL,  // частица                  | бы, же
    Grammem::PREP,  // предлог                  | в
    Grammem::PNCT    // знак пунктуации
};

//! Падеж
static constexpr std::array<Grammem, 6> ALL_CASES = {
  Grammem::nomn,  // именительный    |  Кто?, Что?    |  (хомяк) ест
  Grammem::gent,  // родительный    |  Кого? Чего?    |  у нас нет (хомяка)
  Grammem::datv,  // дательный      |  Кому? Чему?    |  сказать (хомяку) спасибо
  Grammem::ablt,  // винительный    |  Кого? Что?    |  тигр ест (хомяка)
  Grammem::accs,  // творительный    |  Кем? Чем?      |  зерно съедено (хомяком)
  Grammem::loct    // предложный      |  О ком? О чём?  |  хомяка несут в (коробке)
};

//! Число
static constexpr std::array<Grammem, 2> ALL_NUMBERS = {
  Grammem::sing,  // единственное
  Grammem::plur    // множественное
};

//! Род
static constexpr std::array<Grammem, 3> ALL_GENDERS = {
  Grammem::masc,  // мужской
  Grammem::femn,  // женский
  Grammem::neut    // средний
};

//! Время
static constexpr std::array<Grammem, 3> ALL_TENSES = {
  Grammem::pres,  // настоящее
  Grammem::past,  // прошлое
  Grammem::futr    // будущее
};

//! Лицо
static constexpr std::array<Grammem, 3> ALL_PERSONS = {
  Grammem::per1,  // первое (я, мы)
  Grammem::per2,  // второе (ты, вы)
  Grammem::per3    // третье (он, она, оно, они)
};

//! Словоформы терминологической связки
static const std::array<Morphology, 12> TERM_FORMS = {
  // TODO: replace static object for constexpr. Need to figure out constexpr Morphology
  Morphology{ Grammem::sing, Grammem::nomn },
  Morphology{ Grammem::sing, Grammem::gent },
  Morphology{ Grammem::sing, Grammem::datv },
  Morphology{ Grammem::sing, Grammem::ablt },
  Morphology{ Grammem::sing, Grammem::accs },
  Morphology{ Grammem::sing, Grammem::loct },
  Morphology{ Grammem::plur, Grammem::nomn },
  Morphology{ Grammem::plur, Grammem::gent },
  Morphology{ Grammem::plur, Grammem::datv },
  Morphology{ Grammem::plur, Grammem::ablt },
  Morphology{ Grammem::plur, Grammem::accs },
  Morphology{ Grammem::plur, Grammem::loct },
};

} // namespace ccl::lang

namespace std {
template <>
struct hash<ccl::lang::Morphology> {
  uint32_t operator()(const ccl::lang::Morphology& form) const noexcept {
    using std::hash;
    uint32_t result = 0;
    for (const auto tag : form.tags) {
      result += static_cast<uint32_t>(tag);
    }
    return result + 0xFF * static_cast<uint32_t>(size(form.tags)); // NOLINT
  }
};
} // namespace std