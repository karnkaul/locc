#pragma once
#include <klib/demangle.hpp>
#include <klib/log.hpp>

namespace locc {
/// \brief Typed tagged logger.
template <typename Type>
class Logger : public klib::TaggedLogger {
  public:
	explicit(false) Logger() : klib::TaggedLogger(klib::demangled_name<Type>()) {}
};
} // namespace locc
