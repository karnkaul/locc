#pragma once
#include "locc/alphabet.hpp"
#include "locc/category.hpp"

namespace locc {
/// \brief Describes a list of Categories that use the same Alphabet.
struct CodeFamily {
	Alphabet alphabet{};
	std::vector<Category> categories{};
};
} // namespace locc
