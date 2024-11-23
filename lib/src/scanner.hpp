#pragma once
#include <loc_type.hpp>
#include <locc/grammar.hpp>
#include <string_view>

namespace locc {
struct Scanner {
	explicit Scanner(Grammar const& grammar) : m_grammar(grammar) {}

	[[nodiscard]] auto get_type(std::string_view line) -> LocType;
	[[nodiscard]] auto in_block_comment() const -> bool { return m_current_bc != nullptr; }

  private:
	[[nodiscard]] auto is_line_comment() const -> bool;

	auto bc_open() -> bool;
	auto bc_close() -> bool;
	void next_word();

	Grammar const& m_grammar{};

	// per line
	std::string_view m_remaining{};
	bool m_found_code{};

	// persistent
	BlockComment const* m_current_bc{};
};
} // namespace locc
