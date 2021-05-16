#include <fstream>
#include <app/config.hpp>
#include <ui/ui.hpp>

namespace cfg {
void settings::prep() {
	for (auto const& [_, exts] : ext_groups) {
		for (auto const& ext : exts) { ext_passlist.insert(ext); }
	}
	for (auto const& [ext, _] : comment_infos) {
		if (ext.at(0) == '.') { ext_passlist.insert(ext); }
	}
	for (auto const& [id, exts] : id_groups) {
		for (auto const& ext : exts) { ext_to_id[ext] = id; }
	}
	for (auto const& [ext, _] : ext_to_id) { ext_passlist.insert(ext); }
	for (auto const& [group, exts] : ext_groups) {
		if (auto const search = comment_infos.find(group); search != comment_infos.end()) {
			for (auto const& ext : exts) { comment_infos[ext] = search->second; }
		}
	}
}

std::optional<locc::file_t> settings::include_file(stdfs::path const& path) const {
	if (!stdfs::is_regular_file(stdfs::absolute(path))) { return std::nullopt; }
	if (path.generic_string().find(".git/") != std::string::npos) { return std::nullopt; }
	auto const& skip = skip_substrs;
	auto const path_str = path.generic_string();
	if (std::any_of(skip.begin(), skip.end(), [&path_str](auto const& skip) { return path_str.find(skip) < path_str.size(); })) { return std::nullopt; }
	auto const filename = path.filename().generic_string();
	if (filenames_as_ext.find(filename) != filenames_as_ext.end()) { return locc::file_t{.path = path, .ext = filename, .id = get_id(filename), .lines = {}}; }
	auto const ext = path.extension().generic_string();
	if (ext.empty()) { return std::nullopt; }
	if (std::none_of(ext_passlist.begin(), ext_passlist.end(), [&ext](auto skip) { return skip == ext; })) { return std::nullopt; }
	return locc::file_t{.path = path, .ext = ext, .id = get_id(ext), .lines = {}};
}

locc::comment_info const& settings::find_comment_info(locc::ext_t const& extension) const {
	if (auto iter = comment_infos.find(extension); iter != comment_infos.end()) { return iter->second; }
	static locc::comment_info const default_ret;
	return default_ret;
}

locc::id_t settings::get_id(std::string const& query) const {
	if (!query.empty()) {
		if (auto search = ext_to_id.find(query); search != ext_to_id.end()) { return search->second; }
		if (query.at(0) == '.' && filenames_as_ext.find(query) == filenames_as_ext.end()) { return "*" + query; }
	}
	return query;
}
} // namespace cfg
