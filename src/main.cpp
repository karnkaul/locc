#include <algorithm>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <kt/args_parser/args_parser.hpp>
#include <app/config.hpp>
#include <app/file_list_generator.hpp>
#include <app/line_counter.hpp>
#include <ui/ui.hpp>

namespace
{
bool parse_options(std::deque<locc::parser::entry>& out_entries)
{
	for (auto iter = out_entries.begin(); iter != out_entries.end();)
	{
		auto& [key, value] = *iter;
		if (locc::match_any(key, "h", "help"))
		{
			locc::print_help();
			return false;
		}
		else if (locc::match_any(key, "version"))
		{
			locc::print_version();
			return false;
		}
		if (locc::parse_options(key, value))
		{
			iter = out_entries.erase(iter);
		}
		else
		{
			++iter;
		}
	}
	if (out_entries.empty())
	{
		out_entries.push_back({".", {}});
	}
	return true;
}

std::deque<locc::file> list_files(std::deque<locc::parser::entry> entries)
{
	for (auto& [ext, _] : cfg::g_ext_groups)
	{
		cfg::g_ext_passlist.insert(ext);
	}
	for (auto& [ext, _] : cfg::g_ext_config)
	{
		if (ext.at(0) == '.')
		{
			cfg::g_ext_passlist.insert(ext);
		}
	}
	for (auto iter = entries.begin(); iter != entries.end();)
	{
		auto& [key, value] = *iter;
		if (locc::parse_options(key, value))
		{
			iter = entries.erase(iter);
		}
		else
		{
			return locc::file_list(std::move(entries));
		}
	}
	return {};
}

void run_loc(std::deque<locc::file> files)
{
	for (auto const& [ext, group] : cfg::g_ext_groups)
	{
		if (auto search = cfg::g_ext_config.find(group); search != cfg::g_ext_config.end())
		{
			cfg::g_ext_config[ext] = search->second;
		}
	}
	auto result = locc::process(std::move(files));
	locc::print(result);
}
} // namespace

int main(int argc, char** argv)
{
	locc::parser parser;
	std::vector<std::string_view> args;
	args.reserve((std::size_t)argc);
	for (int i = 0; i < argc; ++i)
	{
		args.push_back(argv[i]);
	}
	while (args.size() < 2)
	{
		args.push_back({".", {}});
	}
	parser.parse(args);
	parser.entries.pop_front();
	if (!parse_options(parser.entries))
	{
		return 0;
	}
	auto file_paths = list_files(std::move(parser.entries));
	locc::do_if(cfg::test(cfg::flag::debug), &locc::print_debug_prologue);
	run_loc(file_paths);
}
