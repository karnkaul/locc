#include "detail/precision_parser.hpp"
#include <algorithm>
#include <cassert>

namespace locc::detail {
namespace {
[[nodiscard]] constexpr auto at_or(std::string_view const text, std::size_t const index, char const fallback = '\0') -> char {
	if (index >= text.size()) { return fallback; }
	return text[index];
}

[[nodiscard]] constexpr auto is_quote(char const c) -> bool { return c == '"' || c == '\''; }
} // namespace

void PrecisionParser::next_line(std::string_view const line) {
	TextParser::next_line(line);
	if (line.empty()) { return; }

	m_remain = line;

	skip_whitespace();

	m_has_code = m_has_comment = false;
	while (!m_remain.empty()) { parse_next(); }
	if (m_has_code) { ++m_metrics[Metric::Code]; }
	if (m_has_comment) { ++m_metrics[Metric::Comments]; }
}

auto PrecisionParser::current() const -> char { return peek(0); }

auto PrecisionParser::peek(std::size_t const lookahead) const -> char { return at_or(m_remain, lookahead); }

void PrecisionParser::advance() {
	assert(!m_remain.empty());
	m_remain.remove_prefix(1);
}

void PrecisionParser::skip_whitespace() {
	while (!m_remain.empty() && std::isspace(static_cast<unsigned char>(current()))) { advance(); }
}

void PrecisionParser::parse_next() {
	if (m_remain.empty()) { return; }

	if (std::holds_alternative<Quoted>(m_state)) {
		m_has_code = true;
		seek_end_quote();
		assert(!std::holds_alternative<Quoted>(m_state) || m_remain.empty());
		return;
	}

	if (std::holds_alternative<Commented>(m_state)) {
		m_has_comment = true;
		seek_comment_close();
		assert(!std::holds_alternative<Commented>(m_state) || m_remain.empty());
		return;
	}

	if (in_quote()) { return; }
	if (in_line_comment()) { return; }
	if (in_block_comment()) { return; }

	m_has_code = true;
	advance();
}

void PrecisionParser::seek_end_quote() {
	auto const quote = std::get<Quoted>(m_state).quote;
	while (!m_remain.empty()) {
		char const c = current();
		advance();
		if (c == '\\') {
			if (!m_remain.empty()) { advance(); }
			continue;
		}
		if (c == quote) {
			m_state = std::monostate{};
			return;
		}
	}
}

void PrecisionParser::seek_comment_close() {
	auto const* comment = std::get<Commented>(m_state).comment;
	auto const i = m_remain.find(comment->close);
	if (i == std::string_view::npos) {
		m_remain = {};
		return;
	}
	m_remain.remove_prefix(i + comment->close.size());
	m_state = std::monostate{};
}

auto PrecisionParser::in_quote() -> bool {
	char const c = current();
	if (is_quote(c)) {
		m_state = Quoted{.quote = c};
		m_has_code = true;
		advance();
		return true;
	}
	return false;
}

auto PrecisionParser::in_line_comment() -> bool {
	auto const func = [this](comment::Line const& comment) {
		if (m_remain.starts_with(comment.token)) {
			m_remain = {};
			m_has_comment = true;
			return true;
		}
		return false;
	};
	return std::ranges::any_of(m_alphabet.line_comments, func);
}

auto PrecisionParser::in_block_comment() -> bool {
	auto const func = [this](comment::Block const& comment) {
		if (m_remain.starts_with(comment.open)) {
			m_state = Commented{.comment = &comment};
			m_has_comment = true;
			return true;
		}
		return false;
	};
	return std::ranges::any_of(m_alphabet.block_comments, func);
}
} // namespace locc::detail
