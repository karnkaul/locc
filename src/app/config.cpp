#include <fstream>
#include <app/config.hpp>
#include <ui/ui.hpp>

namespace cfg {
locc::comment_info const& settings::find_comment_info(locc::ext_t const& extension) const {
	if (auto iter = comment_infos.find(extension); iter != comment_infos.end()) {
		return iter->second;
	}
	static locc::comment_info const default_ret;
	return default_ret;
}

locc::id_t settings::get_id(std::string const& query) const {
	if (!query.empty()) {
		if (auto search = ext_to_id.find(query); search != ext_to_id.end()) {
			return search->second;
		}
		if (query.at(0) == '.' && filenames_as_ext.find(query) == filenames_as_ext.end()) {
			return "*" + query;
		}
	}
	return query;
}
} // namespace cfg
