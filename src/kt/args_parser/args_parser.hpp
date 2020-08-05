#pragma once
#include <algorithm>
#include <deque>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace kt
{
///
/// \brief Container wrapper for parsing command line arguments
///
/// Features:
/// 	- Parse input in the form of `-x[=y] --foo[=bar] -abcd`
/// 	- Store results as key-value string pairs (value is empty unless `=` is present in the token)
/// 	- Search for values among parsed keys (returns nullptr if not found)
///
template <template <typename...> typename Cont = std::deque, typename... Args>
struct args_parser
{
	using key = std::string;
	using value = std::string;
	using key_view = std::string_view;
	using value_view = std::string_view;

	template <typename T>
	constexpr static bool is_string = std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>;

	///
	/// \brief Each entry is a key-value pair
	///
	struct entry
	{
		key k;
		value v;
	};
	///
	/// \brief Type of the container holding entries
	///
	using type_t = Cont<entry, Args...>;

	///
	/// \brief Container holding entries
	///
	type_t entries;

	///
	/// \brief Parse given arguments into entries
	///
	type_t const& parse(int argc, char const* const* argv);
	///
	/// \brief Parse given arguments into entries
	///
	template <template <typename...> typename C, typename T, typename... CArgs>
	type_t const& parse(C<T, CArgs...> const& input);
	///
	/// \brief Search for a value among parsed keys, given a key
	/// \returns Pointer to found value or nullptr
	///
	value const* find(key const& key) const;
};

template <template <typename...> typename Cont, typename... Args>
typename args_parser<Cont, Args...>::type_t const& args_parser<Cont, Args...>::parse(int argc, char const* const* argv)
{
	auto key_value = [](std::string_view token, std::size_t equals) -> entry {
		auto k = key(token.substr(0, equals));
		auto v = equals < token.size() - 1 ? key(token.substr(equals + 1)) : value();
		return {std::move(k), std::move(v)};
	};
	entries.clear();
	for (int i = 0; i < argc; ++i)
	{
		std::string_view token = argv[i];
		if (!token.empty())
		{
			if (token.at(0) == '-')
			{
				if (token.size() > 1 && token.at(1) == '-')
				{
					token = token.substr(2);
					key k;
					value v;
					std::size_t const equals = token.find('=');
					if (equals != std::string::npos)
					{
						entries.push_back(key_value(std::move(token), equals));
					}
					else
					{
						entries.push_back({key(std::move(token)), {}});
					}
				}
				else
				{
					token = token.substr(1);
					std::size_t const equals = token.find('=');
					if (equals != std::string::npos)
					{
						entries.push_back(key_value(std::move(token), equals));
					}
					else
					{
						for (auto c : token)
						{
							entries.push_back({key(1, c), {}});
						}
					}
				}
			}
			else
			{
				entries.push_back({key(std::move(token)), {}});
			}
		}
	}
	return entries;
}

template <template <typename...> typename Cont, typename... Args>
template <template <typename...> typename C, typename T, typename... CArgs>
typename args_parser<Cont, Args...>::type_t const& args_parser<Cont, Args...>::parse(C<T, CArgs...> const& input)
{
	static_assert(is_string<T>, "Invalid type!");
	std::vector<char const*> args;
	args.reserve(input.size());
	for (auto const& str : input)
	{
		args.push_back(str.data());
	}
	return parse((int)args.size(), args.data());
}

template <template <typename...> typename Cont, typename... Args>
typename args_parser<Cont, Args...>::value const* args_parser<Cont, Args...>::find(key const& key) const
{
	auto search = std::find_if(entries.begin(), entries.end(), [key](auto const& entry) -> bool { return entry.k == key; });
	if (search != entries.end())
	{
		return &search->v;
	}
	return nullptr;
}
} // namespace kt
