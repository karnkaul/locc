#include <locc/count_lines.hpp>
#include <scanner.hpp>
#include <utility>

namespace {
struct StreamSplitter {
	std::istream& in;

	std::string line{};

	explicit StreamSplitter(std::istream& in) : in(in) {}

	auto operator()(std::string_view& out) -> bool {
		if (!std::getline(in, line)) { return false; }
		out = line;
		return true;
	}
};

struct TextSplitter {
	std::string_view text{};

	explicit TextSplitter(std::string_view const text) : text(text) {}

	auto operator()(std::string_view& out) -> bool {
		if (text.empty()) { return false; }
		auto const i = text.find_first_of('\n');
		if (i == std::string_view::npos) {
			out = text;
			text = {};
			return true;
		}
		out = text.substr(0, i);
		text = text.substr(i + 1);
		return true;
	}
};

template <typename SplitterT>
auto count_lines(SplitterT& splitter, locc::Grammar const& grammar) -> locc::LineCount {
	using namespace locc;
	using enum LocType;
	auto ret = LineCount{};
	auto scanner = Scanner{grammar};
	auto line = std::string_view{};
	while (splitter(line)) {
		++ret.metrics[LineCount::Lines];
		switch (scanner.get_type(line)) {
		case Code: ++ret.metrics[LineCount::Code]; break;
		case Comment: ++ret.metrics[LineCount::Comments]; break;
		case Empty: ++ret.metrics[LineCount::Empty]; break;
		case None: break;
		default: std::unreachable();
		}
	}
	return ret;
}
} // namespace

auto locc::count_lines(std::istream& in, Grammar const& grammar) -> LineCount {
	try {
		auto splitter = StreamSplitter{in};
		return count_lines(splitter, grammar);
	} catch (...) { return {}; }
}

auto locc::count_lines(std::string_view const text, Grammar const& grammar) -> LineCount {
	try {
		auto splitter = TextSplitter{text};
		return count_lines(splitter, grammar);
	} catch (...) { return {}; }
}
