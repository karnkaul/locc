#pragma once
#include <string_view>
#include <vector>

namespace locc {
/// \brief List of filename and extension matches.
struct Category {
	/// \brief Display name.
	std::string_view name{};
	/// \brief Comparison is case-sensitive.
	std::vector<std::string_view> exact_filenames{};
	/// \brief Comparison is case-insensitive.
	std::vector<std::string_view> file_extensions{};
};
} // namespace locc
