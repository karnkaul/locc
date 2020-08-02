#pragma once
#include <optional>
#include <string>
#include <deque>
#include <utility>

namespace ap
{
using key = std::string;
using value = std::string;
using entry = std::pair<key, value>;

std::deque<entry> parse(int argc, char const* const* argv);

value const* find(std::deque<entry> const& entries, key const& key);
} // namespace ap
