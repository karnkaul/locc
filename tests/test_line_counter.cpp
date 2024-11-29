#include <codes.hpp>
#include <klib/task/queue.hpp>
#include <klib/unit_test.hpp>
#include <locc/line_counter.hpp>
#include <array>
#include <filesystem>
#include <fstream>
#include <print>
#include <thread>

namespace {
using namespace locc;
using namespace std::chrono_literals;

namespace fs = std::filesystem;

constexpr std::string_view root_dir_v = "test_src";

struct File {
	File(File const&) = delete;
	File(File&&) = default;
	auto operator=(File const&) = delete;
	auto operator=(File&&) -> File& = default;

	explicit File(std::string path, std::string_view code) : path(std::move(path)), code(code) { create(); }

	void create() {
		ASSERT(!path.empty() && !code.empty());
		path = fs::current_path() / root_dir_v / path;
		auto const parent = path.parent_path();
		if (!fs::exists(parent)) {
			auto ec = std::error_code{};
			auto const dirs_created = fs::create_directories(parent, ec);
			ASSERT(dirs_created);
		}
		auto file = std::ofstream{path};
		ASSERT(file.good());
		file << code;
	}

	~File() {
		if (path.empty()) { return; }
		auto ec = std::error_code{};
		auto const files_removed = fs::remove(path, ec);
		EXPECT(files_removed);
	}

	fs::path path{};
	std::string_view code{};
};

struct Fixture {
	std::vector<Row> rows{};
	klib::task::Queue queue{};
	std::vector<Grammar> grammars{};

	static auto create(int worker_threads = 2) -> Fixture {
		return Fixture{
			.queue = klib::task::Queue{klib::task::QueueCreateInfo{.thread_count = klib::task::ThreadCount(worker_threads)}},
			.grammars = {Grammar::create(Grammar::Type::CLike), Grammar::create(Grammar::Type::ShellLike)},
		};
	}
};

void expect_cpp_code(Row const& row) {
	EXPECT(row.file_type == "C++");
	EXPECT(row.line_count.metrics[LineCount::Code] == 4);
	EXPECT(row.line_count.metrics[LineCount::Comments] == 2);
	EXPECT(row.line_count.metrics[LineCount::Empty] == 3);
	EXPECT(row.line_count.metrics[LineCount::Lines] == 9);
}

void expect_cmake_code(Row const& row) {
	EXPECT(row.file_type == "CMake script");
	EXPECT(row.line_count.metrics[LineCount::Code] == 3);
	EXPECT(row.line_count.metrics[LineCount::Comments] == 1);
	EXPECT(row.line_count.metrics[LineCount::Empty] == 3);
	EXPECT(row.line_count.metrics[LineCount::Lines] == 7);
}

TEST(line_counter_progress) {
	auto files = std::array{
		File{"src/hw.cpp", cpp_code_v},
		File{"CMakeLists.txt", cmake_code_v},
	};

	auto fixture = Fixture::create();
	auto const query = Query{
		.grammars = fixture.grammars,
		.path = root_dir_v,
	};
	auto counter = LineCounter{fixture.queue, query};
	fixture.queue.enqueue(counter);
	auto progress = counter.get_progress();
	ASSERT(progress.state != State::None);
	while (!progress.is_done()) {
		auto const latest_progress = counter.get_progress();
		if (latest_progress.state != progress.state) {
			std::println("=={}==", to_str(latest_progress.state));
			progress.state = latest_progress.state;
		}
		if (latest_progress != progress) {
			std::println("progress: [{}/{} {:.1f}]", latest_progress.counted, latest_progress.total, latest_progress.as_float());
			progress = latest_progress;
		}
		std::this_thread::yield();
	}

	ASSERT(counter.get_progress().is_done());
	auto rows = counter.to_rows();
	ASSERT(rows.size() == 2);
	sort_by_metric(rows, LineCount::Lines);

	expect_cpp_code(rows.at(0));
	EXPECT(rows.at(0).line_count.metrics[LineCount::Files] == 1);
	expect_cmake_code(rows.at(1));
	EXPECT(rows.at(1).line_count.metrics[LineCount::Files] == 1);
}

TEST(line_counter_wait) {
	auto files = std::array{
		File{"src/hw.cpp", cpp_code_v},
		File{"CMakeLists.txt", cmake_code_v},
	};

	auto fixture = Fixture::create();
	auto const query = Query{
		.grammars = fixture.grammars,
		.path = root_dir_v,
	};
	auto counter = LineCounter{fixture.queue, query};
	fixture.queue.enqueue(counter);
	ASSERT(counter.get_progress().state != State::None);
	counter.wait();
	ASSERT(counter.get_progress().state == State::Completed);
	auto rows = counter.to_rows();
	ASSERT(rows.size() == 2);
	sort_by_metric(rows, LineCount::Lines);

	expect_cpp_code(rows.at(0));
	EXPECT(rows.at(0).line_count.metrics[LineCount::Files] == 1);
	expect_cmake_code(rows.at(1));
	EXPECT(rows.at(1).line_count.metrics[LineCount::Files] == 1);
}

TEST(line_counter_bail) {
	auto files = std::array{
		File{"src/hw.cpp", cpp_code_v},
		File{"CMakeLists.txt", cmake_code_v},
	};

	auto counter = std::optional<LineCounter>{};
	{
		auto fixture = Fixture::create();
		auto const query = Query{
			.grammars = fixture.grammars,
			.path = root_dir_v,
		};
		counter.emplace(fixture.queue, query);
		fixture.queue.pause();
		fixture.queue.enqueue(*counter);
		ASSERT(counter->get_progress().state != State::None);
		fixture.queue.drain_and_wait();
	}
	auto const progress = counter->get_progress();
	EXPECT(progress.state == State::Dropped);
}

TEST(line_counter_filter) {
	auto files = std::array{
		File{"src/hw.cpp", cpp_code_v},
		File{"ext/one.cpp", cpp_code_v},
		File{"ext/two.cpp", cpp_code_v},
	};

	struct Filter : IFileFilter {
		[[nodiscard]] auto should_count(std::string_view path) const -> bool final { return !path.contains("ext/"); }
	};

	auto const filter = Filter{};

	auto fixture = Fixture::create();
	auto const query = Query{
		.grammars = fixture.grammars,
		.path = root_dir_v,
		.filter = &filter,
	};
	auto counter = LineCounter{fixture.queue, query};
	fixture.queue.enqueue(counter);
	counter.wait();
	auto const rows = counter.to_rows();
	ASSERT(rows.size() == 1);
	EXPECT(rows.front().line_count.metrics[LineCount::Files] == 1);
}
} // namespace
