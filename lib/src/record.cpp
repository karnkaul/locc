#include "locc/record.hpp"
#include <algorithm>
#include <format>

namespace locc {
auto Record::serialize(Metric const metric, std::string_view const separator) const -> std::string {
	auto const value = metrics[metric];
	auto unbeautified = std::format("{}", value);
	if (separator.empty() || value < 1000) { return unbeautified; }

	auto ret = std::string{};
	std::ranges::reverse(unbeautified);
	auto digits = 0;
	for (char const c : unbeautified) {
		ret.push_back(c);
		if (++digits == 3) {
			ret += separator;
			digits = 0;
		}
	}
	if (ret.back() == ',') { ret.pop_back(); }
	std::ranges::reverse(ret);
	return ret;
}
} // namespace locc
