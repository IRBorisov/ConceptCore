#include "ccl/rslang/AsciiLexer.h"

#ifdef _MSC_VER
  #pragma warning( push )
  #pragma warning( disable : 4127 4189 4244 4996 )
  #pragma warning( disable : 6011 6323 6330 )
  #pragma warning( disable : 26400 26401 26402 26408 26438 26440 26446 26447 )
  #pragma warning( disable : 26451 26456 26462 26474 26481 26482 26485 )
  #pragma warning( disable : 26492 26493 26494 26495 26496 26497 )
  #define OS_WIN
#endif

#ifdef __clang__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wsign-conversion"
  #pragma clang diagnostic ignored "-Wimplicit-int-conversion"
  #pragma clang diagnostic ignored "-Wimplicit-float-conversion"
  #pragma clang diagnostic ignored "-Wshadow"
  #pragma clang diagnostic ignored "-Wold-style-cast"
  #pragma clang diagnostic ignored "-Wdouble-promotion"
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wuseless-cast"
  #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
  #pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

#include "AsciiLexerImpl.hpp"

#ifdef _MSC_VER
  #pragma warning( pop )
  #undef OS_WIN
#endif

#ifdef __clang__
  #pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
  #pragma GCC diagnostic pop
#endif

namespace ccl::rslang::detail {

using asciilex::AsciiLexerImpl;

AsciiLexer::AsciiLexer(const std::string& input)
  : impl{ std::make_unique<AsciiLexerImpl>(input) } {}

AsciiLexer& AsciiLexer::operator()(const std::string& input) {
  SetInput(input);
  return *this;
}

AsciiLexer::AsciiLexer(ErrorReporter reporter)
  : LexerBase<AsciiLexer>{ std::move(reporter) }, impl{ std::make_unique<AsciiLexerImpl>() } {}

AsciiLexer::AsciiLexer() :
  impl{ std::make_unique<AsciiLexerImpl>() } {}

AsciiLexer::~AsciiLexer() = default;

void AsciiLexer::SetInput(const std::string& input) {
  impl->in(input);
}

TokenID AsciiLexer::DoLex() {
  return impl->lex();
}

std::string AsciiLexer::GetText() const {
  return impl->str();
}

StrRange AsciiLexer::Range() const {
  return impl->Range();
}

StrRange AsciiLexer::RangeInBytes() const {
  return Range();
}

} // namespace ccl::rslang::detail
