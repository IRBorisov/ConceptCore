#pragma once

#ifdef CONCEPTCOREDLL_EXPORTS
	#define DLLEXPORT __declspec(dllexport)
#else
	#define DLLEXPORT __declspec(dllimport)
#endif

//!Requires: const char* arguments are expected to ASCII-only character sequences!
DLLEXPORT HRESULT __stdcall ConvertToASCII(VARIANT* outResult, VARIANT* inputText);
DLLEXPORT HRESULT __stdcall ConvertToMath(VARIANT* outResult, const char* inputText);

//! Return "INVALID_INPUT" if input text is not valid ASCII syntax RSLang sequence
DLLEXPORT HRESULT __stdcall ASTasText(VARIANT* outResult, const char* inputText);