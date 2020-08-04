#pragma once
#include <array>
#include <bitset>
#include <sstream>
#include <unordered_set>
#include "common.hpp"

namespace cfg
{
enum class flag
{
	blanks,
	one_thread,
	verbose,
	debug,
	quiet,
	count_
};

enum class mode
{
	explt,
	implt,
};

inline std::bitset<(std::size_t)flag::count_> g_flags;
inline mode g_mode = mode::explt;
inline constexpr std::array g_flag_names = {"blanks", "one_thread", "verbose", "debug", "quiet"};
inline constexpr std::array g_mode_names = {"explicit", "implicit"};

inline std::unordered_set<locc::comment_line> g_comment_lines;

inline std::unordered_set<std::string> g_ext_passlist;
inline std::unordered_set<std::string> g_skip_substrs = {"build", "Build", "out", "CMakeFiles"};

inline std::unordered_map<locc::ext, locc::ext_group> g_ext_groups = {
	{".c", "c-style"},	 {".cc", "c-style"},	{".cpp", "c-style"},   {".h", "c-style"},	   {".hpp", "c-style"},
	{".inl", "c-style"}, {".tpp", "c-style"},	{".java", "c-style"},  {".cs", "c-style"},	   {".js", "c-style"},
	{".css", "c-style"}, {".sh", "bash-style"}, {".py", "bash-style"}, {".txt", "bash-style"}, {".cmake", "bash-style"}};
inline locc::ext_config g_ext_config = {
	{"c-style", {{"//"}, {{"/*", "*/"}}}},
	{"bash-style", {{"#"}, {}}},
};

inline locc::config const& find_config(locc::ext const& extension)
{
	auto iter = g_ext_config.end();
	if (auto group = g_ext_groups.find(extension); group != g_ext_groups.end())
	{
		iter = g_ext_config.find(group->second);
	}
	else
	{
		iter = g_ext_config.find(extension);
	}
	if (iter != g_ext_config.end())
	{
		return iter->second;
	}
	static locc::config const default_ret;
	return default_ret;
}

inline void set(cfg::flag flag)
{
	cfg::g_flags.set((std::size_t)flag);
}

inline bool test(cfg::flag flag)
{
	return cfg::g_flags.test((std::size_t)flag);
}
} // namespace cfg
