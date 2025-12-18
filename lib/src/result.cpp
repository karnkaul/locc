#include "locc/result.hpp"

namespace locc {
auto Result::compute_total() const -> Metrics {
	auto ret = Metrics{};
	for (auto const& record : records) { ret += record.metrics; }
	return ret;
}
} // namespace locc
