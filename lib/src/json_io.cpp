#include <locc/json_io.hpp>

namespace {
template <typename Type>
void fill(std::vector<Type>& out, dj::Json const& arr) {
	auto const& av = arr.array_view();
	out.reserve(av.size());
	for (auto const& in : av) {
		auto t = Type{};
		from_json(in, t);
		if constexpr (std::same_as<Type, std::string>) {
			if (t.empty()) { continue; }
		} else {
			if (!t.is_valid()) { continue; }
		}
		out.push_back(std::move(t));
	}
}

template <typename Type>
void fill(dj::Json& out, std::string_view const key, std::vector<Type> const& arr) {
	if (arr.empty()) { return; }
	auto& out_arr = out[key];
	for (auto const& in : arr) { to_json(out_arr.push_back({}), in); }
}
} // namespace

void locc::to_json(dj::Json& out, LineComment const& line_comment) { out = line_comment.prefix; }

void locc::from_json(dj::Json const& json, LineComment& out) { out.prefix = json.as_string(); }

void locc::to_json(dj::Json& out, BlockComment const& block_comment) {
	to_json(out["open"], block_comment.open);
	to_json(out["close"], block_comment.close);
}

void locc::from_json(dj::Json const& json, BlockComment& out) {
	from_json(json["open"], out.open);
	from_json(json["close"], out.close);
}

void locc::to_json(dj::Json& out, FileType const& file_type) {
	to_json(out["type_name"], file_type.type_name);
	if (!file_type.exact_filename.empty()) { to_json(out["exact_filename"], file_type.exact_filename); }
	fill(out, "extensions", file_type.extensions);
}

void locc::from_json(dj::Json const& json, FileType& out) {
	from_json(json["type_name"], out.type_name);
	from_json(json["exact_filename"], out.exact_filename);
	fill(out.extensions, json["extensions"]);
}

void locc::to_json(dj::Json& out, Grammar const& grammar) {
	fill(out, "file_types", grammar.file_types);
	fill(out, "line_comments", grammar.line_comments);
	fill(out, "block_comments", grammar.block_comments);
}

void locc::from_json(dj::Json const& json, Grammar& out) {
	fill(out.file_types, json["file_types"]);
	fill(out.line_comments, json["line_comments"]);
	fill(out.block_comments, json["block_comments"]);
}
