#pragma once
#include "detail/config.hpp"
#include "locc/alphabet.hpp"
#include "locc/metrics.hpp"
#include <filesystem>
#include <mutex>
#include <unordered_map>

namespace locc::detail {
namespace fs = std::filesystem;

struct Workspace {
	struct Entry {
		explicit(false) Entry(Alphabet const* alphabet = {}) : alphabet(alphabet) {}

		Alphabet const* alphabet{};
		Metrics metrics{};
	};

	using EntryMap = std::unordered_map<std::string_view, Entry>;

	explicit Workspace(Config const& config) : config(config) {}

	[[nodiscard]] static auto to_absolute_path(std::string_view in_path) -> fs::path;

	[[nodiscard]] auto find_exclude_match(fs::path const& path) const -> std::string_view;
	[[nodiscard]] auto get_entry(fs::path const& path) -> Entry*;

	Config const& config;

	fs::path path{};

	EntryMap entries{};
	std::mutex mutex{};
};
} // namespace locc::detail
