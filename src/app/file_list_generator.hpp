#pragma once
#include <app/common.hpp>
#include <kt/args_parser/args_parser.hpp>

namespace locc {
using parser = kt::args_parser<>;

std::deque<file> file_list(parser::type_t const& entries);
} // namespace locc
