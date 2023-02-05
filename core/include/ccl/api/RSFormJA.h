#pragma once

#include "ccl/semantic/RSForm.h"

namespace ccl::api {

//! JSON accessor wrapper for RSForm object
class RSFormJA {
private:
	std::unique_ptr<semantic::RSForm> schema{};
	static constexpr int JSON_IDENT = 4;

private:
	RSFormJA() noexcept = default;

public:
	[[nodiscard]] const semantic::RSForm& data() const noexcept;
	[[nodiscard]] semantic::RSForm& data() noexcept;

	[[nodiscard]] static RSFormJA FromData(semantic::RSForm&& data);
	[[nodiscard]] static RSFormJA FromJSON(std::string_view json);
	[[nodiscard]] std::string ToJSON() const;
	[[nodiscard]] std::string ToMinimalJSON() const;

	[[nodiscard]] std::string ParseExpression(const std::string& text) const;
};

} // namespace ccl::api
