#include <app.hpp>
#include <counter.hpp>
#include <klib/args/parse.hpp>
#include <klib/task/queue.hpp>
#include <array>
#include <print>

namespace locc::cli {
namespace {
struct Csv {
	std::string_view remain{};

	constexpr auto operator()(std::string_view& out) -> bool {
		if (remain.empty()) { return false; }
		auto const i = remain.find_first_of(',');
		if (i == std::string_view::npos) {
			out = remain;
			remain = {};
		} else {
			out = remain.substr(0, i);
			remain = remain.substr(i + 1);
		}
		return true;
	}
};
} // namespace

auto App::run(int const argc, char const* const* argv) -> int {
	m_params.thread_count = std::uint8_t(klib::task::get_max_threads());

	auto default_grammars = false;

	auto const args = std::array{
		klib::args::option(m_params.sort_column, "c,sort-column", "sort by COLUMN_NAME (default Lines)"),
		klib::args::option(m_params.exclude_patterns, "e,exclude", "comma-separated exclude list (will not be counted)"),
		klib::args::option(m_params.grammars_json, "g,grammars", "path to JSON containing an array of custom grammars"),
		klib::args::option(m_params.thread_count, "t,threads", "number of threads to use"),
		klib::args::flag(m_params.sort_ascending, "a,sort-ascend", "sort in ascending order"),
		klib::args::flag(m_params.no_progress, "p,no-progress", "do not print progress while counting"),
		klib::args::flag(default_grammars, "default-grammars", "output default grammars as JSON and exit"),
		klib::args::flag(m_params.verbose, "v,verbose", "verbose output"),
		klib::args::positional(m_params.path, klib::args::optional_v, "path", "path to count for"),
	};

	auto const info = klib::args::ParseInfo{
		.help_text = "lines of code counter",
		.version = locc::version_v,
	};

	auto const parse_result = klib::args::parse(info, args, argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	if (default_grammars) {
		std::println("{}", serialize_default_grammars());
		return EXIT_SUCCESS;
	}

	return execute();
}

auto App::execute() -> int {
	if (m_params.thread_count <= 1) { m_params.thread_count = 2; }

	auto ici = InstanceCreateInfo{.file_filter = &m_filter};

	if (!m_params.grammars_json.empty()) {
		ici.grammars.clear();
		if (load_grammars_into(ici.grammars, m_params.grammars_json.data()) == 0) {
			std::println(stderr, "Failed to load grammars from '{}'", m_params.grammars_json);
			return EXIT_FAILURE;
		}
	}

	auto csv = Csv{m_params.exclude_patterns};
	for (auto pattern = std::string_view{}; csv(pattern);) { m_filter.exclude_patterns.push_back(pattern); }

	Counter{m_params, std::move(ici)}.run();
	return EXIT_SUCCESS;
}
} // namespace locc::cli
