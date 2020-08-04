#include <cctype>
#include <fstream>
#include <functional>
#include <mutex>
#include <string>
#include <stdexcept>
#include <thread>
#include <async_queue/async_queue.hpp>
#include <app/config.hpp>
#include <ui/ui.hpp>
#include "line_counter.hpp"

using async_queue = kt::async_queue<std::function<void()>>;

namespace
{
struct worker final
{
	std::thread thread;

	worker() = default;
	worker(async_queue& out_queue)
	{
		thread = std::thread([this, &out_queue]() { work(out_queue); });
	}
	worker(worker&&) = default;
	worker& operator=(worker&&) = default;
	~worker()
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}

	void work(async_queue& out_queue)
	{
		while (auto task = out_queue.pop())
		{
			if (!task)
			{
				break;
			}
			task();
		}
	}
};

async_queue g_queue;
std::deque<worker> g_workers;
} // namespace

namespace
{
constexpr std::size_t g_DEBUG_skip_to_line = 124;

void trim_leading_spaces(std::string_view& out_line)
{
	while (!out_line.empty() && std::isspace(out_line.at(0)))
	{
		out_line = out_line.substr(1);
	}
}

bool line_comment_at_start(locc::config const& config, std::string_view sub_line)
{
	for (auto& str : config.comment_lines)
	{
		if (auto const search = sub_line.find(str); search == 0)
		{
			return true;
		}
	}
	return false;
}

void trim_leading_comment_blocks(locc::config const& config, locc::comment_block const* open_block, std::string_view& out_line)
{
	if (!out_line.empty())
	{
		if (!open_block)
		{
			for (auto const& block : config.comment_blocks)
			{
				if (auto begin = out_line.find(block.first); begin != locc::null_index)
				{
					open_block = &block;
					out_line = out_line.substr(begin + block.first.size());
					trim_leading_comment_blocks(config, open_block, out_line);
				}
			}
		}
		else
		{
			if (auto end = out_line.find(open_block->second); end != locc::null_index)
			{
				out_line = out_line.substr(end + open_block->second.size());
				open_block = nullptr;
				trim_leading_comment_blocks(config, open_block, out_line);
			}
		}
	}
	return;
}

void count_line(locc::config const& config, locc::file& out_file, locc::comment_block const* open_block, std::string_view line)
{
	++out_file.lines.total;
	if (cfg::test(cfg::flag::debug) && g_DEBUG_skip_to_line == out_file.lines.total)
	{
		;
	}
	trim_leading_spaces(line);
	if (!open_block)
	{
		if (line_comment_at_start(config, line))
		{
			++out_file.lines.comments;
		}
		else
		{
			trim_leading_comment_blocks(config, open_block, line);
			if (!line.empty())
			{
				++out_file.lines.code;
			}
			else
			{
				open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
			}
		}
	}
	else
	{
		trim_leading_comment_blocks(config, open_block, line);
		if (!line.empty())
		{
			++out_file.lines.comments;
		}
		else
		{
			open_block ? ++out_file.lines.comments : ++out_file.lines.blank;
		}
	}
}

locc::file count_lines(stdfs::path file_path)
{
	locc::file ret;
	std::ifstream f(stdfs::absolute(file_path), std::ios::in);
	if (f.good())
	{
		ret.lines.code = ret.lines.total = 1;
		std::string line;
		locc::comment_block const* open_block = nullptr;
		auto const& config = cfg::find_config(file_path.extension().generic_string());
		while (std::getline(f, line))
		{
			count_line(config, ret, open_block, line);
		}
	}
	ret.path = std::move(file_path);
	return ret;
}
} // namespace

locc::result locc::process(std::deque<stdfs::path> file_paths)
{
	result ret;
	if (file_paths.empty())
	{
		return ret;
	}
	kt::lockable mutex;
	bool const blanks = cfg::test(cfg::flag::blanks);
	auto process_file = [&ret, &mutex, blanks](auto iter) {
		auto& file_path = *iter;
		auto ext = file_path.extension();
		auto file = count_lines(std::move(file_path));
		auto lock = mutex.lock();
		ret.totals.total += file.lines.total;
		ret.totals.code += file.lines.code;
		auto& data = ret.dist[ext.empty() ? "[none]" : ext.generic_string()];
		++data.counts.files;
		if (blanks)
		{
			ret.totals.code += file.lines.blank;
			data.counts.lines.code += file.lines.blank;
		}
		data.counts.lines.code += file.lines.code;
		data.counts.lines.comments += file.lines.comments;
		data.counts.lines.total += file.lines.total;
		ret.files.push_back(std::move(file));
	};
	int thread_count = cfg::test(cfg::flag::one_thread) ? 0 : (int)std::min((std::size_t)std::thread::hardware_concurrency() - 1, file_paths.size() - 1);
	locc::log(cfg::test(cfg::flag::debug), "  -- parsing ", file_paths.size(), " files\n");
	if (thread_count > 0)
	{
		g_workers.clear();
		locc::log(cfg::test(cfg::flag::debug), "  -- launching ", thread_count, " worker threads\n");
		for (int count = thread_count; count > 0; --count)
		{
			g_workers.push_back(worker(g_queue));
		}
	}
	for (auto iter = file_paths.begin(); iter != file_paths.end(); ++iter)
	{
		if (thread_count > 0)
		{
			g_queue.push([iter, process_file]() { process_file(iter); });
		}
		else
		{
			process_file(iter);
		}
	}
	if (thread_count > 0)
	{
		while (!g_queue.empty())
		{
			auto task = g_queue.pop();
			task();
		}
		auto residue = g_queue.clear();
		g_queue.active(false);
		g_workers.clear();
		locc::log(cfg::test(cfg::flag::debug), "  -- worker threads completed\n");
		for (auto& task : residue)
		{
			task();
		}
	}
	locc::log(cfg::test(cfg::flag::debug), "\n");
	for (auto& [_, data] : ret.dist)
	{
		data.ratio.divide(data.counts.lines, ret.totals);
	}
	return ret;
}
