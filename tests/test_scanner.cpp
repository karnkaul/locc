#include <klib/unit_test.hpp>
#include <scanner.hpp>

namespace {
using namespace locc;

[[nodiscard]] auto make_scanner() -> Scanner {
	static auto const grammar_v = Grammar::create(Grammar::Type::CLike);
	auto ret = Scanner{grammar_v};
	return ret;
}

TEST(scanner_empty) {
	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type("");
	EXPECT(loc_type == LocType::Empty);
}

TEST(scanner_lc) {
	static constexpr std::string_view line_v = "  // line comment";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Comment);
}

TEST(scanner_loc) {
	static constexpr std::string_view line_v = "    int x{};";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Code);
}

TEST(scanner_loc_trailing_lc) {
	static constexpr std::string_view line_v = "    int x{}; // some comment";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Code);
}

TEST(scanner_loc_trailing_bc) {
	static constexpr std::string_view line_v = "    int x{}; /* some comment */";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Code);
}

TEST(scanner_loc_leading_bc) {
	static constexpr std::string_view line_v = "\t/* some comment */ int x{};";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Code);
}

TEST(scanner_full_bc) {
	static constexpr std::string_view line_v = "    /* some comment */";

	auto scanner = make_scanner();
	auto const loc_type = scanner.get_type(line_v);
	EXPECT(loc_type == LocType::Comment);
}

TEST(scanner_multi_bc) {
	auto scanner = make_scanner();

	auto line = std::string_view{"\tfoo(/*alpha=*/true, /*beta=*/0);"};
	auto loc_type = scanner.get_type(line);
	EXPECT(loc_type == LocType::Code);
	EXPECT(!scanner.in_block_comment());
}

TEST(scanner_multi_line_bc) {
	auto scanner = make_scanner();

	auto line = std::string_view{"    int x; /* bc open"};
	auto loc_type = scanner.get_type(line);
	EXPECT(loc_type == LocType::Code);
	EXPECT(scanner.in_block_comment());

	line = "\tcomment continued";
	loc_type = scanner.get_type(line);
	EXPECT(loc_type == LocType::Comment);
	EXPECT(scanner.in_block_comment());

	line = "    bc closed */ int y; /* another bc open";
	loc_type = scanner.get_type(line);
	EXPECT(loc_type == LocType::Code);
	EXPECT(scanner.in_block_comment());

	line = "\tbc closed */ int z;";
	loc_type = scanner.get_type(line);
	EXPECT(loc_type == LocType::Code);
	EXPECT(!scanner.in_block_comment());
}
} // namespace
