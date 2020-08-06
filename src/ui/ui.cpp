#include <app/config.hpp>
#include <build_version.hpp>
#include "ui.hpp"
#include "table_formatter/table_formatter.hpp"

namespace
{
template <typename F>
void parse_values(locc::parser::value_view values, F f)
{
	while (!values.empty())
	{
		std::size_t const comma = values.find(',');
		if (comma > 0 && comma != locc::null_index)
		{
			f(values.substr(0, comma));
			values = values.substr(comma + 1);
		}
		else
		{
			if (!values.empty())
			{
				f(values);
			}
			values = {};
		}
	}
}
} // namespace

bool locc::parse_options(locc::parser::key const& key, locc::parser::value value)
{
	if (match_any(key, "skip-substr"))
	{
		parse_values(value, [](auto v) { cfg::g_skip_substrs.insert(std::string(v)); });
		return true;
	}
	else if (match_any(key, "e", "extensions"))
	{
		parse_values(value, [](auto v) {
			if (v.at(0) == '.')
			{
				cfg::g_ext_passlist.insert(std::string(v));
			}
		});
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
	else if (match_any(key, "s", "symlinks"))
	{
		cfg::set(cfg::flag::follow_symlinks);
		return true;
	}
	else if (match_any(key, "foo"))
	{
		return true;
	}
	return false;
}

void locc::print_debug_prologue()
{
	log("\n  -- flags:");
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i)
	{
		log(cfg::test((cfg::flag)i), " ", cfg::g_flag_names.at(i));
	}
	if (cfg::g_flags.none())
	{
		log(" [none]");
	}
	log("\n  -- mode: ", cfg::g_mode_names.at((std::size_t)cfg::g_mode), "\n\n");
}

void locc::print(locc::result const& result)
{
	if (result.totals.code > 0 || cfg::test(cfg::flag::verbose))
	{
		kt::table_formatter tf;
		if (cfg::test(cfg::flag::verbose))
		{
			auto sort_index = tf.add_column("File", true);
			tf.add_column("LOC");
			tf.add_column("Total");
			tf.add_column("Comments");
			for (auto const& file : result.files)
			{
				tf.add_row(file.path.generic_string(), file.lines.code, file.lines.total, file.lines.comments);
			}
			tf.sort(sort_index, false);
			log("\n", tf.to_string(), "\n");
			tf.clear();
		}
		if (!cfg::test(cfg::flag::quiet))
		{
			auto dist = result.transform_dist();
			tf.add_column("File", true);
			tf.add_column("LOC");
			tf.add_column("Total");
			tf.add_column("Comments");
			tf.add_column("Files");
			auto sort_index = tf.add_column("Ratio");
			locc::result::file_stats total;
			for (auto const& [id, data] : dist)
			{
				tf.add_row(id, data.counts.lines.code, data.counts.lines.total, data.counts.lines.comments, data.counts.files, data.ratio.code);
				total.counts.lines.add(data.counts.lines);
				total.counts.files += data.counts.files;
				total.ratio.add(data.ratio);
			}
			tf.sort(sort_index, true);
			tf.add_row("Total", total.counts.lines.code, total.counts.lines.total, total.counts.lines.comments, total.counts.files, total.ratio.code);
			log("\n", tf.to_string(), "\n");
		}
		else
		{
			log_force(result.totals.code);
		}
	}
}

void locc::print_help()
{
	log_force("\n == TODO (Help Summary) == \n\n");
}

void locc::print_version()
{
	log_force(g_version, "\n(", g_git_commit_hash, ")\n");
}
