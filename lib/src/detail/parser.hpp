#pragma once
#include "locc/metrics.hpp"
#include <klib/base_types.hpp>
#include <string_view>

namespace locc::detail {
class TextParser : public klib::Polymorphic {
  public:
	virtual void next_line(std::string_view line);

	[[nodiscard]] auto get_metrics() const -> Metrics const& { return m_metrics; }

  protected:
	void check_empty(std::string_view line);

	Metrics m_metrics{};
};
} // namespace locc::detail
