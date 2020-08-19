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
	auto const json_full = stdfs::absolute(json_path);
	if (std::ifstream file(json_full); file.good())
	{
		std::stringstream str;
		str << file.rdbuf();
		auto json = dj::deserialise(str.str());
		if (auto root = dj::cast<dj::object>(json))
		{
			locc::log_if(test(flag::debug), "\n  -- reading settings from [{}]", json_path.generic_string());
			if (root->value<dj::boolean>("no_defaults"))
			{
				locc::log_if(test(flag::debug), "\n  -- purging default settings");
				skip_substrs.clear();
				filenames_as_ext.clear();
				ext_groups.clear();
				id_groups.clear();
				comment_infos.clear();
			}
			if (auto p_skip_substrs = root->find<dj::array>("skip_substrs"))
			{
				locc::log_if(test(flag::debug), "\n   - skip-substrs:");
				p_skip_substrs->for_each_value<dj::string>([this](auto const& skip_substr) {
					locc::log_if(test(flag::debug), "  {}", skip_substr);
					skip_substrs.insert(skip_substr);
				});
			}
			if (auto p_filenames_as_ext = root->find<dj::array>("filenames_as_ext"))
			{
				locc::log_if(test(flag::debug), "\n   - filenames-as-ext:");
				p_filenames_as_ext->for_each_value<dj::string>([this](auto const& filename) {
					locc::log_if(test(flag::debug), "  {}", filename);
					filenames_as_ext.insert(filename);
				});
			}
			if (auto p_ext_groups = root->find<dj::object>("ext_groups"))
			{
				locc::log_if(test(flag::debug), "\n   - ext-groups:");
				for (auto const& kvp : p_ext_groups->fields)
				{
					auto const& [group, p_base] = kvp;
					if (auto p_arr = p_base->cast<dj::array>())
					{
						locc::log_if(test(flag::debug), "\n    - {}:", group);
						p_arr->for_each_value<dj::string>([this, &kvp](auto const& ext) {
							locc::log_if(test(flag::debug), "  {}", ext);
							ext_groups[kvp.first].push_back(ext);
						});
					}
				}
			}
			if (auto p_id_groups = root->find<dj::object>("id_groups"))
			{
				locc::log_if(test(flag::debug), "\n   - id-groups:");
				for (auto const& kvp : p_id_groups->fields)
				{
					auto const& [group, p_base] = kvp;
					if (auto p_arr = p_base->cast<dj::array>())
					{
						locc::log_if(test(flag::debug), "\n    - {}:", group);
						p_arr->for_each_value<dj::string>([this, &kvp](auto const& ext) {
							locc::log_if(test(flag::debug), "  {}", ext);
							id_groups[kvp.first].push_back(ext);
						});
					}
				}
			}
			if (auto p_comment_infos = root->find<dj::object>("comment_infos"))
			{
				locc::log_if(test(flag::debug), "\n   - comment-infos:");
				for (auto const& kvp : p_comment_infos->fields)
				{
					auto const& [group, p_base] = kvp;
					if (auto p_obj = p_base->cast<dj::object>())
					{
						locc::log_if(test(flag::debug), "\n    - {}:", group);
						if (auto p_lines = p_obj->find<dj::array>("lines"))
						{
							p_lines->for_each_value<dj::string>([this, &kvp](auto const& line) {
								locc::log_if(test(flag::debug), "  {}", line);
								comment_infos[kvp.first].comment_lines.push_back(line);
							});
						}
						if (auto p_blocks = p_obj->find<dj::array>("blocks"))
						{
							p_blocks->for_each<dj::object>([this, &kvp](auto const& block) {
								auto const& open = block.template value<dj::string>("open");
								auto const& close = block.template value<dj::string>("close");
								if (!open.empty() && !close.empty())
								{
									locc::log_if(test(flag::debug), "  [{}, {}]", open, close);
									comment_infos[kvp.first].comment_blocks.push_back({open, close});
								}
							});
						}
					}
				}
			}
			locc::log_if(test(flag::debug), "\n");
			return true;
		}
	}
	return false;
}
} // namespace cfg
