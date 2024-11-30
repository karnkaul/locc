#include <codes.hpp>
#include <klib/unit_test.hpp>
#include <locc/count_lines.hpp>
#include <sstream>

namespace {
using namespace locc;

TEST(counter_text) {
	auto text = std::stringstream{R"(
random text file
// pseudo lc
/* psuedo bc */

)"};
	auto const line_count = count_lines(text, {});
	EXPECT(line_count.metrics[LineCount::Code] == 0);
	EXPECT(line_count.metrics[LineCount::Comments] == 0);
	EXPECT(line_count.metrics[LineCount::Empty] == 2);
	EXPECT(line_count.metrics[LineCount::Lines] == 5);
}

TEST(counter_cpp) {
	auto text = std::stringstream{cpp_code_v.data()};

	auto const grammar = Grammar::create(Grammar::Type::CLike);
	auto const line_count = count_lines(text, grammar);
	EXPECT(line_count.metrics[LineCount::Code] == 4);
	EXPECT(line_count.metrics[LineCount::Comments] == 2);
	EXPECT(line_count.metrics[LineCount::Empty] == 3);
	EXPECT(line_count.metrics[LineCount::Lines] == 9);
}

TEST(counter_cmake) {
	auto const grammar = Grammar::create(Grammar::Type::ShellLike);
	auto const line_count = count_lines(cmake_code_v, grammar);
	EXPECT(line_count.metrics[LineCount::Code] == 3);
	EXPECT(line_count.metrics[LineCount::Comments] == 1);
	EXPECT(line_count.metrics[LineCount::Empty] == 3);
	EXPECT(line_count.metrics[LineCount::Lines] == 7);
}
} // namespace
