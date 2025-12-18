#pragma once
#include "detail/parser.hpp"
#include "locc/alphabet.hpp"

namespace locc::detail {
class CodeParser : public TextParser {
  public:
	explicit CodeParser(Alphabet const& alphabet) : m_alphabet(alphabet) {}

	void next_line(std::string_view line) final;

  private:
	[[nodiscard]] static constexpr auto is_space(char const c) -> bool { return c == ' ' || c == '\t'; }

	[[nodiscard]] auto at_end() const -> bool { return m_remain.empty(); }
	[[nodiscard]] auto current() const -> char { return at_end() ? '\0' : m_remain.front(); }

	void skip_whitespace();
	auto check_comment_closed() -> bool;
	auto in_line_comment() -> bool;
	auto in_block_comment() -> bool;

	void advance(std::size_t const count = 1) { m_remain.remove_prefix(count); }

	Alphabet const& m_alphabet;

	std::string_view m_remain{};

	comment::Block const* m_block_comment{};
};
} // namespace locc::detail
