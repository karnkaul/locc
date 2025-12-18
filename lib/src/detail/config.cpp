#include "detail/config.hpp"
#include "locc/iequal.hpp"
#include "locc/json_io.hpp"
#include "locc/logger.hpp"
#include <array>

namespace locc::detail {
namespace {
auto const log = Logger<Config>{};

auto const default_spec = Specification{
	.code_families = {
		CodeFamily{
			.alphabet =
				Alphabet{
					.line_comments = {comment::Line{.token = "//"}},
					.block_comments = {comment::Block{.open = "/*", .close = "*/"}},
				},
			.categories =
				{
					Category{.name = "C/C++ Header", .file_extensions = {".h"}},
					Category{.name = "C++ Header", .file_extensions = {".hpp", ".tpp", ".inl"}},
					Category{.name = "C Source", .file_extensions = {".c"}},
					Category{.name = "C++ Source", .file_extensions = {".cpp", ".cc", ".cxx"}},
					Category{.name = "C++ Module", .file_extensions = {".cppm", ".ixx"}},
					Category{.name = "JSONC", .file_extensions = {".jsonc"}},
					Category{.name = "Rust", .file_extensions = {".rs"}},
					Category{.name = "C#", .file_extensions = {".cs"}},
					Category{.name = "Java", .file_extensions = {".java"}},
					Category{.name = "Kotlin", .file_extensions = {".kt"}},
					Category{.name = "Go", .file_extensions = {".go"}},
					Category{.name = "JavaScript", .file_extensions = {".js"}},
					Category{.name = "TypeScript", .file_extensions = {".ts"}},
					Category{.name = "Groovy", .file_extensions = {".groovy"}},
					Category{.name = "Swift", .file_extensions = {".swift"}},
				},
		},
		CodeFamily{
			.alphabet =
				Alphabet{
					.line_comments = {comment::Line{.token = "#"}},
				},
			.categories =
				{
					Category{.name = "Python", .file_extensions = {".py"}},
					Category{.name = "Shell", .file_extensions = {".sh", ".bash", ".zsh", ".bat"}},
					Category{.name = "CMake", .exact_filenames = {"CMakeLists.txt"}, .file_extensions = {".cmake"}},
					Category{.name = "Makefile", .exact_filenames = {"Makefile"}},
					Category{.name = "YAML", .file_extensions = {".yaml"}},
					Category{.name = "INI", .file_extensions = {".ini"}},
				},
		},
		CodeFamily{
			.alphabet =
				Alphabet{
					.block_comments = {comment::Block{.open = "<!--", .close = "-->"}},
				},
			.categories =
				{
					Category{.name = "HTML", .file_extensions = {".htm", ".html", ".xhtml"}},
					Category{.name = "XML", .file_extensions = {".xml"}},
					Category{.name = "Markdown", .file_extensions = {".md"}},
					Category{.name = "PHP", .file_extensions = {".php"}},
				},
		},
	},
	.text_categories = {
		Category{.name = "Plain Text", .file_extensions = {".txt"}},
		Category{.name = "JSON", .file_extensions = {".json"}},
	},
	.exclude_suffixes = {
		"build", "out", ".git", ".cache", ".config", ".vs", ".vscode", ".idea", "ext/src", "vendor",
	},
};

template <typename T, typename U>
void append_to(T& out, U const& u) {
	out.insert(out.end(), std::begin(u), std::end(u));
}

void append_to(Specification& out, Specification const& in, Flag const flags) {
	if ((flags & Flag::NoBuiltinCodeFamilies) == Flag::None) { append_to(out.code_families, in.code_families); }
	if ((flags & Flag::NoBuiltinTextCategories) == Flag::None) { append_to(out.text_categories, in.text_categories); }
	if ((flags & Flag::NoBuiltinExcludeSuffixes) == Flag::None) { append_to(out.exclude_suffixes, in.exclude_suffixes); }
}

void append_from_json(Config& out, std::string_view const json_path) {
	if (json_path.empty()) { return; }
	auto result = dj::Json::from_file(json_path);
	if (!result) {
		log.warn("failed to read spec JSON: {}", json_path);
		return;
	}

	out.custom_spec_json = std::move(*result);
	auto spec = Specification{};
	from_json(out.custom_spec_json, spec);
	log.info("parsed Specification from: {} (code families: {}, text categories: {}, exclude suffixes: {})", json_path, spec.code_families.size(),
			 spec.text_categories.size(), spec.exclude_suffixes.size());
	append_to(out.spec, spec, Flag::None);
}
} // namespace

auto Config::build(InitInfo info) -> Config {
	auto ret = Config{.spec = std::move(info.custom_spec), .heuristic = info.heuristic};

	append_from_json(ret, info.custom_spec_json);
	append_to(ret.spec, default_spec, info.flags);

	return ret;
}

auto Config::find_exclude_match(std::string_view const path) const -> std::string_view {
	auto const it = std::ranges::find_if(spec.exclude_suffixes, [&path](std::string_view const suffix) {
		if (path.size() < suffix.size()) { return false; }
		auto const substr = path.substr(path.size() - suffix.size());
		return iequal(substr, suffix);
	});
	if (it == spec.exclude_suffixes.end()) { return {}; }
	return *it;
}

[[nodiscard]] auto Config::find_entry_info(std::string_view const filename, std::string_view const extension) const -> EntryInfo {
	auto const cat_match = [filename, extension](Category const& category) {
		if (std::ranges::find(category.exact_filenames, filename) != category.exact_filenames.end()) { return true; }
		auto const ext_match = [extension](std::string_view const in) { return iequal(extension, in); };
		return std::ranges::find_if(category.file_extensions, ext_match) != category.file_extensions.end();
	};

	for (auto const& family : spec.code_families) {
		auto const it = std::ranges::find_if(family.categories, cat_match);
		if (it != family.categories.end()) { return EntryInfo{.category = it->name, .alphabet = &family.alphabet}; }
	}

	auto const it = std::ranges::find_if(spec.text_categories, cat_match);
	if (it != spec.text_categories.end()) { return EntryInfo{.category = it->name}; }

	return {};
}
} // namespace locc::detail
