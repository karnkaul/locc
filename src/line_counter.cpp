#include <fstream>
#include <line_counter.hpp>

namespace
{
constexpr std::size_t g_DEBUG_skip_to_line = 124;

bool ignore_line(std::string_view line)
{
	for (auto& str : cfg::g_ignore_lines)
	{
		if (auto const search = line.find(str); search == 0)
		{
			return true;
		}
	}
	return false;
}

std::string_view trim(std::string_view sub_line)
{
	while (!sub_line.empty() && std::isspace(sub_line.at(0)))
	{
		sub_line = sub_line.substr(1);
	}
	if (!sub_line.empty())
	{
		if (ignore_line(sub_line))
		{
			return {};
		}
		for (auto& pair : cfg::g_ignore_blocks)
		{
			if (auto const start = sub_line.find(pair.first); start != loc::null_index)
			{
				auto const end = sub_line.find(pair.second);
				if (end != loc::null_index && end + pair.second.size() < sub_line.size())
				{
					return trim(sub_line.substr(end + pair.second.size()));
				}
				return sub_line;
			}
		}
	}
	return sub_line;
}

loc::ignore_block const* start_ignoring(std::string_view sub_line)
{
	for (auto& pair : cfg::g_ignore_blocks)
	{
		if (auto const start = sub_line.find(pair.first); start != loc::null_index)
		{
			auto const end = sub_line.find(pair.second);
			if (end == loc::null_index || end >= sub_line.size())
			{
				return &pair;
			}
			return start_ignoring(sub_line.substr(end));
		}
	}
	return nullptr;
}

bool count_line(std::string_view line, loc::ignore_block const*& out_ignoring)
{
	line = trim(line);
	if (!line.empty())
	{
		if (out_ignoring)
		{
			if (auto const end = line.find(out_ignoring->second); end != loc::null_index)
			{
				auto const offset = out_ignoring->second.size();
				out_ignoring = nullptr;
				return count_line(line.substr(end + offset), out_ignoring);
			}
		}
		else
		{
			out_ignoring = start_ignoring(line);
			return out_ignoring == nullptr;
		}
	}
	return false;
}

loc::file count_lines(stdfs::path file)
{
	loc::file ret;
	std::ifstream f(file, std::ios::in);
	if (f.good())
	{
		ret.lines.loc = ret.lines.total = 1;
		std::string line;
		loc::ignore_block const* ignoring = nullptr;
		while (std::getline(f, line))
		{
			++ret.lines.total;
			if (cfg::test(cfg::flag::debug) && g_DEBUG_skip_to_line == ret.lines.total)
			{
				;
			}
			while (!line.empty() && std::isspace(line.at(0)))
			{
				line = line.substr(1);
			}
			if (line.empty())
			{
				if (!ignoring)
				{
					++ret.lines.empty;
				}
			}
			else
			{
				if (count_line(line, ignoring))
				{
					++ret.lines.loc;
				}
			}
		}
	}
	ret.path = std::move(file);
	return ret;
}

std::size_t width(std::size_t number)
{
	std::size_t ret = 0;
	while (number > 0)
	{
		number /= 10;
		++ret;
	}
	return ret;
}
} // namespace

loc::result loc::process(std::deque<stdfs::path> file_paths)
{
	result ret;
	for (auto& file_path : file_paths)
	{
		auto file = count_lines(std::move(file_path));
		ret.totals.lines.total += file.lines.total;
		ret.totals.lines.loc += file.lines.loc;
		ret.totals.lines.empty += file.lines.empty;
		ret.totals.max_widths.total = std::max(ret.totals.max_widths.total, width(file.lines.total));
		ret.totals.max_widths.loc = std::max(ret.totals.max_widths.loc, width(file.lines.loc));
		ret.totals.max_widths.empty = std::max(ret.totals.max_widths.empty, width(file.lines.empty));
		ret.files.push_back(std::move(file));
	}
	return ret;
}
