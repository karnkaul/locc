#pragma once
#include <filesystem>
#include <vector>
#include <app/common.hpp>

namespace locc {
namespace stdfs = std::filesystem;

std::vector<file> file_list(stdfs::path const& root);
} // namespace locc
