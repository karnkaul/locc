#include <locc/grammar.hpp>
#include <locc/json_io.hpp>
#include <cassert>
#include <format>

namespace locc {
namespace {
auto create_c_like() -> Grammar {
	return Grammar{
		.file_types =
			{
				FileType{.type_name = "C++ header", .extensions = {".hpp", ".inl", ".tpp"}},
				FileType{.type_name = "C++", .extensions = {".cpp", ".cc", ".cxx"}},
				FileType{.type_name = "C/C++ header", .extensions = {".h"}},
				FileType{.type_name = "C", .extensions = {".c"}},
				FileType{.type_name = "Rust", .extensions = {".rs"}},
				FileType{.type_name = "C#", .extensions = {".cs"}},
				FileType{.type_name = "Java", .extensions = {".java"}},
				FileType{.type_name = "Kotlin", .extensions = {".kt"}},
				FileType{.type_name = "Go", .extensions = {".go"}},
				FileType{.type_name = "JavaScript", .extensions = {".js"}},
				FileType{.type_name = "TypeScript", .extensions = {".ts"}},
				FileType{.type_name = "JSON", .extensions = {".json"}},
				FileType{.type_name = "Groovy", .extensions = {".groovy"}},
				FileType{.type_name = "Swift", .extensions = {".swift"}},
			},
		.line_comments = {LineComment{.prefix = "//"}},
		.block_comments = {BlockComment{.open = "/*", .close = "*/"}},
	};
}

auto create_shell_like() -> Grammar {
	return Grammar{
		.file_types =
			{
				FileType{.type_name = "CMake script", .extensions = {".cmake"}, .exact_filename = "CMakeLists.txt"},
				FileType{.type_name = "Shell script", .extensions = {".sh", ".bash"}},
				FileType{.type_name = "Python", .extensions = {".py"}},
				FileType{.type_name = "Makefile", .exact_filename = "Makefile"},
				FileType{.type_name = "YAML", .extensions = {".yaml"}},
				FileType{.type_name = "INI", .extensions = {".ini"}},
			},
		.line_comments = {LineComment{.prefix = "#"}},
	};
}

auto create_html_like() -> Grammar {
	return Grammar{
		.file_types =
			{
				FileType{.type_name = "HTML", .extensions = {".htm", ".html", ".xhtml"}},
				FileType{.type_name = "XML", .extensions = {".xml"}},
				FileType{.type_name = "Markdown", .extensions = {".md"}},
				FileType{.type_name = "PHP", .extensions = {".php"}},
			},
		.block_comments = {BlockComment{.open = "<!--", .close = "-->"}},
	};
}

auto create_plain_text() -> Grammar {
	return Grammar{
		.file_types =
			{
				FileType{.type_name = "Plain Text", .extensions = {".txt"}},
				FileType{.type_name = "CSV", .extensions = {".csv"}},
			},
	};
}
} // namespace

auto Grammar::create(Type const type) -> Grammar {
	switch (type) {
	case Type::CLike: return create_c_like();
	case Type::ShellLike: return create_shell_like();
	case Type::HtmlLike: return create_html_like();
	case Type::PlainText: return create_plain_text();
	default: return {};
	}
}
} // namespace locc

auto locc::get_default_grammars() -> std::vector<Grammar> const& {
	static auto const ret = [] {
		using Type = Grammar::Type;
		auto ret = std::vector<Grammar>{};
		ret.reserve(std::size_t(Type::COUNT_));
		for (auto t = Type{}; t < Type::COUNT_; t = Type(int(t) + 1)) {
			ret.push_back(Grammar::create(t));
			assert(ret.back().is_valid());
		}
		return ret;
	}();
	return ret;
}

auto locc::serialize_default_grammars() -> std::string {
	auto json = dj::Json{};
	for (auto const& grammar : get_default_grammars()) { to_json(json.push_back({}), grammar); }
	return std::format("{}", json);
}

auto locc::load_grammars_into(std::vector<Grammar>& out, char const* path_to_json) -> std::size_t {
	auto const json = dj::Json::from_file(path_to_json);
	if (!json) { return 0; }
	auto const grammars = json->as_array();
	out.reserve(out.size() + grammars.size());
	for (auto const& in_g : grammars) {
		auto out_g = Grammar{};
		from_json(in_g, out_g);
		if (!out_g.is_valid()) { continue; }
		out.push_back(std::move(out_g));
	}
	return grammars.size();
}
