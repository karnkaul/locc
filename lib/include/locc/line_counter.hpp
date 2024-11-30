#pragma once
#include <klib/task/queue_fwd.hpp>
#include <klib/task/task.hpp>
#include <locc/grammar.hpp>
#include <locc/progress.hpp>
#include <locc/query.hpp>
#include <locc/row.hpp>
#include <memory>

namespace locc {
class LineCounter : public klib::task::Task {
  public:
	explicit LineCounter(klib::task::Queue& queue, Query query);

	[[nodiscard]] auto get_progress() const -> Progress;
	[[nodiscard]] auto to_rows() const -> std::vector<Row>;

  private:
	void execute() final;

	struct Impl;
	struct Deleter {
		void operator()(Impl* ptr) const noexcept;
	};
	std::unique_ptr<Impl, Deleter> m_impl{};
};
} // namespace locc
