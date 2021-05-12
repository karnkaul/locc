#pragma once
#include <vector>
#include <app/common.hpp>

namespace locc {
struct result;

result process(std::vector<locc::file> files);
} // namespace locc
