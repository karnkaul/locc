#pragma once
#include <string>
#include <vector>

namespace locc {
struct LineComment {
	std::string prefix{}; // required

	[[nodiscard]] auto is_valid() const -> bool { return !prefix.empty(); }
};

struct BlockComment {
	std::string open{};	 // required
	std::string close{}; // required

	[[nodiscard]] auto is_valid() const -> bool { return !open.empty() && !close.empty(); }
};

struct FileType {
	std::string type_name{};			   // required
	std::vector<std::string> extensions{}; // at least 1 extension or exact_filename required
	std::string exact_filename{};		   // at least 1 extension or exact_filename required

	[[nodiscard]] auto is_valid() const -> bool { return !type_name.empty() && (!extensions.empty() || !exact_filename.empty()); }
};

struct Grammar {
	enum class Type { CLike, ShellLike, HtmlLike, PlainText, COUNT_ };

	std::vector<FileType> file_types{};			// minimum 1 requied
	std::vector<LineComment> line_comments{};	// optional
	std::vector<BlockComment> block_comments{}; // optional

	[[nodiscard]] auto is_valid() const -> bool { return !file_types.empty(); }

	[[nodiscard]] auto is_code() const -> bool { return !line_comments.empty() || !block_comments.empty(); }

	[[nodiscard]] static auto create(Type type) -> Grammar;
};

[[nodiscard]] auto get_default_grammars() -> std::vector<Grammar> const&;

[[nodiscard]] auto serialize_default_grammars() -> std::string;

auto load_grammars_into(std::vector<Grammar>& out, char const* path_to_json) -> std::size_t;
} // namespace locc
