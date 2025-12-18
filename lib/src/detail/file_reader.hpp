#pragma once
#include <fstream>

namespace locc::detail {
class FileReader {
  public:
	explicit FileReader(std::string const& path);

	[[nodiscard]] auto get_size() const -> std::size_t { return m_size; }
	[[nodiscard]] auto get_remain() const -> std::size_t { return m_remain; }

	auto read_into(std::string& out) -> bool;

	explicit operator bool() const { return !!m_file; }

  private:
	std::ifstream m_file{};
	std::size_t m_size{};
	std::size_t m_remain{};
};
} // namespace locc::detail
