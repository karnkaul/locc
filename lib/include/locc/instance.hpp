#pragma once
#include <ktask/queue_fwd.hpp>
#include <locc/line_counter.hpp>

namespace locc {
struct InstanceCreateInfo {
	std::vector<Grammar> grammars{get_default_grammars()};
	IFileFilter const* file_filter{&DefaultFileFilter::get_instance()};
};

class Instance {
  public:
	using CreateInfo = InstanceCreateInfo;

	explicit Instance(ktask::Queue& queue, CreateInfo create_info = {});

	[[nodiscard]] auto start_count(std::string_view path) -> std::unique_ptr<LineCounter>;

	[[nodiscard]] auto get_grammars() const -> std::span<Grammar const> { return m_grammars; }

  private:
	ktask::Queue* m_queue;

	std::vector<Grammar> m_grammars;
	IFileFilter const* m_filter;
};
} // namespace locc
