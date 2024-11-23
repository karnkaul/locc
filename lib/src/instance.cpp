#include <ktask/queue.hpp>
#include <locc/count_lines.hpp>
#include <locc/instance.hpp>

namespace locc {
Instance::Instance(ktask::Queue& queue, CreateInfo create_info)
	: m_queue(&queue), m_grammars(std::move(create_info.grammars)), m_filter(create_info.file_filter) {}

auto Instance::start_count(std::string_view const path) -> LineCounter {
	auto const query = Query{
		.grammars = m_grammars,
		.path = path,
		.filter = m_filter,
	};
	return LineCounter{*m_queue, query};
}
} // namespace locc
