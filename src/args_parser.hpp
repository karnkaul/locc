#pragma once
#include <optional>
#include <string>
#include <deque>
#include <utility>
#include <common.hpp>

namespace loc
{
std::deque<entry> parse(int argc, char const* const* argv);

value const* find(std::deque<entry> const& entries, key const& key);
} // namespace loc
