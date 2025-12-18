#pragma once
#include "locc/record.hpp"
#include <chrono>
#include <vector>

namespace locc {
/// \brief Collection of Records.
struct Result {
	/// \returns Metrics aggregated from all records.
	[[nodiscard]] auto compute_total() const -> Metrics;

	std::string query_path{};
	std::vector<Record> records{};
	std::chrono::milliseconds time_taken{};
};
} // namespace locc
