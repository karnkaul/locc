#pragma once
#include <array>
#include <bitset>
#include <deque>
#include <filesystem>
#include <string_view>
#include <unordered_set>

#define DOIF(cond, expr) \
	do                   \
	{                    \
		if (cond)        \
		{                \
			expr;        \
		}                \
	} while (0);

namespace stdfs = std::filesystem;

namespace loc
{
using ignore_line = std::string;
using ignore_block = std::pair<std::string, std::string>;

constexpr std::size_t null_index = std::string::npos;

struct pair_hasher final
{
	std::size_t operator()(ignore_block const& pair) const
	{
		return std::hash<std::string>()(pair.first) ^ std::hash<std::string>()(pair.second);
	}
};

struct line_counts final
{
	std::size_t total = {};
	std::size_t empty = {};
	std::size_t loc = {};
};

struct file final
{
	stdfs::path path;
	line_counts lines;
};

struct result final
{
	std::deque<file> files;
	struct
	{
		line_counts max_widths;
		line_counts lines;
	} totals;
};
} // namespace loc

namespace cfg
{
enum class flag
{
	blanks,
	one_thread,
	verbose,
	debug,
	help,
	count_
};

inline std::bitset<(std::size_t)flag::count_> g_flags;
inline std::array<std::string_view, (std::size_t)flag::count_> const g_flag_names = {"blanks", "one_thread", "verbose", "debug", "help"};
inline constexpr std::array g_skip_ext = {".jpg", ".png", ".tga", ".pdf", ".exe", ".dll", ".zip", ".tar", ".a", ".so", ".obj", ".mtl", ".ninja", ".bin", ".o"};

inline std::unordered_set<loc::ignore_line> g_ignore_lines;
inline std::unordered_set<loc::ignore_block, loc::pair_hasher> g_ignore_blocks;
inline std::unordered_set<std::string> g_skip_dirs = {"build", "Build", "out", "CMakeFiles"};

inline void set(cfg::flag flag)
{
	cfg::g_flags.set((std::size_t)flag);
}

inline bool test(cfg::flag flag)
{
	return cfg::g_flags.test((std::size_t)flag);
}
} // namespace cfg
