#pragma once
#include "detail/code_parser.hpp"
#include "detail/parser.hpp"
#include "detail/precision_parser.hpp"
#include "locc/alphabet.hpp"
#include "locc/metrics.hpp"
#include <klib/task/task.hpp>
#include <filesystem>

namespace locc::detail {
namespace fs = std::filesystem;

class Counter : public klib::task::Task {
  public:
	explicit Counter(fs::path path) : m_path(std::move(path)) {}

	void execute() final;

	[[nodiscard]] auto get_metrics() const -> Metrics const&;

  protected:
	virtual void next_line(std::string_view line) = 0;
	[[nodiscard]] virtual auto get_parsed_metrics() const -> Metrics const& = 0;

	fs::path m_path{};

	Metrics m_metrics{};
};

class TextCounter : public Counter {
  public:
	explicit TextCounter(fs::path path) : Counter(std::move(path)) {}

  private:
	void next_line(std::string_view const line) final { m_parser.next_line(line); }
	[[nodiscard]] auto get_parsed_metrics() const -> Metrics const& final { return m_parser.get_metrics(); }

	TextParser m_parser{};
};

template <std::derived_from<TextParser> ParserT>
class CodeCounterBase : public Counter {
  public:
	using ParserType = ParserT;

	explicit CodeCounterBase(Alphabet const& alphabet, fs::path path) : Counter(std::move(path)), m_alphabet(alphabet) {}

  private:
	void next_line(std::string_view const line) final { m_parser.next_line(line); }
	[[nodiscard]] auto get_parsed_metrics() const -> Metrics const& final { return m_parser.get_metrics(); }

	Alphabet const& m_alphabet;
	ParserT m_parser{m_alphabet};
};

using CodeCounter = CodeCounterBase<CodeParser>;
using PrecisionCounter = CodeCounterBase<PrecisionParser>;
} // namespace locc::detail
