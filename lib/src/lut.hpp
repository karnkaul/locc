#pragma once
#include <locc/grammar.hpp>
#include <span>
#include <unordered_map>

namespace locc {
struct Lut {
	struct Entry {
		Grammar const* grammar{};
		std::string_view type_name{};
	};

	std::unordered_map<std::string_view, Entry> by_extension{};
	std::unordered_map<std::string_view, Entry> by_filename{};

	[[nodiscard]] static auto build(std::span<Grammar const> grammars) -> Lut;

	[[nodiscard]] auto extension(std::string_view ext) const -> Entry const*;
	[[nodiscard]] auto filename(std::string_view name) const -> Entry const*;

	[[nodiscard]] auto get_entry(std::string_view const ext, std::string_view const name) const -> Entry const* {
		auto const* ret = filename(name);
		if (ret == nullptr) { ret = extension(ext); }
		return ret;
	}
};
} // namespace locc
