#include <concepts>
#include <app/config.hpp>
#include <build_version.hpp>
#include <ui/table_formatter/table_formatter.hpp>
#include <ui/ui.hpp>

namespace {
template <typename F>
requires std::invocable<F, std::string_view>
void parse_values(std::string_view values, F f) {
	while (!values.empty()) {
		std::size_t const comma = values.find(',');
		if (comma > 0 && comma != locc::null_index) {
			f(values.substr(0, comma));
			values = values.substr(comma + 1);
		} else {
			if (!values.empty()) {
				f(values);
			}
			values = {};
		}
	}
}
} // namespace

clap::interpreter::spec_t::main_t locc::options_cmd() {
	using spec_t = clap::interpreter::spec_t;
	spec_t::main_t ret;
	spec_t::opt_t opt;
	opt.id = "skip-substr";
	opt.description = "Path substrings to skip (comma separated)";
	opt.value_fmt = "<substr>[,<substr>...]";
	ret.options.push_back(std::move(opt));
	opt.id = "extensions";
	opt.description = "Additional extensions to track";
	opt.value_fmt = "<ext>[,<ext>...]";
	ret.options.push_back(std::move(opt));
	opt.id = "sort-by";
	opt.description = "Sort by";
	opt.value_fmt = "[Column-Name]";
	ret.options.push_back(std::move(opt));
	opt.id = "blanks";
	opt.description = "Show column counting blank lines";
	opt.value_fmt.clear();
	ret.options.push_back(std::move(opt));
	opt.id = "one-thread";
	opt.description = "Restrict program to a single thread";
	ret.options.push_back(std::move(opt));
	opt.id = "skip-symlinks";
	opt.description = "Skip symlinks";
	ret.options.push_back(std::move(opt));
	opt.id = "quiet";
	opt.description = "Quiet mode";
	ret.options.push_back(std::move(opt));
	opt.id = "verbose";
	opt.description = "Verbose mode";
	ret.options.push_back(std::move(opt));
	opt.id = "debug";
	opt.description = "Debug mode";
	ret.options.push_back(std::move(opt));
	ret.callback = [](clap::interpreter::params_t const& params) {
		if (auto val = params.opt_value("skip-substr")) {
			parse_values(*val, [](auto v) { cfg::g_settings.skip_substrs.insert(std::string(v)); });
		}
		if (auto val = params.opt_value("extensions")) {
			parse_values(*val, [](auto v) {
				if (!v.empty() && v[0] == '.') {
					cfg::g_settings.ext_passlist.insert(std::string(v));
				}
			});
		}
		if (auto val = params.opt_value("sort-by")) {
			for (std::size_t i = 0; i < cfg::g_columns.size(); ++i) {
				if (*val == cfg::g_columns.at(i).name) {
					cfg::g_sort_by = static_cast<cfg::col>(i);
					break;
				}
			}
		}
		if (params.opt_value("blanks")) {
			cfg::set(cfg::flag::blanks);
		}
		if (params.opt_value("one-thread")) {
			cfg::set(cfg::flag::one_thread);
		}
		if (params.opt_value("skip-symlinks")) {
			cfg::set(cfg::flag::skip_symlinks);
		}
		if (params.opt_value("quiet")) {
			cfg::set(cfg::flag::quiet);
		}
		if (params.opt_value("verbose")) {
			cfg::set(cfg::flag::verbose);
		}
		if (params.opt_value("debug")) {
			cfg::set(cfg::flag::debug);
		}
	};
	return ret;
}

void locc::print_debug_prologue() {
	log_if(true, "\n  -- flags:");
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i) {
		log_if(true, "{} = {}, ", cfg::g_flag_names.at(i), cfg::test((cfg::flag)i));
	}
	if (cfg::g_flags.none()) {
		log_if(true, " [none]");
	}
	log_if(true, "\n  -- sort-by: {}", cfg::g_columns.at((std::size_t)cfg::g_sort_by).name);
	log_if(true, "\n\n");
}

void locc::print(locc::result_t const& result) {
	if (result.totals.code > 0 || cfg::test(cfg::flag::verbose)) {
		kt::table_formatter tf;
		if (cfg::test(cfg::flag::verbose)) {
			auto sort_index = tf.add_column("File", true);
			tf.add_column("LOC");
			tf.add_column("Total");
			tf.add_column("Comments");
			for (auto const& file : result.files) {
				tf.add_row(file.path.generic_string(), file.lines.code, file.lines.total, file.lines.comments);
			}
			tf.sort(sort_index, false);
			log_if(true, "\n{}\n", tf.to_string());
			tf.clear();
		}
		if (!cfg::test(cfg::flag::quiet)) {
			auto dist = result.transform_dist();
			for (auto const& column : cfg::g_columns) {
				tf.add_column(column.ui_name(), column.reverse);
			}
			locc::result_t::file_stats total;
			for (auto const& [id, data] : dist) {
				tf.add_row(id, data.counts.lines.code, data.counts.lines.total, data.counts.lines.comments, data.counts.files, data.ratio.code);
				total.counts.lines.add(data.counts.lines);
				total.counts.files += data.counts.files;
				total.ratio.add(data.ratio);
			}
			tf.sort((std::uint8_t)cfg::g_sort_by, true);
			tf.add_row("Total", total.counts.lines.code, total.counts.lines.total, total.counts.lines.comments, total.counts.files, total.ratio.code);
			log_if(true, "\n{}\n", tf.to_string());
		} else {
			log_force("{}", result.totals.code);
		}
	}
}

void locc::print_help() { log_force("\n == TODO (Help Summary) == \n\n"); }

void locc::print_version() { log_force(g_version, "\n(", g_git_commit_hash, ")\n"); }
