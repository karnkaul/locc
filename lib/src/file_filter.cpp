#include <locc/file_filter.hpp>
#include <algorithm>

namespace locc {
auto DefaultFileFilter::should_count(std::string_view const path) const -> bool {
	return !std::ranges::any_of(skip_patterns, [path](std::string_view const s) { return path.contains(s); });
}

auto DefaultFileFilter::get_instance() -> DefaultFileFilter const& {
	static auto ret = DefaultFileFilter{};
	return ret;
}
} // namespace locc
