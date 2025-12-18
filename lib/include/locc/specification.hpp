#pragma once
#include "locc/code_family.hpp"
#include <vector>

namespace locc {
struct Specification {
	/// \brief List of custom CodeGroups.
	std::vector<CodeFamily> code_families{};
	/// \brief List of custom text (non-code) Categories.
	std::vector<Category> text_categories{};
	/// \brief Paths ending with any exclude suffix will be skipped.
	/// Comparison is made against path.generic_string() and is case-insensitive.
	std::vector<std::string_view> exclude_suffixes{};
};
} // namespace locc
