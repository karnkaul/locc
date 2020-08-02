#pragma once
#include <common.hpp>

namespace locc
{
std::deque<stdfs::path> file_list(std::deque<locc::entry> const& entries);
} // namespace locc
