#pragma once
#include "locc/metrics.hpp"
#include <string>

namespace locc {
/// \brief Consolidated Metrics for a Category.
struct Record {
	/// \param metric Metric to serialize.
	/// \param separator Digit separator.
	[[nodiscard]] auto serialize(Metric metric, std::string_view separator = ",") const -> std::string;

	std::string category{};
	Metrics metrics{};
};
} // namespace locc
