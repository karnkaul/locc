#pragma once
#include <filesystem>
#include <kt/args_parser/args_parser.hpp>

namespace stdfs = std::filesystem;

namespace locc
{
using parser = kt::args_parser<>;

std::deque<stdfs::path> file_list(parser::type_t const& entries);
} // namespace locc
