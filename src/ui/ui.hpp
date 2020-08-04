#pragma once
#include <app/config.hpp>
#include <app/file_list_generator.hpp>

namespace locc
{
struct result;

bool parse_options(parser::key const& key, parser::value value);
void print_debug_prologue();
void print(result const& result);
void print_help();
void print_version();

std::stringstream& concat(std::stringstream& in);

template <typename Arg, typename... Args>
std::stringstream& concat(std::stringstream& in, Arg&& arg, Args&&... args);

template <typename X, typename... Args>
void xout([[maybe_unused]] X& ostream, [[maybe_unused]] Args&&... args);

template <typename... Args>
void log([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args);

template <typename... Args>
void err([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args);

template <typename... Args>
void log(Args&&... args);

template <typename... Args>
void err(Args&&... args);

template <typename... Args>
void log_force(Args&&... args);

template <typename F, typename... Args>
void do_if(bool predicate, F f, Args&&...);

template <typename Pred, typename F, typename... Args>
void do_if(Pred predicate, F f, Args&&...);

inline std::stringstream& concat(std::stringstream& in)
{
	return in;
}

template <typename Arg, typename... Args>
std::stringstream& concat(std::stringstream& in, Arg&& arg, Args&&... args)
{
	in << std::forward<Arg>(arg);
	return concat(in, std::forward<Args>(args)...);
}

template <typename X, typename... Args>
void xout([[maybe_unused]] X& ostream, [[maybe_unused]] Args&&... args)
{
	if constexpr (sizeof...(args) > 0)
	{
		std::stringstream str;
		concat(str, std::forward<Args>(args)...);
		ostream << str.str();
	}
}

template <typename... Args>
void log([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cout, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void err([[maybe_unused]] bool predicate, [[maybe_unused]] Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cerr, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void log(Args&&... args)
{
	log(true, std::forward<Args>(args)...);
}

template <typename... Args>
void err(Args&&... args)
{
	err(true, std::forward<Args>(args)...);
}

template <typename... Args>
void log_force(Args&&... args)
{
	xout(std::cout, std::forward<Args>(args)...);
}

template <typename F, typename... Args>
void do_if(bool predicate, F f, Args&&... args)
{
	if (predicate)
	{
		f(std::forward<Args>(args)...);
	}
}

template <typename Pred, typename F, typename... Args>
void do_if(Pred predicate, F f, Args&&... args)
{
	if (predicate())
	{
		f(std::forward<Args>(args)...);
	}
}
} // namespace locc
