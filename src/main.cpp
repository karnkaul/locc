#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <app/config.hpp>
#include <app/file_list_generator.hpp>
#include <app/line_counter.hpp>
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
}

void run_loc(std::vector<locc::file> files) {
	for (auto const& [group, exts] : cfg::g_settings.ext_groups) {
		if (auto search = cfg::g_settings.comment_infos.find(group); search != cfg::g_settings.comment_infos.end()) {
			for (auto const& ext : exts) {
				cfg::g_settings.comment_infos[ext] = search->second;
			}
		}
	}
	auto result = locc::process(std::move(files));
	locc::print(result);
}
} // namespace

int main(int argc, char** argv) {
	clap::interpreter interpreter;
	auto const expr = interpreter.parse(argc, argv);
	using spec_t = clap::interpreter::spec_t;
	spec_t spec;
	spec.main = locc::options_cmd();
	auto const result = interpreter.interpret(std::cout, std::move(spec), expr);
	if (result == decltype(result)::quit || expr.command.id.empty()) {
		return 0;
	}
	prep_options();
	locc::do_if(cfg::test(cfg::flag::debug), &locc::print_debug_prologue);
	run_loc(locc::file_list(expr.command.id));
}
