#pragma once
#include <locc/file_filter.hpp>
#include <locc/grammar.hpp>
#include <span>

namespace locc {
struct Query {
	std::span<Grammar const> grammars{};
	std::string_view path{};
	IFileFilter const* filter{};
};
} // namespace locc
