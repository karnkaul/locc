#pragma once
#include <string_view>
#include <vector>

namespace locc {
class IFileFilter {
  public:
	IFileFilter() = default;
	IFileFilter(IFileFilter const&) = default;
	IFileFilter(IFileFilter&&) = default;
	auto operator=(IFileFilter const&) -> IFileFilter& = default;
	auto operator=(IFileFilter&&) -> IFileFilter& = default;

	virtual ~IFileFilter() = default;

	[[nodiscard]] virtual auto should_count(std::string_view path) const -> bool = 0;
};

struct DefaultFileFilter : IFileFilter {
	std::vector<std::string_view> skip_patterns{"build/", "out/", ".cache/", ".git/"};

	[[nodiscard]] auto should_count(std::string_view path) const -> bool override;

	[[nodiscard]] static auto get_instance() -> DefaultFileFilter const&;
};
} // namespace locc
