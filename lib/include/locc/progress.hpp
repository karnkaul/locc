#pragma once
#include <cstdint>
#include <string_view>

namespace locc {
enum class State : int { None, Dropped, Waiting, Initializing, Collecting, Counting, Completed };

struct Progress {
	State state{};

	std::uint64_t counted{};
	std::uint64_t total{};

	[[nodiscard]] constexpr auto is_done() const -> bool { return state == State::Completed || state == State::Dropped; }

	[[nodiscard]] constexpr auto as_float() const -> float {
		if (total == 0) { return 0.0f; }
		return float(counted) / float(total);
	}

	auto operator==(Progress const&) const -> bool = default;
};

constexpr auto to_str(State const state) -> std::string_view {
	switch (state) {
	default:
	case State::None: return "None";
	case State::Dropped: return "Dropped";
	case State::Waiting: return "Waiting";
	case State::Initializing: return "Initializing";
	case State::Collecting: return "Collecting";
	case State::Counting: return "Counting";
	case State::Completed: return "Completed";
	}
}
} // namespace locc
