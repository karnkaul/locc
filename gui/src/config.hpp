#pragma once
#include <char_buf.hpp>
#include <ktask/queue_create_info.hpp>
#include <optional>
#include <string_view>
#include <vector>

namespace locc::gui {
struct Config {
	class Modal;

	std::vector<CharBuf> exclude_patterns{};
	ktask::ThreadCount thread_count{};
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

	CharBuf m_exclude_buf{};
	std::optional<std::size_t> m_selected_exclude{};

	bool m_open_popup{};
};
} // namespace locc::gui
