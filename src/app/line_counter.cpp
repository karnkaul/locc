#include "line_counter.hpp"
#include <cctype>
#include <fstream>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <app/config.hpp>
#include <kt/async_queue/async_queue.hpp>
#include <ui/ui.hpp>

using async_queue = kt::async_queue<std::function<void()>>;

namespace {
struct worker final {
	std::thread thread;

	worker() = default;
	worker(async_queue& out_queue) {
		thread = std::thread([this, &out_queue]() { work(out_queue); });
	}
	worker(worker&&) = default;
	worker& operator=(worker&&) = default;
	~worker() {
		if (thread.joinable()) {
			thread.join();
		}
	}

	void work(async_queue& out_queue) {
		while (auto task = out_queue.pop()) {
			if (!task) {
				break;
			}
			(*task)();
		}
	}
};

async_queue g_queue;
std::deque<worker> g_workers;
} // namespace

namespace {
constexpr std::size_t g_DEBUG_skip_to_line = 124;

void trim_leading_spaces(std::string_view& out_line) {
	while (!out_line.empty() && std::isspace(out_line.at(0))) {
		out_line = out_line.substr(1);
	}
}

bool line_comment_at_start(locc::comment_info const& info, std::string_view sub_line) {
	for (auto& str : info.comment_lines) {
		if (auto const search = sub_line.find(str); search == 0) {
			return true;
		}
	}
	return false;
}

void trim_leading_comment_blocks(locc::comment_info const& info, locc::comment_block const* open_block, std::string_view& out_line) {
	if (!out_line.empty()) {
		if (!open_block) {
			for (auto const& block : info.comment_blocks) {
				if (auto begin = out_line.find(block.first); begin != locc::null_index) {
					open_block = &block;
					out_line = out_line.substr(begin + block.first.size());
					trim_leading_comment_blocks(info, open_block, out_line);
				}
			}
		} else {
			if (auto end = out_line.find(open_block->second); end != locc::null_index) {
				out_line = out_line.substr(end + open_block->second.size());
				open_block = nullptr;
				trim_leading_comment_blocks(info, open_block, out_line);
			}
		}
	}
	return;
}

void count_line(locc::comment_info const& info, locc::file& out_file, locc::comment_block const* open_block, std::string_view line) {
	++out_file.lines.total;
	if (cfg::test(cfg::flag::debug) && g_DEBUG_skip_to_line == out_file.lines.total) {
		;
	}
	trim_leading_spaces(line);
	if (!open_block) {
		if (line_comment_at_start(info, line)) {
			++out_file.lines.comments;
		} else {
			trim_leading_comment_blocks(info, open_block, line);
			if (!line.empty()) {
				++out_file.lines.code;
			} else {
				open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
			}
		}
	} else {
		trim_leading_comment_blocks(info, open_block, line);
		if (!line.empty()) {
			++out_file.lines.comments;
		} else {
			open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
		}
	}
}

void count_lines(locc::file& out_file) {
	std::ifstream f(stdfs::absolute(out_file.path), std::ios::in);
	if (f.good()) {
		out_file.lines.code = out_file.lines.total = 1;
		std::string line;
		locc::comment_block const* open_block = nullptr;
		auto const& info = cfg::g_settings.find_comment_info(out_file.ext);
		while (std::getline(f, line)) {
			count_line(info, out_file, open_block, line);
		}
	}
}
} // namespace

locc::result locc::process(std::deque<file> files) {
	result ret;
	if (files.empty()) {
		return ret;
	}
	kt::lockable mutex;
	bool const blanks = cfg::test(cfg::flag::blanks);
	auto process_file = [&ret, &mutex, blanks](auto iter) {
		auto& file = *iter;
		count_lines(file);
		auto lock = mutex.lock();
		ret.totals.total += file.lines.total;
		ret.totals.code += file.lines.code;
		auto& data = ret.dist[file.id];
		++data.counts.files;
		if (blanks) {
			ret.totals.code += file.lines.blank;
			data.counts.lines.code += file.lines.blank;
		}
		data.counts.lines.code += file.lines.code;
		data.counts.lines.comments += file.lines.comments;
		data.counts.lines.total += file.lines.total;
		ret.files.push_back(std::move(file));
	};
	locc::log_if(cfg::test(cfg::flag::debug), "  -- parsing {} files\n", files.size());
	int thread_count = cfg::test(cfg::flag::one_thread) ? 0 : (int)std::min((std::size_t)std::thread::hardware_concurrency() - 1, files.size() - 1);
	g_workers.clear();
	if (thread_count > 0) {
		locc::log_if(cfg::test(cfg::flag::debug), "  -- launching {} worker threads\n", thread_count);
		for (int count = thread_count; count > 0; --count) {
			g_workers.push_back(worker(g_queue));
		}
	}
	for (auto iter = files.begin(); iter != files.end(); ++iter) {
		g_queue.push([iter, process_file]() { process_file(iter); });
	}
	while (!g_queue.empty()) {
		auto task = g_queue.pop();
		(*task)();
	}
	auto residue = g_queue.clear();
	g_queue.active(false);
	g_workers.clear();
	locc::log_if(cfg::test(cfg::flag::debug) && thread_count > 0, "  -- worker threads completed\n");
	for (auto& task : residue) {
		task();
	}
	locc::log_if(cfg::test(cfg::flag::debug), "\n");
	for (auto& [_, data] : ret.dist) {
		data.ratio.divide(data.counts.lines, ret.totals);
	}
	return ret;
}
