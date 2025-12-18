#pragma once
#include "detail/parser.hpp"
#include "locc/alphabet.hpp"
#include <variant>

namespace locc::detail {
class PrecisionParser : public TextParser {
  public:
	explicit PrecisionParser(Alphabet const& alphabet) : m_alphabet(alphabet) {}

	void next_line(std::string_view line) final;

  private:
	struct Quoted {
		char quote;
	};
	struct Commented {
		comment::Block const* comment{};
	};
	using State = std::variant<std::monostate, Quoted, Commented>;

	[[nodiscard]] auto current() const -> char;
	[[nodiscard]] auto peek(std::size_t lookahead = 1) const -> char;
	void advance();

	void skip_whitespace();
	void parse_next();
	void seek_end_quote();
	void seek_comment_close();
	auto in_quote() -> bool;
	auto in_line_comment() -> bool;
	auto in_block_comment() -> bool;

	Alphabet const& m_alphabet{};

	std::string_view m_remain{};

	State m_state{};
	bool m_has_code{};
	bool m_has_comment{};
};
} // namespace locc::detail
