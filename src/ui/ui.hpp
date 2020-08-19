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

inline std::ostream& format(std::ostream& out, std::string_view fmt)
{
	out << fmt;
	return out;
}

template <typename Arg, typename... Args>
std::ostream& format(std::ostream& out, std::string_view fmt, Arg&& arg, Args&&... args)
{
	if (auto search = fmt.find("{}"); search != std::string::npos)
	{
		std::string_view text(fmt.data(), search);
		out << text << std::forward<Arg>(arg);
		return format(out, fmt.substr(search + 2), std::forward<Args>(args)...);
	}
	return format(out, fmt);
}

template <typename X, typename... Args>
void xout(X& ostream, std::string_view fmt, Args&&... args)
{
	std::stringstream str;
	format(str, fmt, std::forward<Args>(args)...);
	ostream << str.str();
}

template <typename... Args>
void log_if(bool predicate, std::string_view fmt, Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cout, fmt, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void err_if(bool predicate, std::string_view fmt, Args&&... args)
{
	if (predicate && !cfg::test(cfg::flag::quiet))
	{
		xout(std::cerr, fmt, std::forward<Args>(args)...);
	}
}

template <typename... Args>
void log_force(std::string_view fmt, Args&&... args)
{
	xout(std::cout, fmt, std::forward<Args>(args)...);
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
