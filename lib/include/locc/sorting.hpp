#pragma once
#include "locc/record.hpp"
#include <klib/base_types.hpp>
#include <algorithm>
#include <array>
#include <compare>
#include <span>

namespace locc {
/// \brief Compares two Metrics.
struct MetricsCompare {
	[[nodiscard]] constexpr auto operator()(Metrics const& lhs, Metrics const& rhs) const -> std::strong_ordering { return lhs[metric] <=> rhs[metric]; }

	/// \brief Metric to compare.
	Metric metric{Metric::Lines};
	/// \brief Sort order. Cannot be std::strong_ordering::equal.
	std::strong_ordering sort_order{std::strong_ordering::greater};
};

/// \brief Compares two Records in a specified order of Metrics.
/// Last comparison is against each Record's category.
struct RecordCompare {
	constexpr auto operator()(Record const& lhs, Record const& rhs) const -> bool {
		for (auto const& compare : metrics_compares) {
			auto const result = compare(lhs.metrics, rhs.metrics);
			if (result == std::strong_ordering::equal) { continue; }
			return result == compare.sort_order;
		}
		return lhs.category < rhs.category;
	}

	/// \brief Ordered list of Metric Compares.
	std::span<MetricsCompare const> metrics_compares{};
};

/// \brief Default ordered list of Metric Compares.
constexpr auto default_metrics_compares_v = std::array{
	MetricsCompare{.metric = Metric::Lines}, MetricsCompare{.metric = Metric::Code},	 MetricsCompare{.metric = Metric::Files},
	MetricsCompare{.metric = Metric::Empty}, MetricsCompare{.metric = Metric::Comments},
};

/// \brief Default Record Compare.
constexpr auto default_record_compare_v = RecordCompare{
	.metrics_compares = default_metrics_compares_v,
};

/// \brief Wrapper for sorting Records.
/// \param records Records to sort.
/// \param record_compare Pass a default constructed instance to sort by category.
inline void sort_records(std::span<Record> records, RecordCompare const& record_compare = default_record_compare_v) {
	std::ranges::sort(records, record_compare);
}
} // namespace locc
