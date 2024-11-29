#pragma once
#include <klib/fixed_string.hpp>
#include <klib/task/queue_create_info.hpp>
#include <optional>
#include <string_view>
#include <vector>

namespace locc::gui {
struct Config {
	class Modal;

	std::vector<klib::FixedString<64>> exclude_patterns{};
	klib::task::ThreadCount thread_count{};
};

class Config::Modal {
  public:
	explicit Modal();

	static constexpr std::string_view label_v = "Config";

	void open_on_next_update(Config config);

	auto update() -> bool;

	Config config{};

  private:
	void inspect();

	int m_thread_count;

	klib::StrBuf<64> m_exclude_buf{};
	std::optional<std::size_t> m_selected_exclude{};

	bool m_open_popup{};
};
} // namespace locc::gui
