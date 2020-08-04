#pragma once
#include <deque>
#include <filesystem>

namespace stdfs = std::filesystem;

namespace locc
{
struct result;

result process(std::deque<stdfs::path> file_paths);
} // namespace locc
