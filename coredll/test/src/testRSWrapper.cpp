#include "pch.h"

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

} // Unnamed detail namespace

class UTRSWrap : public ::testing::Test {
protected:
	void TestMathCall(const std::string& input, const std::string& expected = "", bool expectSuccess = true);
	void TestASCIICall(const std::string& input, const std::string& expected = "", bool expectSuccess = true);
	void TestASTCall(const std::string& input, const std::string& expected = "", bool expectSuccess = true);
};

void UTRSWrap::TestMathCall(const std::string& input, const std::string& expected, bool expectSuccess) {
	VARIANT varInput{};
	TextToVariant(input.c_str(), &varInput);
	auto encodedInput = TextFromVariant(&varInput, CP_ACP);
	
	VARIANT varResult{};
	EXPECT_EQ(expectSuccess, SUCCEEDED(::ConvertToMath(&varResult, encodedInput.c_str())));
	if (expectSuccess) {
		auto result = TextFromVariant(&varResult);
		EXPECT_EQ(expected, result) << "Convert from " << input << " to " << expected;
	}
	::VariantClear(&varInput);
	::VariantClear(&varResult);
}

void UTRSWrap::TestASCIICall(const std::string& input, const std::string& expected, bool expectSuccess) {
	VARIANT varInput{};
	TextToVariant(input.c_str(), &varInput);

	VARIANT varResult{};
	EXPECT_EQ(expectSuccess, SUCCEEDED(::ConvertToASCII(&varResult, &varInput)));
	if (expectSuccess) {
		auto result = TextFromVariant(&varResult);
		EXPECT_EQ(expected, result) << "Convert from " << input << " to " << expected;
	}
	::VariantClear(&varInput);
	::VariantClear(&varResult);
}

void UTRSWrap::TestASTCall(const std::string& input, const std::string& expected, bool expectSuccess) {
	VARIANT varResult{};
	EXPECT_EQ(expectSuccess, SUCCEEDED(::ASTasText(&varResult, input.c_str())));
	if (expectSuccess) {
		auto result = TextFromVariant(&varResult);
		EXPECT_EQ(expected, result) << "AST from " << input << " should be " << expected;
	}
	::VariantClear(&varResult);
}

TEST_F(UTRSWrap, ConvertToMath) {
	TestMathCall("", "");
	TestMathCall("a,?b", "a,?b");
	TestMathCall("1+1=2", "1+1=2");
	TestMathCall("B(X1*X2)", "B(X1*X2)"_rs);
	TestMathCall("   X1   ", "X1");
	TestMathCall(R"(S1 \in B(X1))", u8"S1\u2208\u212C(X1)"_c17);
}

TEST_F(UTRSWrap, ConvertToASCII) {
	TestASCIICall("", "");
	TestASCIICall("a,?b", "a,?b");
	TestASCIICall("1+1=2", R"(1 \plus 1 \eq 2)");
	TestASCIICall("B(X1*X2)"_rs, "B(X1*X2)");
	TestASCIICall("   X1   ", "X1");
	TestASCIICall(u8"S1\u2208\u212C(X1)"_c17, R"(S1 \in B(X1))");
}

TEST_F(UTRSWrap, ASTString) {
	TestASTCall("", "INVALID_INPUT");
	TestASTCall("a,?b", "INVALID_INPUT");
	TestASTCall("1+1=2", "[=[+[1][1]][2]]");
	TestASTCall("B(X1*X2)", u8"[\u212C[\u00D7[X1][X2]]]"_c17);
	TestASTCall(R"(\A a \in X1 1 \eq 2)", u8"[\u2200[a][X1][=[1][2]]]"_c17);
}