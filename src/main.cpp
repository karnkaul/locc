#include <algorithm>
#include <fstream>
#include <iomanip>
#include <optional>
#include <stdexcept>
#include <app/config.hpp>
#include <app/file_list_generator.hpp>
#include <app/line_counter.hpp>
#include <build_version.hpp>
#include <clap/interpreter.hpp>
#include <ui/ui.hpp>

namespace {
void prep_options() {
	for (auto const& [_, exts] : cfg::g_settings.ext_groups) {
		for (auto const& ext : exts) {
			cfg::g_settings.ext_passlist.insert(ext);
		}
	}
	for (auto const& [ext, _] : cfg::g_settings.comment_infos) {
		if (ext.at(0) == '.') {
			cfg::g_settings.ext_passlist.insert(ext);
		}
	}
	for (auto const& [id, exts] : cfg::g_settings.id_groups) {
		for (auto const& ext : exts) {
			cfg::g_settings.ext_to_id[ext] = id;
		}
	}
	for (auto const& [ext, _] : cfg::g_settings.ext_to_id) {
		cfg::g_settings.ext_passlist.insert(ext);
	}
	for (auto const& [group, exts] : cfg::g_settings.ext_groups) {
		if (auto const search = cfg::g_settings.comment_infos.find(group); search != cfg::g_settings.comment_infos.end()) {
			for (auto const& ext : exts) {
				cfg::g_settings.comment_infos[ext] = search->second;
			}
		}
	}
}

std::optional<locc::file_t> include_file(stdfs::path const& path) {
	if (!stdfs::is_regular_file(stdfs::absolute(path))) {
		return std::nullopt;
	}
	if (path.generic_string().find(".git/") != std::string::npos) {
		return std::nullopt;
	}
	auto const filename = path.filename().generic_string();
	if (cfg::g_settings.filenames_as_ext.find(filename) != cfg::g_settings.filenames_as_ext.end()) {
		return locc::file_t{.path = path, .ext = filename, .id = cfg::g_settings.get_id(filename), .lines = {}};
	}
	auto const ext = path.extension().generic_string();
	if (ext.empty()) {
		return std::nullopt;
	}
	if (std::none_of(cfg::g_settings.ext_passlist.begin(), cfg::g_settings.ext_passlist.end(), [&ext](auto skip) { return skip == ext; })) {
		return std::nullopt;
	}
	auto const& skip = cfg::g_settings.skip_substrs;
	auto const path_str = path.generic_string();
	if (std::any_of(skip.begin(), skip.end(), [&path_str](auto const& skip) { return path_str.find(skip) < path_str.size(); })) {
		return std::nullopt;
	}
	return locc::file_t{.path = path, .ext = ext, .id = cfg::g_settings.get_id(ext), .lines = {}};
}

void run(stdfs::path const& root) {
	locc::result_t result{};
	if (stdfs::is_directory(root)) {
		locc::counter_t counter(&result);
		bool const blanks = cfg::test(cfg::flag::blanks);
		for (auto const& it : stdfs::recursive_directory_iterator(root, stdfs::directory_options::skip_permission_denied)) {
			if (!cfg::test(cfg::flag::skip_symlinks) || (!it.is_symlink() && it.exists() && it.is_regular_file() && it.file_size() > 0)) {
				stdfs::path path;
				try {
					if (auto file = include_file(it.path())) {
						locc::log_if(cfg::test(cfg::flag::debug) && cfg::test(cfg::flag::verbose), "  -- tracking {}\n ", it.path().generic_string());
						counter.count(std::move(*file), blanks);
					}
				} catch (std::exception const& e) {
					locc::err_if(cfg::test(cfg::flag::debug), e.what(), "\n");
				}
			}
		}
	}
	locc::print(result);
}
} // namespace

int main(int argc, char** argv) {
	clap::interpreter interpreter;
	interpreter.m_command = false;
	auto const expr = interpreter.parse(argc, argv);
	using spec_t = clap::interpreter::spec_t;
	spec_t spec;
	spec.main = locc::options_cmd();
	spec.main.exe = stdfs::path(argv[0]).filename().string();
	spec.main.version = locc::g_version;
	auto const result = interpreter.interpret(std::cout, std::move(spec), expr);
	if (result == decltype(result)::quit || expr.arguments.empty()) {
		return 0;
	}
	prep_options();
	locc::do_if(cfg::test(cfg::flag::debug), &locc::print_debug_prologue);
	/*auto files = locc::file_list(expr.arguments.front());
	run_loc(files);*/
	run(expr.arguments.front());
}
