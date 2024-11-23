#pragma once
#include <locc/build_version.hpp>
#include <locc/file_filter.hpp>
#include <params.hpp>

namespace locc::cli {
class App {
  public:
	auto run(int argc, char const* const* argv) -> int;

  private:
	auto execute() -> int;

	Params m_params{};
	DefaultFileFilter m_filter{};
};
} // namespace locc::cli
