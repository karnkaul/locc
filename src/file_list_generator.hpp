#pragma once
#include <common.hpp>

namespace loc
{
std::deque<stdfs::path> file_list(std::deque<loc::entry> const& entries);
} // namespace loc
