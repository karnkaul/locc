#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <args_parser.hpp>
#include <common.hpp>
#include <file_list_generator.hpp>
#include <line_counter.hpp>

namespace
{
void help_summary()
{
	loc::log("\n == TODO (Help Summary) == \n\n");
}

template <typename T, typename... Ts>
bool match_any(T const& lhs, Ts const&... rhs)
{
	return (... || (lhs == rhs));
}

bool parse_options(loc::key const& key, loc::value const& value)
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
		else if (!value.empty())
		{
			cfg::g_ignore_lines.insert(value);
		}
		return true;
	}
	else if (match_any(key, "skip-substr"))
	{
		if (!value.empty())
		{
			cfg::g_skip_substrs.insert(std::move(value));
		}
		return true;
	}
	else if (match_any(key, "skip-ext"))
	{
		if (!value.empty())
		{
			cfg::g_skip_exts.insert(std::move(value));
		}
	}
	else if (match_any(key, "b", "blanks"))
	{
		cfg::set(cfg::flag::blanks);
		return true;
	}
	else if (match_any(key, "o", "one-thread"))
	{
		cfg::set(cfg::flag::one_thread);
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
		return true;
	}
	else if (match_any(key, "q", "quiet"))
	{
		cfg::set(cfg::flag::quiet);
		return true;
	}
	else if (match_any(key, "h", "help"))
	{
		cfg::set(cfg::flag::help);
		return true;
	}
	return false;
}

std::deque<stdfs::path> file_list(std::deque<loc::entry> entries)
{
	for (auto iter = entries.begin(); iter != entries.end();)
	{
		auto& [key, value] = *iter;
		if (parse_options(key, value))
		{
			iter = entries.erase(iter);
		}
		else
		{
			return loc::file_list(entries);
		}
	}
	return {};
}

void print_flags()
{
	loc::log("\n  -- flags:");
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i)
	{
		loc::log(cfg::test((cfg::flag)i), " ", cfg::g_flag_names.at(i));
	}
	if (cfg::g_flags.none())
	{
		loc::log(" [none]");
	}
	loc::log("\n\n");
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
		auto const w_total = result.totals.max_widths.total;
		if (cfg::test(cfg::flag::verbose))
		{
			auto const w_loc = cfg::test(cfg::flag::blanks) ? result.totals.max_widths.total : result.totals.max_widths.loc;
			for (auto const& file : result.files)
			{
				auto const loc = (cfg::test(cfg::flag::blanks) ? file.lines.loc + file.lines.empty : file.lines.loc);
				loc::log("  ", std::setw(w_loc), loc, "\t[ ", std::setw(w_total), file.lines.total, " ]  ", file.path.generic_string(), "\n");
			}
			loc::log("  ", std::setw(w_loc));
		}
		loc::log_force(cfg::test(cfg::flag::blanks) ? result.totals.lines.loc + result.totals.lines.empty : result.totals.lines.loc);
		char const* loc_msg = cfg::test(cfg::flag::blanks) ? "total lines of code (including blanks)" : "total lines of code";
		loc::log(cfg::test(cfg::flag::verbose), "\t[ ", std::setw(w_total), result.totals.lines.total, " ]  ", loc_msg);
		loc::log("\n");
	}
}
} // namespace

int main(int argc, char** argv)
{
	auto entries = loc::parse(argc, argv);
	if (entries.size() < 2)
	{
		help_summary();
		return 0;
	}
	entries.pop_front();
	auto file_paths = file_list(std::move(entries));
	DOIF(cfg::test(cfg::flag::debug), print_flags());
	if (cfg::test(cfg::flag::help))
	{
		help_summary();
		return 0;
	}
	run_loc(file_paths);
}
