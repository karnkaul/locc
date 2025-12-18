#pragma once
#include "locc/instance.hpp"
#include "locc/logger.hpp"
#include <klib/args/parse_result.hpp>

namespace locc::cli {
class App {
  public:
	auto run(int argc, char const* const* argv) -> int;

  private:
	struct Args {
		[[nodiscard]] auto get_flags() const -> Flag;

		int thread_count{int(get_max_threads())};
		std::string_view path{};
		std::string_view sort_by{};
		std::string_view heuristic{};
		std::string_view spec_json{};
		std::string_view log_level{};
		bool no_code_families{};
		bool no_text_categories{};
		bool no_excude_suffixes{};
	};

	auto parse_args(int argc, char const* const* argv) -> klib::args::ParseResult;

	void set_log_level();
	void create_instance();
	void print_result();
	void sort_result();

	Logger<App> m_log{};

	Args m_args{};

	std::unique_ptr<Instance> m_instance{};
	Result m_result{};
};
} // namespace locc::cli
