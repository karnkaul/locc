#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <args_parser.hpp>
#include <common.hpp>
#include <line_counter.hpp>

#define DOIF(cond, expr) \
	do                   \
	{                    \
		if (cond)        \
		{                \
			expr;        \
		}                \
	} while (0);

namespace
{
void help_summary()
{
	std::cout << "\n == TODO (Help Summary) == \n\n";
}

template <typename T, typename... Ts>
bool match_any(T const& lhs, Ts const&... rhs)
{
	return (... || (lhs == rhs));
}

bool parse_options(ap::key const& key, ap::value const& value)
{
	if (match_any(key, "i", "ignore"))
	{
		std::size_t const comma = value.find(",");
		if (comma != loc::null_index && value.size() > comma)
		{
			auto l = value.substr(0, comma);
			auto r = value.substr(comma + 1);
			cfg::g_ignore_blocks.insert({std::move(l), std::move(r)});
		}
		else
		{
			cfg::g_ignore_lines.insert(value);
		}
		return true;
	}
	else if (match_any(key, "b", "blanks"))
	{
		cfg::set(cfg::flag::blanks);
		return true;
	}
	else if (match_any(key, "v", "verbose"))
	{
		cfg::set(cfg::flag::verbose);
		return true;
	}
	else if (match_any(key, "d", "debug"))
	{
		cfg::set(cfg::flag::debug);
		cfg::set(cfg::flag::verbose);
		return true;
	}
	else if (match_any(key, "h", "help"))
	{
		cfg::set(cfg::flag::help);
		return true;
	}
	return false;
}

bool skip_file(stdfs::path const& path)
{
	if (!stdfs::is_regular_file(path))
	{
		return true;
	}
	auto const ext = path.extension();
	if (ext.empty())
	{
		return true;
	}
	auto const ext_str = ext.generic_string();
	if (std::any_of(cfg::g_skip_ext.begin(), cfg::g_skip_ext.end(), [ext_str](auto skip) -> bool { return std::string_view(skip) == ext_str; }))
	{
		return true;
	}
	auto p = path;
	while (!p.empty() && p.has_parent_path())
	{
		auto const name = p.filename().generic_string();
		if (name.size() > 1 && name.at(0) == '.' && name.at(1) != '.')
		{
			return true;
		}
		if (std::any_of(cfg::g_skip_dirs.begin(), cfg::g_skip_dirs.end(), [name](auto skip) -> bool { return std::string_view(skip) == name; }))
		{
			return true;
		}
		p = p.parent_path();
	}
	if (path.generic_string().find(".git") != std::string::npos)
	{
		return true;
	}
	return false;
}

std::deque<stdfs::path> file_list(std::deque<ap::entry> const& entries)
{
	std::deque<stdfs::path> ret;
	bool reading_files = false;
	for (auto& [key, value] : entries)
	{
		if (!reading_files && !parse_options(key, value))
		{
			auto k = key;
			if (key == "." || key == "..")
			{
				k += "/";
			}
			if (stdfs::is_directory(stdfs::absolute(k)))
			{
				for (auto& path : stdfs::recursive_directory_iterator(k))
				{
					if (!skip_file(path))
					{
						DOIF(cfg::test(cfg::flag::debug), std::cout << "  -- tracking " << path.path().generic_string() << "\n");
						ret.push_back(std::move(path));
					}
				}
				return ret;
			}
			reading_files = true;
		}
		if (reading_files)
		{
			if (!skip_file(stdfs::absolute(key)))
			{
				ret.push_back(std::move(key));
			}
		}
	}
	return ret;
}

void print_flags()
{
	std::cout << "-- flags:";
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i)
	{
		DOIF(cfg::test((cfg::flag)i), std::cout << " " << cfg::g_flag_names.at(i));
	}
	if (cfg::g_flags.none())
	{
		std::cout << " [none]";
	}
	std::cout << "\n\n";
}

void run_loc(std::deque<stdfs::path> file_paths)
{
	if (cfg::g_ignore_lines.empty())
	{
		cfg::g_ignore_lines = {"//"};
	}
	if (cfg::g_ignore_blocks.empty())
	{
		cfg::g_ignore_blocks = {{"/*", "*/"}};
	}
	auto result = loc::process(std::move(file_paths));
	if (result.totals.lines.loc > 0 || cfg::test(cfg::flag::verbose))
	{
		DOIF(cfg::test(cfg::flag::verbose), result.totals.max_widths.loc += 2;);
		auto const w_loc = cfg::test(cfg::flag::blanks) ? result.totals.max_widths.loc + result.totals.max_widths.empty : result.totals.max_widths.loc;
		auto const w_total = result.totals.max_widths.total;
		if (cfg::test(cfg::flag::verbose))
		{
			for (auto const& file : result.files)
			{
				auto const loc = (cfg::test(cfg::flag::blanks) ? file.lines.loc + file.lines.empty : file.lines.loc);
				std::cout << std::setw(w_loc) << loc << "\t[" << std::setw(w_total + 2) << file.lines.total << " ]  " << file.path.generic_string() << "\n";
			}
			std::cout << std::setw(w_loc);
		}
		std::cout << (cfg::test(cfg::flag::blanks) ? result.totals.lines.loc + result.totals.lines.empty : result.totals.lines.loc);
		char const* loc_msg = cfg::test(cfg::flag::blanks) ? "total lines of code (including blanks)" : "total lines of code";
		DOIF(cfg::test(cfg::flag::verbose), std::cout << "\t[" << std::setw(w_total + 2) << result.totals.lines.total << " ]  " << loc_msg);
		std::cout << "\n";
	}
}
} // namespace

int main(int argc, char** argv)
{
	auto entries = ap::parse(argc, argv);
	if (entries.size() < 2)
	{
		help_summary();
		return 0;
	}
	entries.pop_front();
	auto file_paths = file_list(entries);
	DOIF(cfg::test(cfg::flag::debug), print_flags());
	if (cfg::test(cfg::flag::help))
	{
		help_summary();
		return 0;
	}
	run_loc(file_paths);
}
