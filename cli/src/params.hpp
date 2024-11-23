#pragma once
#include <cstdint>
#include <string_view>

namespace locc::cli {
struct Params {
	std::string_view sort_by{};
	std::string_view exclude_patterns{};
	std::string_view grammars_json{};
	std::uint8_t thread_count{};

	bool no_progress{};
	bool verbose{};

	std::string_view path{"."};
};
} // namespace locc::cli
