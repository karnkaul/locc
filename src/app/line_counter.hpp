#pragma once
#include <deque>
#include <app/common.hpp>

namespace locc {
struct result;

result process(std::deque<locc::file> files);
} // namespace locc
