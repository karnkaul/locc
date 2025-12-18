#include "detail/file_reader.hpp"
#include <algorithm>
#include <cassert>

namespace locc::detail {
namespace {
constexpr std::size_t max_buffer_size_v{1024};
} // namespace

FileReader::FileReader(std::string const& path) : m_file(path, std::ios::binary | std::ios::ate), m_size(std::size_t(m_file.tellg())), m_remain(m_size) {
	m_file.seekg({}, std::ios::beg);
}

auto FileReader::read_into(std::string& out) -> bool {
	auto const read_size = std::min(m_remain, max_buffer_size_v);
	if (read_size == 0) { return false; }
	out.resize(read_size);
	m_file.read(out.data(), std::streamsize(read_size));
	assert(read_size <= m_remain);
	m_remain -= read_size;
	return true;
}
} // namespace locc::detail
