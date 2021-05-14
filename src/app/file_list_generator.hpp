#pragma once
#include <filesystem>
#include <vector>
#include <app/common.hpp>

namespace locc {
namespace stdfs = std::filesystem;

std::vector<file_t> file_list(stdfs::path const& root);
} // namespace locc
