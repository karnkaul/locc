#include "detail/parser.hpp"

namespace locc::detail {
void TextParser::next_line(std::string_view const line) {
	++m_metrics[Metric::Lines];
	check_empty(line);
}

void TextParser::check_empty(std::string_view const line) {
	if (!line.empty()) { return; }
	++m_metrics[Metric::Empty];
}
} // namespace locc::detail
