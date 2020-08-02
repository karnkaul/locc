#pragma once
#include <array>
#include <bitset>
#include <deque>
#include <iostream>
#include <filesystem>
#include <string_view>
#include <sstream>
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

using key = std::string;
using value = std::string;
using entry = std::pair<key, value>;

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
		struct
		{
			std::size_t total = {};
			std::size_t loc = {};
		} max_widths;
		line_counts lines;
	} totals;
};

std::stringstream& concat(std::stringstream& in);

template <typename Arg, typename... Args>
std::stringstream& concat(std::stringstream& in, Arg&& arg, Args&&... args);

template <typename X, typename... Args>
void xout([[maybe_unused]] X& ostream, [[maybe_unused]] Args&&... args);

template <typename... Args>
void log([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args);

template <typename... Args>
void err([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args);

template <typename... Args>
void log(Args&&... args);

template <typename... Args>
void err(Args&&... args);

template <typename... Args>
void log_force(Args&&... args);
} // namespace loc

namespace cfg
{
enum class flag
{
	blanks,
	one_thread,
	verbose,
	debug,
	quiet,
	help,
	count_
};

inline std::bitset<(std::size_t)flag::count_> g_flags;
inline std::array<std::string_view, (std::size_t)flag::count_> const g_flag_names = {"blanks", "one_thread", "verbose", "debug", "quiet", "help"};

inline std::unordered_set<loc::ignore_line> g_ignore_lines;
inline std::unordered_set<loc::ignore_block, loc::pair_hasher> g_ignore_blocks;

inline std::unordered_set<std::string> g_skip_exts = {".exe", ".bin", ".o",	  ".obj", ".a",	  ".lib", ".so",  ".dll",
													  ".jpg", ".png", ".tga", ".mtl", ".pdf", ".zip", ".tar", ".ninja"};
inline std::unordered_set<std::string> g_skip_substrs = {"build", "Build", "out", "CMakeFiles"};

inline void set(cfg::flag flag)
{
	cfg::g_flags.set((std::size_t)flag);
}

inline bool test(cfg::flag flag)
{
	return cfg::g_flags.test((std::size_t)flag);
}
} // namespace cfg

namespace loc
{
inline std::stringstream& concat(std::stringstream& in)
{
	return in;
}

template <typename Arg, typename... Args>
std::stringstream& concat(std::stringstream& in, Arg&& arg, Args&&... args)
{
	in << std::forward<Arg>(arg);
	return concat(in, std::forward<Args>(args)...);
}

template <typename X, typename... Args>
void xout([[maybe_unused]] X& ostream, [[maybe_unused]] Args&&... args)
{
	if constexpr (sizeof...(args) > 0)
	{
		std::stringstream str;
		concat(str, std::forward<Args>(args)...);
		ostream << str.str();
	}
}

template <typename... Args>
void log([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cout, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void err([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cerr, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void log(Args&&... args)
{
	log(true, std::forward<Args>(args)...);
}

template <typename... Args>
void err(Args&&... args)
{
	err(true, std::forward<Args>(args)...);
}

template <typename... Args>
void log_force(Args&&... args)
{
	xout(std::cout, std::forward<Args>(args)...);
}
} // namespace loc
