#pragma once
#include <locc/line_counter.hpp>

#include <memory>

namespace locc::gui {
class Counter {
  public:
	static constexpr auto sort_by_file_type_v = LineCount::Metric{100};

	Counter() = default;

	void start(std::string path, std::unique_ptr<LineCounter> counter);

	[[nodiscard]] auto get_path() const -> char const* { return m_path.c_str(); }

	void sort_rows();

	void update();

	explicit operator bool() const { return !!m_counter; }

	bool show_progress{true};
	bool show_totals{true};
	LineCount::Metric sort_by{LineCount::Lines};
	bool sort_ascend{false};

  private:
	struct DisplayRow {
		std::string_view name{};
		std::array<std::string, 6> metrics{};
	};

	void update_progress();
	void update_rows();

	void set_rows(std::vector<Row> rows);
	void set_display_rows();

	std::string m_path{"Drag a file / directory to start counting"};
	std::unique_ptr<LineCounter> m_counter{};

	bool m_got_rows{};
	std::vector<Row> m_rows{};
	std::vector<DisplayRow> m_display_rows{};
	DisplayRow m_totals{.name = "Total"};

	std::string m_progress_overlay{};
};
} // namespace locc::gui
