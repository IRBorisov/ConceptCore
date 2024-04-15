#include "pch.h"

#include "ConceptCore.h"
#include "wrapRSLang.h"

namespace {

std::string TextFromVariant(const VARIANT* var, UINT codePage = CP_UTF8) {
	if (var->vt != VT_BSTR) {
		throw std::runtime_error("Expected string Variant");
	}
	int inputSize = static_cast<int>(::SysStringLen(var->bstrVal));
	const auto outputSize = ::WideCharToMultiByte(codePage, 0, var->bstrVal, inputSize, nullptr, 0, nullptr, nullptr);
	std::string result(outputSize, 0);
	::WideCharToMultiByte(codePage, 0, var->bstrVal, inputSize, &result[0], outputSize, nullptr, nullptr);
	return result;
}

void TextToVariant(const char* text, VARIANT* result, UINT codePage = CP_UTF8) {
	::VariantClear(result);

	const auto inputSize = static_cast<int>(::strlen(text)) + 1;
	auto wideBuffer = std::make_unique<OLECHAR[]>(inputSize); // Note: buffer capacity is excessive because multibyte -> wchar_t
	::MultiByteToWideChar(codePage, 0, text, static_cast<int>(inputSize * sizeof(OLECHAR)), wideBuffer.get(), inputSize);
	result->vt = VT_BSTR;
	result->bstrVal = SysAllocString(wideBuffer.get());
}

} // anonymous namespace

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE /*hinstDLL*/, DWORD fdwReason, LPVOID /*lpvReserved*/) {
	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		return TRUE;

	case DLL_PROCESS_DETACH:
		return TRUE;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		return TRUE;
	}
}

DLLEXPORT HRESULT __stdcall ConvertToASCII(VARIANT* outResult, VARIANT* inputText) {
	try {
		auto result = core::ToASCII(TextFromVariant(inputText));
		TextToVariant(result.c_str(), outResult);
		return S_OK;
	} catch (const std::exception& e) {
		TextToVariant(e.what(), outResult, CP_ACP);
		return E_FAIL;
	}
}

DLLEXPORT HRESULT __stdcall ConvertToMath(VARIANT* outResult, const char* inputText) {
	try {
		auto result = core::ToMath(inputText);
		TextToVariant(result.c_str(), outResult);
		return S_OK;
	} catch (const std::exception& e) {
		TextToVariant(e.what(), outResult, CP_ACP);
		return E_FAIL;
	}
}

DLLEXPORT HRESULT __stdcall ASTasText(VARIANT* outResult, const char* inputText) {
	try {
		auto result = core::GetASTString(inputText);
		TextToVariant(result.c_str(), outResult);
		return S_OK;
	} catch (const std::exception& e) {
		TextToVariant(e.what(), outResult, CP_ACP);
		return E_FAIL;
	}
}