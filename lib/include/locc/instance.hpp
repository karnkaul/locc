#pragma once
#include "locc/init_info.hpp"
#include "locc/result.hpp"
#include <memory>

namespace locc {
/// \brief Central user API.
class Instance : public klib::Polymorphic {
  public:
	/// \returns Non-null concrete instance.
	[[nodiscard]] static auto create(InitInfo init_info = {}) -> std::unique_ptr<Instance>;

	/// \returns Number of worker threads.
	[[nodiscard]] virtual auto get_thread_count() const -> ThreadCount = 0;

	/// \returns Result summarizing file counts.
	[[nodiscard]] virtual auto process(std::string_view path) -> Result = 0;
};
} // namespace locc
