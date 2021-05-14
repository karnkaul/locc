#include <algorithm>
#include <optional>
#include <app/config.hpp>
#include <app/file_list_generator.hpp>
#include <ui/ui.hpp>

namespace {
std::optional<locc::file_t> include_file(stdfs::path const& path) {
	if (!stdfs::is_regular_file(stdfs::absolute(path))) {
		return std::nullopt;
	}
	if (path.generic_string().find(".git/") != std::string::npos) {
		return std::nullopt;
	}
	auto const filename = path.filename().generic_string();
	if (cfg::g_settings.filenames_as_ext.find(filename) != cfg::g_settings.filenames_as_ext.end()) {
		return locc::file_t{.path = path, .ext = filename, .id = cfg::g_settings.get_id(filename), .lines = {}};
	}
	auto const ext = path.extension().generic_string();
	if (ext.empty()) {
		return std::nullopt;
	}
	if (std::none_of(cfg::g_settings.ext_passlist.begin(), cfg::g_settings.ext_passlist.end(), [&ext](auto skip) { return skip == ext; })) {
		return std::nullopt;
	}
	auto const& skip = cfg::g_settings.skip_substrs;
	auto const path_str = path.generic_string();
	if (std::any_of(skip.begin(), skip.end(), [&path_str](auto const& skip) { return path_str.find(skip) < path_str.size(); })) {
		return std::nullopt;
	}
	return locc::file_t{.path = path, .ext = ext, .id = cfg::g_settings.get_id(ext), .lines = {}};
}
} // namespace

std::vector<locc::file_t> locc::file_list(stdfs::path const& root) {
	std::vector<locc::file_t> ret;
	ret.reserve(1024 * 1024);
	if (stdfs::is_directory(root)) {
		for (auto const& it : stdfs::recursive_directory_iterator(root, stdfs::directory_options::skip_permission_denied)) {
			if (!cfg::test(cfg::flag::skip_symlinks) || (!it.is_symlink() && it.exists() && it.is_regular_file() && it.file_size() > 0)) {
				stdfs::path path;
				try {
					if (auto file = include_file(it.path())) {
						locc::log_if(cfg::test(cfg::flag::debug) && cfg::test(cfg::flag::verbose), "  -- tracking {}\n ", it.path().generic_string());
						ret.push_back(std::move(*file));
					}
				} catch (std::exception const& e) {
					locc::err_if(cfg::test(cfg::flag::debug), e.what(), "\n");
				}
			}
		}
	}
	return ret;
}
