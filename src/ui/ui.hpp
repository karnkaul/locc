#pragma once
#include <app/config.hpp>
#include <app/file_list_generator.hpp>
#include <clap/interpreter.hpp>
#include <kt/str_format/str_format.hpp>

namespace locc {
struct result;

clap::interpreter::spec_t::main_t options_cmd();

void print_debug_prologue();
void print(result const& result);
void print_help();
void print_version();

std::stringstream& concat(std::stringstream& in);

template <typename Arg, typename... Args>
std::stringstream& concat(std::stringstream& in, Arg&& arg, Args&&... args);

template <typename X, typename... Args>
requires requires(X x) { x << std::declval<std::string>(); }
void xout(X& ostream, std::string_view fmt, Args&&... args) {
	std::stringstream str;
	kt::format_str(str, fmt, std::forward<Args>(args)...);
	ostream << str.str();
}

template <typename... Args>
void log_if(bool predicate, std::string_view fmt, Args&&... args) {
	if (predicate && !cfg::test(cfg::flag::quiet)) {
		xout(std::cout, fmt, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void err_if(bool predicate, std::string_view fmt, Args&&... args) {
	if (predicate && !cfg::test(cfg::flag::quiet)) {
		xout(std::cerr, fmt, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void log_force(std::string_view fmt, Args&&... args) {
	xout(std::cout, fmt, std::forward<Args>(args)...);
}

template <typename F, typename... Args>
void do_if(bool predicate, F f, Args&&... args) {
	if (predicate) {
		f(std::forward<Args>(args)...);
	}
}

template <typename Pred, typename F, typename... Args>
void do_if(Pred predicate, F f, Args&&... args) {
	if (predicate()) {
		f(std::forward<Args>(args)...);
	}
}
} // namespace locc
