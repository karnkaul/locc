#include <counter.hpp>
#include <im_text.hpp>

#include <imgui.h>

#include <ranges>

namespace locc::gui {
void Counter::start(std::string path, std::unique_ptr<LineCounter> counter) {
	m_path = std::move(path);
	m_counter = std::move(counter);
	m_display_rows.clear();
	m_got_rows = false;
	for (auto& metric : m_totals.metrics) { metric.clear(); }
}

void Counter::sort_rows() {
	auto const sort_dir = sort_ascend ? SortDir::Ascending : SortDir::Descending;
	if (sort_by == sort_by_file_type_v) {
		sort_by_file_type(m_rows, sort_dir);
	} else {
		if (sort_by >= LineCount::COUNT_) { sort_by = LineCount::Lines; }
		sort_by_metric(m_rows, sort_by, sort_dir);
	}

	set_display_rows();
}

void Counter::update() {
	if (!m_counter) { return; }

	update_progress();
	update_rows();
}

void Counter::update_progress() {
	if (!show_progress) { return; }
	auto const progress = m_counter->get_progress();
	im_text("Status: {}", to_str(progress.state));
	m_progress_overlay.clear();
	std::format_to(std::back_inserter(m_progress_overlay), "[{}/{}]", progress.counted, progress.total);
	ImGui::SetNextItemWidth(-1.0f);
	ImGui::ProgressBar(progress.as_float(), {-FLT_MIN, 0.0f}, m_progress_overlay.c_str());
}

void Counter::update_rows() {
	if (!m_got_rows && m_counter->get_status() == LineCounter::Status::Completed) {
		set_rows(m_counter->to_rows());
		m_got_rows = true;
	}

	auto const update_row = [](DisplayRow const& row) {
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		im_text("{}", row.name);
		for (std::size_t i = 0; i < LineCount::COUNT_; ++i) {
			auto const& metric = row.metrics[i]; // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
			ImGui::TableSetColumnIndex(int(i) + 1);
			im_text("{}", metric);
		}
	};

	if (ImGui::BeginTable("Counts", 6, ImGuiTableFlags_Borders)) {
		ImGui::TableSetupColumn("File Type");
		ImGui::TableSetupColumn("Files");
		ImGui::TableSetupColumn("Lines");
		ImGui::TableSetupColumn("Code");
		ImGui::TableSetupColumn("Comments");
		ImGui::TableSetupColumn("Empty");
		ImGui::TableHeadersRow();

		for (auto const& row : m_display_rows) { update_row(row); }
		if (show_totals) { update_row(m_totals); }

		ImGui::EndTable();
	}
}

void Counter::set_rows(std::vector<Row> rows) {
	m_rows = std::move(rows);
	sort_rows();
	set_display_rows();

	auto totals = Row{};
	totals.aggregate(m_rows);
	for (auto [in_metric, out_metric] : std::ranges::zip_view(totals.line_count.metrics, m_totals.metrics)) { out_metric = beautify(in_metric); }
}

void Counter::set_display_rows() {
	m_display_rows.clear();
	for (auto& row : m_rows) {
		auto& out_row = m_display_rows.emplace_back(DisplayRow{.name = row.file_type});
		for (auto [in_metric, out_metric] : std::ranges::zip_view(row.line_count.metrics, out_row.metrics)) { out_metric = beautify(in_metric); }
	}
}
} // namespace locc::gui
