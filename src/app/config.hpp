#pragma once
#include <array>
#include <bitset>
#include <sstream>
#include <unordered_set>
#include "common.hpp"

namespace cfg {
enum class flag { blanks, one_thread, verbose, debug, quiet, follow_symlinks, count_ };

enum class mode {
	explt,
	implt,
};

enum class col { file, code, total, comments, files, ratio, count_ };

struct column final {
	std::string_view name;
	bool reverse = false;

	inline std::string ui_name() const {
		std::string ret(name);
		if (!ret.empty()) {
			ret.at(0) = std::toupper(ret.at(0));
		}
		return ret;
	}
};

inline std::bitset<(std::size_t)flag::count_> g_flags;
inline mode g_mode = mode::explt;
inline col g_sort_by = col::code;
inline constexpr std::array g_flag_names = {"blanks", "one_thread", "verbose", "debug", "quiet", "follow_symlinks"};
inline constexpr std::array g_mode_names = {"explicit", "implicit"};
inline std::array<column, (std::size_t)col::count_> g_columns = {
	{{"file", true}, {"code", false}, {"total", false}, {"comments", false}, {"files", false}, {"ratio", false}}};

struct settings final {
	stdfs::path json_path = "locc_settings.json";
	std::unordered_set<locc::ext> ext_passlist;
	std::unordered_map<locc::ext, locc::id> ext_to_id;
	std::unordered_set<std::string> skip_substrs = {"CMakeFiles", "CMakeCache.txt", ".vscode", ".vs", ".xcode"};
	std::unordered_set<std::string> filenames_as_ext = {"Makefile", "CMakeLists.txt", ".gitignore", ".gitattributes", ".gitmodules"};
	// clang-format off
	std::unordered_map<locc::ext, locc::comment_info> comment_infos = {
		{"c-style", {{"//"}, {{"/*", "*/"}}}},
		{"bash-style", {{"#"}, {}}},
		{"xml-style", {{}, {{"<!--", "-->"}}}},
	};
	std::unordered_map<locc::ext_group, std::deque<locc::ext>> ext_groups = {
		{"c-style", {".c", ".cc", ".cpp", ".h", ".hpp", ".inl", ".tpp", ".java", ".cs", ".js", ".css"}},
		{"bash-style", {".sh", ".py", "CMakeLists.txt"}},
		{"xml-style", {".html", ".xml"}}
	};
	std::unordered_map<locc::id, std::deque<locc::ext>> id_groups = {
		{"C", {".c"}},
		{"C++", {".cc", ".cpp"}},
		{"C header", {".h"}},
		{"C++ header", {".hpp", ".inl", ".tpp"}},
		{"Java", {".java"}},
		{"C#", {".cs"}},
		{"CMake script", {"CMakeLists.txt", ".cmake"}},
		{"Shell script", {".sh"}},
		{"Python", {".py"}},
		{"JavaScript", {".js"}},
		{"TypeScript", {".ts"}},
		{"Plain Text", {".txt"}},
		{"CSS", {".css"}},
		{"JSON", {".json"}},
		{"YAML", {".yml"}},
		{"gitignore", {".gitignore"}},
		{"gitattributes", {".gitattributes"}},
		{"CSV", {".csv"}},
		{"INI", {".ini"}},
		{"HTML", {".htm", ".html"}},
		{"XML", {".xml"}},
		{"Markdown", {".md"}},
	};
	// clang-format on

	locc::comment_info const& find_comment_info(locc::ext const& extension) const;
	locc::id get_id(std::string const& query) const;
	bool import();
};

inline settings g_settings;

inline void set(cfg::flag flag) {
	cfg::g_flags.set((std::size_t)flag);
}

inline bool test(cfg::flag flag) {
	return cfg::g_flags.test((std::size_t)flag);
}
} // namespace cfg
