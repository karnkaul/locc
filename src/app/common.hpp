#pragma once
#include <deque>
#include <iostream>
#include <filesystem>
#include <map>
#include <string_view>
#include <unordered_map>

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

template <typename T, typename... Ts>
bool match_any(T const& lhs, Ts const&... rhs)
{
	return (... || (lhs == rhs));
}

struct config final
{
	std::deque<comment_line> comment_lines;
	std::deque<comment_block> comment_blocks;
};
using ext_config = std::unordered_map<ext, config>;

constexpr std::size_t null_index = std::string::npos;

template <typename T>
struct lines_t
{
	T code = {};
	T comments = {};
	T total = {};

	void add(lines_t<T> const& rhs)
	{
		code += rhs.code;
		comments += rhs.comments;
		total += rhs.total;
	}

	template <typename U>
	void divide(lines_t<U> const& num, lines_t<U> const& den)
	{
		code = (T)num.code / (T)den.code;
		comments = (T)num.comments / (T)den.comments;
		total = (T)num.total / (T)den.total;
	}
};

template <typename T>
struct lines_blank_t : lines_t<T>
{
	T blank = {};
};

using lines = lines_t<std::size_t>;
using lines_blank = lines_blank_t<std::size_t>;
using ratio = lines_t<float>;

struct file final
{
	stdfs::path path;
	std::string ext;
	std::string id;
	lines_blank lines;
};

struct result final
{
	struct file_stats
	{
		struct
		{
			lines lines;
			std::size_t files = {};
		} counts;
		ratio ratio;
	};
	using distribution = std::unordered_map<std::string, file_stats>;

	distribution dist;
	std::deque<file> files;
	lines totals;

	template <template <typename...> typename Cont = std::map, typename Pred, typename... Args>
	auto transform_dist(Pred predicate) const
	{
		Cont<std::string, file_stats, Args...> ret;
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
} // namespace locc
