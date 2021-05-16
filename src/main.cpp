#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <app/async_counter.hpp>
#include <app/config.hpp>
#include <build_version.hpp>
#include <clap/interpreter.hpp>
#include <ui/ui.hpp>

namespace {
void run(stdfs::path const& root) {
	locc::result_t result{};
	if (stdfs::is_directory(root)) {
		locc::async_counter counter(&result);
		bool const blanks = cfg::test(cfg::flag::blanks);
		for (auto const& it : stdfs::recursive_directory_iterator(root, stdfs::directory_options::skip_permission_denied)) {
			if (!cfg::test(cfg::flag::skip_symlinks) || !it.is_symlink()) {
				try {
					if (auto file = cfg::g_settings.include_file(it.path())) {
						locc::log_if(cfg::test(cfg::flag::debug) && cfg::test(cfg::flag::verbose), "  -- tracking {}\n ", it.path().generic_string());
						counter.count(std::move(*file), blanks);
					}
				} catch (std::exception const& e) { locc::err_if(cfg::test(cfg::flag::debug), e.what(), "\n"); }
			}
		}
	}
	locc::print(result);
}
} // namespace

int main(int argc, char** argv) {
	if (argc < 1) { return 1; }
	clap::interpreter interpreter;
	interpreter.m_command = false;
	auto const expr = interpreter.parse(argc, argv);
	using spec_t = clap::interpreter::spec_t;
	spec_t spec;
	spec.main = locc::options_cmd();
	spec.main.exe = stdfs::path(argv[0]).filename().string();
	spec.main.version = locc::g_version;
	auto const result = interpreter.interpret(std::cout, std::move(spec), expr);
	if (result != decltype(result)::quit && !expr.arguments.empty()) {
		cfg::g_settings.prep();
		locc::do_if(cfg::test(cfg::flag::debug), &locc::print_debug_prologue);
		run(expr.arguments.front());
	}
}
