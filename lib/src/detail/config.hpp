#pragma once
#include "locc/init_info.hpp"
#include <djson/json.hpp>

namespace locc::detail {
struct Config {
	struct EntryInfo {
		std::string_view category{};
		Alphabet const* alphabet{};
	};

	static auto build(InitInfo info) -> Config;

	[[nodiscard]] auto find_exclude_match(std::string_view path) const -> std::string_view;
	[[nodiscard]] auto find_entry_info(std::string_view filename, std::string_view extension) const -> EntryInfo;

	dj::Json custom_spec_json{};
	Specification spec{};
	Heuristic heuristic{};
};
} // namespace locc::detail
