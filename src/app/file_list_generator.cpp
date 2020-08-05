#include <algorithm>
#include <app/config.hpp>
#include <kt/args_parser/args_parser.hpp>
#include <ui/ui.hpp>
#include "file_list_generator.hpp"

namespace
{
bool skip_file(stdfs::path const& path)
{
	if (!stdfs::is_regular_file(path))
	{
		return true;
	}
	auto const ext = path.extension();
	if (ext.empty())
	{
		return true;
	}
	if (cfg::g_mode == cfg::mode::implt)
	{
		auto const ext_str = ext.generic_string();
		if (std::none_of(cfg::g_ext_passlist.begin(), cfg::g_ext_passlist.end(), [ext_str](auto skip) -> bool { return skip == ext_str; }))
		{
			return true;
		}
	}
	auto p = path;
	while (!p.empty() && p.has_parent_path())
	{
		auto const name = p.filename().generic_string();
		auto const path = p.generic_string();
		if (name.size() > 1 && name.at(0) == '.' && name.at(1) != '.')
		{
			return true;
		}
		if (std::any_of(cfg::g_skip_substrs.begin(), cfg::g_skip_substrs.end(), [path](auto skip) -> bool { return path.find(skip) != locc::null_index; }))
		{
			return true;
		}
		p = p.parent_path();
	}
	if (path.generic_string().find(".git") != std::string::npos)
	{
		return true;
	}
	return false;
}
} // namespace

std::deque<stdfs::path> locc::file_list(parser::type_t const& entries)
{
	std::deque<stdfs::path> ret;
	cfg::g_mode = cfg::mode::implt;
	for (auto& [key, value] : entries)
	{
		if (cfg::g_mode == cfg::mode::implt)
		{
			auto k = key;
			if (key == "." || key == "..")
			{
				k += "/";
			}
			if (stdfs::is_directory(stdfs::absolute(k)))
			{
				auto recurse = stdfs::recursive_directory_iterator(k, stdfs::directory_options::skip_permission_denied);
				for (auto iter = stdfs::begin(recurse); iter != stdfs::end(recurse); ++iter)
				{
					try
					{
						if (!iter->is_symlink())
						{
							auto path = iter->path();
							if (!skip_file(path))
							{
								locc::log(cfg::test(cfg::flag::debug) && cfg::test(cfg::flag::verbose), "  -- tracking ", path.generic_string(), "\n");
								ret.push_back(std::move(path));
							}
						}
					}
					catch (std::exception const& e)
					{
						locc::err("Exception caught: ", e.what(), "\n");
						continue;
					}
				}
				return ret;
			}
			cfg::g_mode = cfg::mode::explt;
		}
		if (cfg::g_mode == cfg::mode::explt)
		{
			if (stdfs::is_regular_file(stdfs::absolute(key)))
			{
				ret.push_back(std::move(key));
			}
		}
	}
	return ret;
}
