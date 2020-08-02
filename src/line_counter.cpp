#include <cctype>
#include <fstream>
#include <mutex>
#include <string>
#include <stdexcept>
#include <thread>
#include <async_queue.hpp>
#include <line_counter.hpp>

namespace
{
constexpr std::size_t g_DEBUG_skip_to_line = 124;

bool ignore_line(std::string_view line)
{
	for (auto& str : cfg::g_ignore_lines)
	{
		if (auto const search = line.find(str); search == 0)
		{
			return true;
		}
	}
	return false;
}

std::string_view trim(std::string_view sub_line)
{
	while (!sub_line.empty() && std::isspace(sub_line.at(0)))
	{
		sub_line = sub_line.substr(1);
	}
	if (!sub_line.empty())
	{
		if (ignore_line(sub_line))
		{
			return {};
		}
		for (auto& pair : cfg::g_ignore_blocks)
		{
			if (auto const start = sub_line.find(pair.first); start != loc::null_index)
			{
				auto const end = sub_line.find(pair.second);
				if (end != loc::null_index && end + pair.second.size() < sub_line.size())
				{
					return trim(sub_line.substr(end + pair.second.size()));
				}
				return sub_line;
			}
		}
	}
	return sub_line;
}

loc::ignore_block const* start_ignoring(std::string_view sub_line)
{
	for (auto& pair : cfg::g_ignore_blocks)
	{
		if (auto const start = sub_line.find(pair.first); start != loc::null_index)
		{
			auto const end = sub_line.find(pair.second);
			if (end == loc::null_index || end >= sub_line.size())
			{
				return &pair;
			}
			return start_ignoring(sub_line.substr(end));
		}
	}
	return nullptr;
}

bool count_line(std::string_view line, loc::ignore_block const*& out_ignoring)
{
	line = trim(line);
	if (!line.empty())
	{
		if (out_ignoring)
		{
			if (auto const end = line.find(out_ignoring->second); end != loc::null_index)
			{
				auto const offset = out_ignoring->second.size();
				out_ignoring = nullptr;
				return count_line(line.substr(end + offset), out_ignoring);
			}
		}
		else
		{
			out_ignoring = start_ignoring(line);
			return out_ignoring == nullptr;
		}
	}
	return false;
}

loc::file count_lines(stdfs::path file_path)
{
	loc::file ret;
	std::ifstream f(stdfs::absolute(file_path), std::ios::in);
	if (f.good())
	{
		ret.lines.loc = ret.lines.total = 1;
		std::string line;
		loc::ignore_block const* ignoring = nullptr;
		while (std::getline(f, line))
		{
			++ret.lines.total;
			if (cfg::test(cfg::flag::debug) && g_DEBUG_skip_to_line == ret.lines.total)
			{
				;
			}
			while (!line.empty() && std::isspace(line.at(0)))
			{
				line = line.substr(1);
			}
			if (line.empty())
			{
				if (!ignoring)
				{
					++ret.lines.empty;
				}
			}
			else
			{
				if (count_line(line, ignoring))
				{
					++ret.lines.loc;
				}
			}
		}
	}
	ret.path = std::move(file_path);
	return ret;
}

std::size_t width(std::size_t number)
{
	std::size_t ret = 0;
	while (number > 0)
	{
		number /= 10;
		++ret;
	}
	return ret;
}

struct worker final
{
	std::thread thread;

	worker() = default;

	worker(loc::async_queue& out_queue)
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

	void work(loc::async_queue& out_queue)
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

loc::async_queue g_queue;
std::deque<worker> g_workers;
} // namespace

loc::result loc::process(std::deque<stdfs::path> file_paths)
{
	result ret;
	lockable mutex;
	auto process_file = [&ret, &mutex](auto iter) {
		auto& file_path = *iter;
		auto file = count_lines(std::move(file_path));
		auto lock = mutex.lock();
		ret.totals.lines.total += file.lines.total;
		ret.totals.lines.loc += file.lines.loc;
		ret.totals.lines.empty += file.lines.empty;
		ret.files.push_back(std::move(file));
	};
	bool const use_threads = !cfg::test(cfg::flag::one_thread) && file_paths.size();
	loc::log(cfg::test(cfg::flag::debug), "  -- parsing ", file_paths.size(), " files\n");
	std::size_t count = 0;
	if (use_threads && g_workers.empty())
	{
		int count = (int)std::thread::hardware_concurrency() - 1;
		loc::log(cfg::test(cfg::flag::debug), "  -- launching ", count, " worker threads\n");
		for (; count > 0; --count)
		{
			g_workers.push_back(worker(g_queue));
		}
	}
	for (auto iter = file_paths.begin(); iter != file_paths.end(); ++iter)
	{
		if (use_threads)
		{
			g_queue.push([iter, process_file]() { process_file(iter); });
		}
		else
		{
			process_file(iter);
		}
		++count;
	}
	if (use_threads)
	{
		while (!g_queue.empty())
		{
			auto task = g_queue.pop();
			task();
		}
		auto residue = g_queue.flush();
		g_workers.clear();
		loc::log(cfg::test(cfg::flag::debug), "  -- worker threads completed\n");
		for (auto& task : residue)
		{
			task();
		}
	}
	loc::log(cfg::test(cfg::flag::debug), "\n");
	ret.totals.max_widths.total = width(ret.totals.lines.total);
	ret.totals.max_widths.loc = width(ret.totals.lines.loc);
	return ret;
}
