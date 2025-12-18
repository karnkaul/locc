#include "detail/code_parser.hpp"
#include <algorithm>
#include <cassert>

namespace locc::detail {
void CodeParser::next_line(std::string_view const line) {
	TextParser::next_line(line);
	if (line.empty()) { return; }

	m_remain = line;
	skip_whitespace();

	if (m_block_comment) {
		++m_metrics[Metric::Comments];
		check_comment_closed();
		return;
	}

	if (in_line_comment()) {
		++m_metrics[Metric::Comments];
		return;
	}

	if (in_block_comment()) {
		++m_metrics[Metric::Comments];
		return;
	}

	++m_metrics[Metric::Code];
}

auto CodeParser::check_comment_closed() -> bool {
	assert(m_block_comment);
	auto const i = m_remain.find(m_block_comment->close);
	if (i == std::string_view::npos) { return false; }
	advance(i + m_block_comment->close.size());
	m_block_comment = {};
	return true;
}

void CodeParser::skip_whitespace() {
	while (!at_end() && is_space(current())) { advance(); }
}

auto CodeParser::in_line_comment() -> bool {
	auto const func = [this](comment::Line const& comment) { return m_remain.starts_with(comment.token); };
	return std::ranges::any_of(m_alphabet.line_comments, func);
}

auto CodeParser::in_block_comment() -> bool {
	auto const func = [this](comment::Block const& comment) {
		if (m_remain.starts_with(comment.open)) {
			advance(comment.open.size());
			m_block_comment = &comment;
			if (!check_comment_closed()) { return true; }
		}
		return false;
	};
	return std::ranges::any_of(m_alphabet.block_comments, func);
}
} // namespace locc::detail
