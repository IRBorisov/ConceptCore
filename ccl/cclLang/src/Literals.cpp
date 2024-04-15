#include "ccl/lang/Literals.h"

namespace ccl::lang {

Morphology operator""_form(const char* input, size_t /*size*/) {
  return Morphology(input);
}

Reference operator""_ref(const char* input, size_t size) {
  return Reference::Parse(std::string_view(input, size));
}

} // namespace ccl::lang