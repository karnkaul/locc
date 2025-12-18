#include "detail/executor.hpp"
#include "locc/error.hpp"
#include "locc/logger.hpp"
#include "locc/sorting.hpp"
#include <klib/task/queue.hpp>

namespace locc::detail {
namespace {
auto const log = Logger<Executor>{};
} // namespace

Executor::Executor(klib::task::Queue* queue, Config const& config) : m_queue(queue), m_heuristic(config.heuristic), m_workspace(config) {}

auto Executor::execute(std::string_view const path) -> Result {
	m_start_time = std::chrono::steady_clock::now();

	m_workspace.path = Workspace::to_absolute_path(path);
	m_result.query_path = m_workspace.path.string();

	if (!fs::exists(m_workspace.path)) { throw Error{std::format("Invalid path: {}", m_workspace.path.string())}; }

	log.info("processing path: {}", m_result.query_path);

	if (fs::is_directory(m_workspace.path)) {
		walk_directory(m_workspace.path);
	} else if (fs::is_regular_file(m_workspace.path)) {
		process_file(m_workspace.path);
	} else {
		log.warn("Unsupported path type: {}", m_workspace.path.string());
		return {};
	}

	prepare_result();

	return std::move(m_result);
}

void Executor::process_file(fs::path path) {
	auto const exclude_match = m_workspace.find_exclude_match(path);
	if (!exclude_match.empty()) {
		if constexpr (klib::log::debug_enabled_v) { log.debug("skipping file: {} (exclude match: {})", path.generic_string(), exclude_match); }
		return;
	}

	auto* entry = m_workspace.get_entry(path.generic_string());
	if (!entry) { return; }

	enqueue_job(std::move(path), *entry);
}

void Executor::enqueue_job(fs::path path, Workspace::Entry& entry) {
	m_jobs.push_back(Job{.counter = create_counter(std::move(path), entry), .entry = &entry});
	if (m_queue && m_queue->can_enqueue()) {
		m_queue->enqueue(*m_jobs.back().counter);
	} else {
		m_jobs.back().counter->execute();
	}
}

void Executor::walk_directory(fs::path const& path) {
	auto const exclude_match = m_workspace.find_exclude_match(path);
	if (!exclude_match.empty()) {
		if constexpr (klib::log::debug_enabled_v) { log.debug("skipping directory: {} (exclude match: {})", path.generic_string(), exclude_match); }
		return;
	}
	for (auto const& it : fs::directory_iterator{path}) {
		if (it.is_directory()) {
			walk_directory(it.path());
			continue;
		}
		if (it.is_symlink()) {
			if constexpr (klib::log::debug_enabled_v) { log.debug("skipping symlink: {}", it.path().generic_string()); }
			continue;
		}
		if (!it.is_regular_file()) {
			if constexpr (klib::log::debug_enabled_v) { log.debug("skipping irregular file: {}", it.path().generic_string()); }
			continue;
		}
		process_file(it.path());
	}
}

void Executor::prepare_result() {
	auto file_count = std::int64_t{};
	for (auto const& job : m_jobs) {
		job.counter->wait();
		auto const& parsed_metrics = job.counter->get_metrics();
		job.entry->metrics += parsed_metrics; // safe to access job.entry here.
		if (parsed_metrics[Metric::Files] > 0) { ++file_count; }
	}
	m_jobs.clear();

	for (auto const& [category, entry] : m_workspace.entries) {
		m_result.records.push_back(Record{.category = std::string{category}, .metrics = entry.metrics});
	}

	sort_records(m_result.records);

	m_result.time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_start_time);

	log.info("counted {} files in {}", file_count, m_result.time_taken);
}

auto Executor::create_counter(fs::path path, Workspace::Entry const& entry) const -> std::unique_ptr<Counter> {
	if (entry.alphabet) {
		switch (m_heuristic) {
		default:
		case Heuristic::Performance: {
			if constexpr (klib::log::debug_enabled_v) { log.debug("creating CodeCounter for {}", path.generic_string()); }
			return std::make_unique<CodeCounter>(*entry.alphabet, std::move(path));
		}
		case Heuristic::Precision: {
			if constexpr (klib::log::debug_enabled_v) { log.debug("creating PrecisionCounter for {}", path.generic_string()); }
			return std::make_unique<PrecisionCounter>(*entry.alphabet, std::move(path));
		}
		}
	}
	if constexpr (klib::log::debug_enabled_v) { log.debug("creating TextCounter for {}", path.generic_string()); }
	return std::make_unique<TextCounter>(std::move(path));
}
} // namespace locc::detail
