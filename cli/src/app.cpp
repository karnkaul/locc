#include "app.hpp"
#include "locc/build_version.hpp"
#include "locc/error.hpp"
#include "locc/iequal.hpp"
#include "locc/sorting.hpp"
#include "locc/table.hpp"
#include <klib/args/parse.hpp>
#include <klib/version_str.hpp>
#include <cassert>
#include <print>

namespace locc::cli {
namespace {
auto to_header(std::string_view const sort_by, table::Header& out) -> bool {
	for (auto i = 0; i < int(table::Header::COUNT_); ++i) {
		out = table::Header(i);
		if (iequal(sort_by, table::header_name_v[out])) { return true; }
	}
	return false;
}

auto to_heuristic(std::string_view const in) -> Heuristic {
	for (auto i = 0; i < int(Heuristic::COUNT_); ++i) {
		auto const ret = Heuristic(i);
		if (iequal(in, heuristic_name_v[ret])) { return ret; }
	}
	return Heuristic::Performance;
}

constexpr auto log_level_name_v = klib::EnumArray<klib::log::Level, std::string_view>{"Error", "Warn", "Info", "Debug"};

auto to_log_level(std::string_view const in) -> klib::log::Level {
	static constexpr auto default_v = klib::debug_v ? klib::log::Level::Debug : klib::log::Level::Warn;
	for (auto i = 0; i < int(log_level_name_v.size()); ++i) {
		auto const ret = klib::log::Level(i);
		if (iequal(in, log_level_name_v[ret])) { return ret; }
	}
	return default_v;
}
} // namespace

auto App::run(int const argc, char const* const* const argv) -> int {
	auto const parse_result = parse_args(argc, argv);
	if (parse_result.early_return()) { return parse_result.get_return_code(); }

	set_log_level();
	create_instance();
	m_result = m_instance->process(m_args.path);
	print_result();

	return EXIT_SUCCESS;
}

auto App::parse_args(int const argc, char const* const* const argv) -> klib::args::ParseResult {
	auto version = std::format("{}", build_version_v);
	if constexpr (klib::debug_v) { version += " (debug)"; }
	auto const info = klib::args::ParseInfo{
		.help_text = "lines of code counter",
		.version = version,
	};
	auto const args = std::array{
		klib::args::named_option(m_args.thread_count, "t,threads", "desired number of threads"),
		klib::args::named_option(m_args.heuristic, "h,heuristic", "counting Heuristic (performance/precision)"),
		klib::args::named_option(m_args.spec_json, "s,spec-json", "path to custom Specification JSON"),
		klib::args::named_option(m_args.log_level, "l,log-level", "logging level (error/warn/info/debug)"),
		klib::args::named_option(m_args.sort_by, "sort-by", "sort by (header in lowercase)"),

		klib::args::named_flag(m_args.no_code_families, "no-code-families", "no default Code Families"),
		klib::args::named_flag(m_args.no_text_categories, "no-text-categories", "no default text Categories"),
		klib::args::named_flag(m_args.no_excude_suffixes, "no-exclude-suffixes", "no default exclude suffixes"),

		klib::args::positional_optional(m_args.path, "path"),
	};
	return klib::args::parse_main(info, args, argc, argv);
}

void App::set_log_level() {
	auto const level = to_log_level(m_args.log_level);
	klib::log::set_max_level(level);
	m_log.info("log level set to: {}", log_level_name_v[level]);
	m_log.info("locc {}", build_version_v);
}

void App::create_instance() {
	auto init_info = InitInfo{
		.thread_count = ThreadCount(m_args.thread_count),
		.heuristic = to_heuristic(m_args.heuristic),
		.custom_spec_json = m_args.spec_json,
		.flags = m_args.get_flags(),
	};
	m_instance = Instance::create(std::move(init_info));
	if (!m_instance) { throw Error{"Internal error (failed to create Instance)"}; }
}

void App::print_result() {
	m_log.debug("path: {}", m_result.query_path);
	m_log.debug("time: {}", m_result.time_taken);

	if (!m_args.sort_by.empty()) { sort_result(); }

	auto const total = m_result.compute_total();
	m_result.records.push_back(Record{.category = "Total", .metrics = total});
	auto const text = table::build(m_result.records);
	std::println("{}", text);
}

void App::sort_result() {
	auto sort_by_header = table::Header{};
	if (!to_header(m_args.sort_by, sort_by_header)) {
		m_log.warn("invalid sort-by: {}, ignoring", m_args.sort_by);
		return;
	}

	if (sort_by_header == table::Header::Category) {
		m_log.debug("sorting by Category");
		sort_records(m_result.records, RecordCompare{});
		return;
	}

	auto const metric = to_metric(sort_by_header);
	assert(metric);

	auto const metrics_compare = MetricsCompare{.metric = *metric};
	auto const record_compare = RecordCompare{.metrics_compares = {&metrics_compare, 1}};
	m_log.debug("sorting by Metric: {}", metric_name_v[*metric]);
	sort_records(m_result.records, record_compare);
}

auto App::Args::get_flags() const -> Flag {
	auto ret = Flag{};
	if (no_code_families) { ret |= Flag::NoBuiltinCodeFamilies; }
	if (no_text_categories) { ret |= Flag::NoBuiltinTextCategories; }
	if (no_excude_suffixes) { ret |= Flag::NoBuiltinExcludeSuffixes; }
	return ret;
}
} // namespace locc::cli
