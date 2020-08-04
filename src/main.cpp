#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <args_parser.hpp>
#include <common.hpp>
#include <file_list_generator.hpp>
#include <line_counter.hpp>
#include <table_formatter.hpp>

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
	if (match_any(key, "skip-substr"))
	{
		parse_values(value, [](auto v) { cfg::g_skip_substrs.insert(std::string(v)); });
		return true;
	}
	else if (match_any(key, "skip-ext"))
	{
		parse_values(value, [](auto v) { cfg::g_skip_exts.insert(std::string(v)); });
		return true;
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
	for (auto& [ext, _] : cfg::g_ext_groups)
	{
		cfg::g_ext_passlist.insert(ext);
	}
	for (auto& [ext, _] : cfg::g_ext_config)
	{
		if (ext.at(0) == '.')
		{
			cfg::g_ext_passlist.insert(ext);
		}
	}
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
	locc::log("\n  -- mode: ", cfg::g_mode_names.at((std::size_t)cfg::g_mode), "\n\n");
}

void run_loc(std::deque<stdfs::path> file_paths)
{
	auto result = locc::process(std::move(file_paths));
	if (result.totals.lines.code > 0 || cfg::test(cfg::flag::verbose))
	{
		locc::table_formatter tf;
		if (cfg::test(cfg::flag::verbose))
		{
			tf.add_column("File", true);
			tf.add_column("LOC");
			tf.add_column("Comments");
			tf.add_column("Total");
			for (auto const& file : result.files)
			{
				tf.add_row(file.path.generic_string(), file.lines.code, file.lines.comments, file.lines.total);
			}
			locc::log("\n", tf.to_string(), "\n");
			tf.clear();
		}
		if (!cfg::test(cfg::flag::quiet))
		{
			auto dist = result.transform_dist();
			tf.add_column("Extension", true);
			tf.add_column("LOC");
			tf.add_column("Total");
			tf.add_column("Comments");
			tf.add_column("Files");
			auto sort_index = tf.add_column("Ratio");
			locc::result::ext_data total;
			for (auto const& [ext, data] : dist)
			{
				tf.add_row(ext, data.counts.lines.code, data.counts.lines.total, data.counts.lines.comments, data.counts.files, data.ratio.code);
				total.counts.lines.add(data.counts.lines);
				total.counts.files += data.counts.files;
				total.ratio.add(data.ratio);
			}
			tf.sort(sort_index, true);
			tf.add_row("Total", total.counts.lines.code, total.counts.lines.total, total.counts.lines.comments, total.counts.files, total.ratio.code);
			locc::log("\n", tf.to_string(), "\n");
		}
		else
		{
			locc::log_force(result.totals.lines.code);
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
