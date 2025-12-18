#pragma once
#include <string_view>
#include <vector>

namespace locc {
namespace comment {
struct Line {
	std::string_view token{};
};

struct Block {
	std::string_view open{};
	std::string_view close{};
};
} // namespace comment

/// \brief Describes a comment schema.
struct Alphabet {
	std::vector<comment::Line> line_comments{};
	std::vector<comment::Block> block_comments{};
};
} // namespace locc
