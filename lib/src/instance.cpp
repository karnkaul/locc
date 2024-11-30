#include <klib/task/queue.hpp>
#include <locc/count_lines.hpp>
#include <locc/instance.hpp>

namespace locc {
Instance::Instance(klib::task::Queue& queue, CreateInfo create_info)
	: m_queue(&queue), m_grammars(std::move(create_info.grammars)), m_filter(create_info.file_filter) {}

auto Instance::start_count(std::string_view const path) -> std::unique_ptr<LineCounter> {
	if (m_grammars.empty()) { return {}; }
	auto const query = Query{
		.grammars = m_grammars,
		.path = path,
		.filter = m_filter,
	};
	auto ret = std::make_unique<LineCounter>(*m_queue, query);
	m_queue->enqueue(*ret);
	return ret;
}
} // namespace locc
