#include "ui.hpp"
#include <app/config.hpp>
#include <build_version.hpp>
#include "table_formatter/table_formatter.hpp"

namespace {
template <typename F>
void parse_values(locc::parser::value_view values, F f) {
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

bool locc::parse_options(locc::parser::key const& key, locc::parser::value value) {
	if (match_any(key, "skip-substr")) {
		parse_values(value, [](auto v) { cfg::g_settings.skip_substrs.insert(std::string(v)); });
		return true;
	} else if (match_any(key, "e", "extensions")) {
		parse_values(value, [](auto v) {
			if (v.at(0) == '.') {
				cfg::g_settings.ext_passlist.insert(std::string(v));
			}
		});
		return true;
	} else if (match_any(key, "sort-by")) {
		for (std::size_t i = 0; i < cfg::g_columns.size(); ++i) {
			if (std::string_view(value) == cfg::g_columns.at(i).name) {
				cfg::g_sort_by = (cfg::col)i;
				break;
			}
		}
		return true;
	} else if (match_any(key, "b", "blanks")) {
		cfg::set(cfg::flag::blanks);
		return true;
	} else if (match_any(key, "o", "one-thread")) {
		cfg::set(cfg::flag::one_thread);
		return true;
	} else if (match_any(key, "v", "verbose")) {
		cfg::set(cfg::flag::verbose);
		return true;
	} else if (match_any(key, "d", "debug")) {
		cfg::set(cfg::flag::debug);
		return true;
	} else if (match_any(key, "q", "quiet")) {
		cfg::set(cfg::flag::quiet);
		return true;
	} else if (match_any(key, "s", "symlinks")) {
		cfg::set(cfg::flag::follow_symlinks);
		return true;
	} else if (match_any(key, "settings")) {
		cfg::g_settings.json_path = std::move(value);
		return true;
	}
	if (cfg::g_settings.json_path.empty()) {
		cfg::g_settings.json_path = "locc_settings.json";
	}
	if (cfg::g_settings.import()) {
		locc::log_if(cfg::test(cfg::flag::verbose), "\nImported config from [{}]", cfg::g_settings.json_path.generic_string());
	}
	return false;
}

void locc::print_debug_prologue() {
	log_if(true, "\n  -- flags:");
	for (std::size_t i = 0; i < (std::size_t)cfg::flag::count_; ++i) {
		log_if(true, "{}, {}", cfg::test((cfg::flag)i), cfg::g_flag_names.at(i));
	}
	if (cfg::g_flags.none()) {
		log_if(true, " [none]");
	}
	log_if(true, "\n  -- mode: {}", cfg::g_mode_names.at((std::size_t)cfg::g_mode));
	log_if(true, "\n  -- sort-by: {}", cfg::g_columns.at((std::size_t)cfg::g_sort_by).name);
	log_if(true, "\n\n");
}

void locc::print(locc::result const& result) {
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
			locc::result::file_stats total;
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

void locc::print_help() {
	log_force("\n == TODO (Help Summary) == \n\n");
}

void locc::print_version() {
	log_force(g_version, "\n(", g_git_commit_hash, ")\n");
}
