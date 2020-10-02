#include "file_list_generator.hpp"
#include <algorithm>
#include <optional>
#include <app/config.hpp>
#include <kt/args_parser/args_parser.hpp>
#include <ui/ui.hpp>

namespace {
std::optional<locc::file> include_file(stdfs::path const& path) {
	if (!stdfs::is_regular_file(stdfs::absolute(path))) {
		return {};
	}
	if (path.generic_string().find(".git/") != std::string::npos) {
		return {};
	}
	auto const filename = path.filename().generic_string();
	if (cfg::g_settings.filenames_as_ext.find(filename) != cfg::g_settings.filenames_as_ext.end()) {
		return locc::file{path, filename, cfg::g_settings.get_id(filename), {}};
	}
	auto const ext = path.extension().generic_string();
	if (ext.empty()) {
		return {};
	}
	if (cfg::g_mode == cfg::mode::implt) {
		if (std::none_of(cfg::g_settings.ext_passlist.begin(), cfg::g_settings.ext_passlist.end(), [&ext](auto skip) -> bool { return skip == ext; })) {
			return {};
		}
	}
	auto p = path;
	while (!p.empty() && p.has_parent_path()) {
		auto const name = p.filename().generic_string();
		auto const path = p.generic_string();
		if (std::any_of(cfg::g_settings.skip_substrs.begin(), cfg::g_settings.skip_substrs.end(),
						[path](auto skip) -> bool { return path.find(skip) != locc::null_index; })) {
			return {};
		}
		p = p.parent_path();
	}
	return locc::file{path, ext, cfg::g_settings.get_id(ext), {}};
}
} // namespace

std::deque<locc::file> locc::file_list(parser::type_t const& entries) {
	std::deque<file> ret;
	cfg::g_mode = cfg::mode::implt;
	for (auto& [key, value] : entries) {
		if (cfg::g_mode == cfg::mode::implt) {
			auto k = key;
			if (key == "." || key == "..") {
				k += "/";
			}
			if (stdfs::is_directory(stdfs::absolute(k))) {
				auto recurse = stdfs::recursive_directory_iterator(k, stdfs::directory_options::skip_permission_denied);
				for (auto iter = stdfs::begin(recurse); iter != stdfs::end(recurse); ++iter) {
					try {
						if (cfg::test(cfg::flag::follow_symlinks) || !iter->is_symlink()) {
							auto path = iter->path();
							auto file = include_file(path);
							if (file) {
								locc::log_if(cfg::test(cfg::flag::debug) && cfg::test(cfg::flag::verbose), "  -- tracking {}\n ", path.generic_string());
								ret.push_back(std::move(*file));
							}
						}
					} catch (std::exception const& e) {
						locc::err_if(cfg::test(cfg::flag::debug), e.what(), "\n");
					}
				}
				return ret;
			}
			cfg::g_mode = cfg::mode::explt;
		}
		if (cfg::g_mode == cfg::mode::explt) {
			if (stdfs::is_regular_file(stdfs::absolute(key))) {
				auto path = stdfs::path(key);
				auto const filename = path.filename().generic_string();
				ret.push_back(file{std::move(path), filename, cfg::g_settings.get_id(filename), {}});
			}
		}
	}
	return ret;
}
