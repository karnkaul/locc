#pragma once
#include <array>
#include <bitset>
#include <deque>
#include <iostream>
#include <filesystem>
#include <map>
#include <string_view>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace stdfs = std::filesystem;

namespace locc
{
template <typename T>
using pair_t = std::pair<T, T>;

using comment_line = std::string;
using comment_block = pair_t<std::string>;
using ext = std::string;
using ext_group = std::string;

using comment_line_view = std::string_view;
using comment_block_view = pair_t<std::string_view>;
using ext_view = std::string_view;
using ext_group_view = std::string_view;

struct config final
{
	std::deque<comment_line> comment_lines;
	std::deque<comment_block> comment_blocks;
};
using ext_config = std::unordered_map<ext, config>;

using key = std::string;
using value = std::string;
using entry = std::pair<key, value>;

using key_view = std::string_view;
using value_view = std::string_view;
using entry_view = std::pair<key_view, value_view>;

constexpr std::size_t null_index = std::string::npos;

struct lines
{
	std::size_t code = {};
	std::size_t comments = {};
	std::size_t total = {};
};

struct lines_blank final : lines
{
	std::size_t blank = {};
};

struct file final
{
	stdfs::path path;
	lines_blank lines;
};

struct result final
{
	struct ext_data
	{
		struct
		{
			lines lines;
			std::size_t files = {};
		} counts;
		float ratio = {};
	};
	using distribution = std::unordered_map<std::string, ext_data>;

	distribution dist;
	std::deque<file> files;
	struct
	{
		struct
		{
			std::size_t total = {};
			std::size_t code = {};
		} max_widths;
		lines lines;
	} totals;

	template <template <typename...> typename Cont = std::map, typename Pred, typename... Args>
	auto transform_dist(Pred predicate) const
	{
		Cont<std::string, ext_data, Args...> ret;
		for (auto const& [ext, data] : dist)
		{
			if (predicate(ext, data))
			{
				ret.emplace(ext, data);
			}
		}
		return ret;
	}

	template <template <typename...> typename Cont = std::map, typename... Args>
	auto transform_dist() const
	{
		auto f = [](auto const&, auto const&) -> bool { return true; };
		return transform_dist<Cont, decltype(f), Args...>(f);
	}
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

template <typename F, typename... Args>
void do_if(bool predicate, F f, Args&&...);

template <typename Pred, typename F, typename... Args>
void do_if(Pred predicate, F f, Args&&...);
} // namespace locc

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

enum class mode
{
	explt,
	implt,
	count_
};

struct pair_hasher final
{
	std::size_t operator()(locc::comment_block const& pair) const
	{
		return std::hash<std::string>()(pair.first) ^ std::hash<std::string>()(pair.second);
	}
};

inline std::bitset<(std::size_t)flag::count_> g_flags;
inline mode g_mode = mode::explt;
inline std::array<std::string_view, (std::size_t)flag::count_> const g_flag_names = {"blanks", "one_thread", "verbose", "debug", "quiet", "help"};
inline std::array<std::string_view, (std::size_t)mode::count_> const g_mode_names = {"explicit", "implicit"};

inline std::unordered_set<locc::comment_line> g_comment_lines;
inline std::unordered_set<locc::comment_block, pair_hasher> g_comment_blocks;

inline std::unordered_set<std::string> g_skip_exts = {".exe", ".bin", ".o",	  ".obj", ".a",	  ".lib", ".so",	".dll", ".jpg",
													  ".png", ".tga", ".mtl", ".pdf", ".zip", ".tar", ".ninja", ".txt", ".md"};
inline std::unordered_set<std::string> g_skip_substrs = {"build", "Build", "out", "CMakeFiles"};

inline std::unordered_map<locc::ext, locc::ext_group> g_ext_groups = {{".c", "c-style"},   {".cc", "c-style"},	 {".cpp", "c-style"},
																	  {".h", "c-style"},   {".hpp", "c-style"},	 {".inl", "c-style"},
																	  {".tpp", "c-style"}, {".java", "c-style"}, {".cs", "c-style"}};
inline locc::ext_config g_ext_config = {{"c-style", {{"//"}, {{"/*", "*/"}}}}};

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

namespace locc
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

template <typename F, typename... Args>
void do_if(bool predicate, F f, Args&&... args)
{
	if (predicate)
	{
		f(std::forward<Args>(args)...);
	}
}

template <typename Pred, typename F, typename... Args>
void do_if(Pred predicate, F f, Args&&... args)
{
	if (predicate())
	{
		f(std::forward<Args>(args)...);
	}
}
} // namespace locc
