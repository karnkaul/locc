#pragma once
#include <span>
#include <app/common.hpp>

namespace locc {
struct result;

result process(std::span<locc::file> files);
} // namespace locc
