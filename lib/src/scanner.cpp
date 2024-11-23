#include <scanner.hpp>
#include <util.hpp>
#include <algorithm>
#include <cassert>

namespace locc {
auto Scanner::get_type(std::string_view const line) -> LocType {
	if (line.empty()) { return LocType::Empty; }

	if (!m_grammar.is_code()) { return LocType::None; }

	m_remaining = trim_spaces(line);
	if (is_line_comment()) { return LocType::Comment; }

	m_found_code = false;

	while (!m_remaining.empty()) {
		auto const bc_closed = [&] {
			if (in_block_comment()) { return bc_close(); }
			return false;
		}();
		if (!bc_closed && m_current_bc != nullptr) { return LocType::Comment; }
		auto const bc_opened = bc_open();
		if (!bc_opened && !bc_closed && !m_remaining.empty()) {
			m_found_code = true;
			next_word();
		}
	}

	return m_found_code ? LocType::Code : LocType::Comment;
}

auto Scanner::is_line_comment() const -> bool {
	if (m_grammar.line_comments.empty()) { return false; }
	return std::ranges::any_of(m_grammar.line_comments, [this](LineComment const& lc) { return m_remaining.starts_with(lc.prefix); });
}

auto Scanner::bc_open() -> bool {
	auto const it = std::ranges::find_if(m_grammar.block_comments, [this](BlockComment const& bc) { return m_remaining.starts_with(bc.open); });
	if (it == m_grammar.block_comments.end()) { return false; }

	m_current_bc = &*it;
	m_remaining = m_remaining.substr(m_current_bc->open.size());
	if (!bc_close()) { m_remaining = {}; }
	return true;
}

auto Scanner::bc_close() -> bool {
	assert(m_current_bc);
	auto const i = m_remaining.find(m_current_bc->close);
	if (i == std::string_view::npos) { return false; }
	m_remaining = m_remaining.substr(i + m_current_bc->close.size());
	m_current_bc = nullptr;
	return true;
}

void Scanner::next_word() {
	// advance to first space
	while (!m_remaining.empty() && !is_space(m_remaining.front())) { m_remaining = m_remaining.substr(1); }
	// advance to next non-space
	while (!m_remaining.empty() && is_space(m_remaining.front())) { m_remaining = m_remaining.substr(1); }
}
} // namespace locc
