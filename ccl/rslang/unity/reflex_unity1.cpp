//! Unity build for used pars of RE-flex library
// see https://github.com/Genivia/RE-flex

#ifdef _MSC_VER
	#pragma warning( push )
	#pragma warning( disable : 4127 4244 4245 4456 4996 )
	#pragma warning( disable : 6246 6330 6386 )
	#pragma warning( disable : 26440 26446 26451 )
	#pragma warning( disable : 26460 26462 26465 26481 26482 26485 )
	#pragma warning( disable : 26492 26494 26496 26497 26819 28182 )
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
#endif

#include "../lib/convert.cpp"

#ifdef _MSC_VER
	#pragma warning( pop )
#endif

#ifdef __clang__
	#pragma clang diagnostic pop
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#pragma GCC diagnostic pop
#endif
