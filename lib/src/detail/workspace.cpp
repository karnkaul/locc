#include "detail/workspace.hpp"
#include <cassert>

namespace locc::detail {
auto Workspace::to_absolute_path(std::string_view const in) -> fs::path {
	if (in.empty()) { return fs::current_path(); }
	return fs::weakly_canonical(fs::absolute(in));
}

auto Workspace::find_exclude_match(fs::path const& path) const -> std::string_view {
	if (config.spec.exclude_suffixes.empty()) { return {}; }
	return config.find_exclude_match(path.generic_string());
}

auto Workspace::get_entry(fs::path const& path) -> Entry* {
	auto const extension = path.extension().string();
	auto const filename = path.filename().string();
	auto const entry_info = config.find_entry_info(filename, extension);
	if (entry_info.category.empty()) { return nullptr; }

	auto lock = std::scoped_lock{mutex};
	auto it = entries.find(entry_info.category);
	if (it == entries.end()) { it = entries.insert_or_assign(entry_info.category, entry_info.alphabet).first; }
	assert(it != entries.end());
	return &it->second;
}
} // namespace locc::detail
