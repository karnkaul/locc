#pragma once
#include <ktask/queue.hpp>
#include <locc/instance.hpp>
#include <params.hpp>

namespace locc::cli {
struct Counter {
	explicit Counter(Params const& params, InstanceCreateInfo ici);

	void run();

  private:
	void print_params() const;

	void print_progress();

	void print_progress_until_ready(LineCounter const& line_counter);

	static constexpr std::size_t progress_bar_length_v{20};

	Params m_params;
	ktask::Queue m_queue;
	Instance m_locc;

	std::string m_progress_bar{};
	Progress m_progress{};
	std::size_t m_progress_length{};
};
} // namespace locc::cli
