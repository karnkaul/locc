#include <fstream>
#include <dumb_json/dumb_json.hpp>
#include <app/config.hpp>
#include <ui/ui.hpp>

namespace cfg
{
locc::comment_info const& settings::find_comment_info(locc::ext const& extension) const
{
	if (auto iter = comment_infos.find(extension); iter != comment_infos.end())
	{
		return iter->second;
	}
	static locc::comment_info const default_ret;
	return default_ret;
}

locc::id settings::get_id(std::string const& query) const
{
	if (!query.empty())
	{
		if (auto search = ext_to_id.find(query); search != ext_to_id.end())
		{
			return search->second;
		}
		if (query.at(0) == '.' && filenames_as_ext.find(query) == filenames_as_ext.end())
		{
			return "*" + query;
		}
	}
	return query;
}

bool settings::import()
{
	auto const json_full = stdfs::absolute(json);
	if (std::ifstream file(json_full); file.good())
	{
		std::stringstream str;
		str << file.rdbuf();
		dj::object root;
		if (root.read(str.str()))
		{
			locc::log(test(flag::debug), "\n  -- reading settings from [", json.generic_string(), "]");
			if (root.value<dj::boolean>("no_defaults"))
			{
				locc::log(test(flag::debug), "\n  -- purging default settings");
				skip_substrs.clear();
				filenames_as_ext.clear();
				ext_groups.clear();
				id_groups.clear();
				comment_infos.clear();
			}
			if (auto p_skip_substrs = root.find<dj::array>("skip_substrs"))
			{
				locc::log(test(flag::debug), "\n   - skip-substrs:");
				auto arr = p_skip_substrs->cast_values<dj::string>();
				for (auto const& skip_substr : arr)
				{
					locc::log(test(flag::debug), "  ", *skip_substr);
					skip_substrs.insert(*skip_substr);
				}
			}
			if (auto p_filenames_as_ext = root.find<dj::array>("filenames_as_ext"))
			{
				locc::log(test(flag::debug), "\n   - filenames-as-ext:");
				auto arr = p_filenames_as_ext->cast_values<dj::string>();
				for (auto const& filename : arr)
				{
					locc::log(test(flag::debug), "  ", *filename);
					filenames_as_ext.insert(*filename);
				}
			}
			if (auto p_ext_groups = root.find<dj::object>("ext_groups"))
			{
				locc::log(test(flag::debug), "\n   - ext-groups:");
				for (auto const& [group, p_base] : p_ext_groups->entries)
				{
					if (auto p_arr = p_base->cast<dj::array>())
					{
						locc::log(test(flag::debug), "\n    - ", group, ":");
						auto arr = p_arr->cast_values<dj::string>();
						for (auto p_ext : arr)
						{
							locc::log(test(flag::debug), "  ", *p_ext);
							ext_groups[group].push_back(*p_ext);
						}
					}
				}
			}
			if (auto p_id_groups = root.find<dj::object>("id_groups"))
			{
				locc::log(test(flag::debug), "\n   - id-groups:");
				for (auto const& [group, p_base] : p_id_groups->entries)
				{
					if (auto p_arr = p_base->cast<dj::array>())
					{
						locc::log(test(flag::debug), "\n    - ", group, ":");
						auto arr = p_arr->cast_values<dj::string>();
						for (auto p_ext : arr)
						{
							locc::log(test(flag::debug), "  ", *p_ext);
							id_groups[group].push_back(*p_ext);
						}
					}
				}
			}
			if (auto p_comment_infos = root.find<dj::object>("comment_infos"))
			{
				locc::log(test(flag::debug), "\n   - comment-infos:");
				for (auto const& [group, p_base] : p_comment_infos->entries)
				{
					if (auto p_obj = p_base->cast<dj::object>())
					{
						locc::log(test(flag::debug), "\n    - ", group, ":");
						if (auto p_lines = p_obj->find<dj::array>("lines"))
						{
							auto lines = p_lines->cast_values<dj::string>();
							for (auto p_line : lines)
							{
								locc::log(test(flag::debug), "  ", *p_line);
								comment_infos[group].comment_lines.push_back(*p_line);
							}
						}
						if (auto p_blocks = p_obj->find<dj::array>("blocks"))
						{
							auto blocks = p_blocks->cast<dj::object>();
							for (auto p_block : blocks)
							{
								auto const& open = p_block->value<dj::string>("open");
								auto const& close = p_block->value<dj::string>("close");
								if (!open.empty() && !close.empty())
								{
									locc::log(test(flag::debug), "  [", open, ", ", close, "]");
									comment_infos[group].comment_blocks.push_back({open, close});
								}
							}
						}
					}
				}
			}
			locc::log(test(flag::debug), "\n");
			return true;
		}
	}
	return false;
}
} // namespace cfg
