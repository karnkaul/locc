#include "detail/counter.hpp"
#include "locc/logger.hpp"
#include <fstream>

namespace locc::detail {
namespace {
auto const log = Logger<Counter>{};
} // namespace

auto Counter::get_metrics() const -> Metrics const& {
	if (is_busy()) {
		log.warn("get_metrics() called while is_busy()");
		static auto const s_default = Metrics{};
		return s_default;
	}
	return m_metrics;
}

void Counter::execute() {
	try {
		auto file = std::ifstream{m_path};
		if (!file) { return; }

		++m_metrics[Metric::Files];

		auto log_path = std::string{};
		if constexpr (klib::log::debug_enabled_v) { log_path = m_path.generic_string(); }

		log.debug("parsing '{}'...", log_path);
		for (auto line = std::string{}; !file.eof();) {
			std::getline(file, line);
			next_line(line);
		}

		m_metrics += get_parsed_metrics();
		log.debug("... finished parsing '{}'", log_path);
	} catch (std::exception const& e) { log.error("failed to process file '{}': {}", m_path.string(), e.what()); }
}
} // namespace locc::detail
