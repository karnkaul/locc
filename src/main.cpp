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
	locc::log("\n == TODO (Help Summary) == \n\n");
}

template <typename T, typename... Ts>
bool match_any(T const& lhs, Ts const&... rhs)
{
	return (... || (lhs == rhs));
}

template <typename F>
void parse_values(locc::value_view values, F f)
{
	while (!values.empty())
	{
		std::size_t const comma = values.find(",");
		if (comma != locc::null_index)
		{
			f(values.substr(0, comma));
			values = values.substr(comma + 1);
		}
		else
		{
			f(values);
			values = {};
		}
	}
}

bool parse_options(locc::key const& key, locc::value value)
{
	if (match_any(key, "i", "ignore"))
	{
		std::size_t const comma = value.find(",");
		if (comma != locc::null_index && value.size() > comma)
		{
			auto l = value.substr(0, comma);
			auto r = value.substr(comma + 1);
			cfg::g_comment_blocks.insert({std::move(l), std::move(r)});
		}
		else if (!value.empty())
		{
			cfg::g_comment_lines.insert(std::move(value));
		}
		return true;
	}
	else if (match_any(key, "skip-substr"))
	{
		parse_values(value, [](auto v) { cfg::g_skip_substrs.insert(std::string(v)); });
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
	else if (match_any(key, "foo"))
	{
		return true;
	}
	return false;
}

std::deque<stdfs::path> file_list(std::deque<locc::entry> entries)
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
			return locc::file_list(std::move(entries));
		}
	}
	return {};
}

void print_flags()
{
	locc::log("\n  -- flags:");
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i)
	{
		locc::log(cfg::test((cfg::flag)i), " ", cfg::g_flag_names.at(i));
	}
	if (cfg::g_flags.none())
	{
		locc::log(" [none]");
	}
	locc::log("\n mode: ", cfg::g_mode_names.at((std::size_t)cfg::g_mode), "\n\n");
}

void run_loc(std::deque<stdfs::path> file_paths)
{
	if (cfg::g_comment_lines.empty())
	{
		cfg::g_comment_lines = {"//"};
	}
	if (cfg::g_comment_blocks.empty())
	{
		cfg::g_comment_blocks = {{"/*", "*/"}};
	}
	auto result = locc::process(std::move(file_paths));
	if (result.totals.lines.code > 0 || cfg::test(cfg::flag::verbose))
	{
		auto const w_loc = cfg::test(cfg::flag::blanks) ? result.totals.max_widths.total : result.totals.max_widths.code;
		auto const w_total = result.totals.max_widths.total;
		if (cfg::test(cfg::flag::verbose))
		{
			for (auto const& file : result.files)
			{
				auto const loc = (cfg::test(cfg::flag::blanks) ? file.lines.code + file.lines.blank : file.lines.code);
				locc::log("  ", std::setw(w_loc), loc, "  [ ", std::setw(w_total), file.lines.total, " ]  ", file.path.generic_string(), "\n");
			}
			locc::log("  ");
		}
		locc::log(!cfg::test(cfg::flag::quiet), std::setw(w_loc));
		locc::log_force(result.totals.lines.code);
		char const* loc_msg = cfg::test(cfg::flag::blanks) ? "total lines of code (including blanks)" : "total lines of code";
		locc::log(!cfg::test(cfg::flag::quiet), "  [ ", std::setw(w_total), result.totals.lines.total, " ]  ", loc_msg);
		locc::log("\n");
		auto dist = result.transform_dist();
		locc::log("Ext | LOC | Total | Comments | Ratio\n");
		for (auto const& [ext, data] : dist)
		{
			locc::log(ext, " : ", data.counts.lines.code, ", ", data.counts.lines.total, ", ", data.counts.lines.comments, ", ", data.ratio, "\n");
		}
	}
}
} // namespace

int main(int argc, char** argv)
{
	auto entries = locc::parse(argc, argv);
	if (entries.size() < 2)
	{
		help_summary();
		return 0;
	}
	entries.pop_front();
	auto file_paths = file_list(std::move(entries));
	locc::do_if(cfg::test(cfg::flag::debug), &print_flags);
	if (cfg::test(cfg::flag::help))
	{
		help_summary();
		return 0;
	}
	for (auto const& [ext, group] : cfg::g_ext_groups)
	{
		if (auto search = cfg::g_ext_config.find(group); search != cfg::g_ext_config.end())
		{
			cfg::g_ext_config[ext] = search->second;
		}
	}
	run_loc(file_paths);
}
