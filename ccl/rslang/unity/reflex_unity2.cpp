//! Unity build for used pars of RE-flex library
// see https://github.com/Genivia/RE-flex

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4127 4244 4245 4456 4706 4996 )
	#pragma warning( disable : 6011 6031 6246 6297 6323 6330 6340 6385 6386 6387 )
	#pragma warning( disable : 26400 26401 26408 26426 26438 26440 26446 26447 )
	#pragma warning( disable : 26451 26456 26462 26474 26481 26482 26485 )
	#pragma warning( disable : 26492 26493 26494 26495 26496 26497 28182 )
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
	#pragma GCC diagnostic ignored "-Wunused-label"
	#pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#include "../lib/debug.cpp"
#include "../lib/error.cpp"
#include "../lib/input.cpp"
#include "../lib/matcher.cpp"

#undef min
#undef max
#include "../lib/pattern.cpp"

#include "../lib/posix.cpp"
#include "../lib/unicode.cpp"
#include "../lib/utf8.cpp"

#include "../unicode/block_scripts.cpp"
#include "../unicode/language_scripts.cpp"
#include "../unicode/letter_scripts.cpp"
#include "../unicode/composer.cpp"

#ifdef _MSC_VER
	#pragma warning( pop )
#endif

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#pragma GCC diagnostic pop
#endif
