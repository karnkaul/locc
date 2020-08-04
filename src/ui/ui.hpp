#pragma once
#include <app/file_list_generator.hpp>

namespace locc
{
struct result;

bool parse_options(parser::key const& key, parser::value value);
void print_debug_prologue();
void print(result const& result);
void print_help();
} // namespace locc
