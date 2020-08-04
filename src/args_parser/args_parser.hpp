#pragma once
#include <algorithm>
#include <string>
#include <deque>
#include <utility>
#if __has_include(<string_view>)
#if !defined(KT_STRING_VIEW)
#define KT_STRING_VIEW
#endif
#include <string_view>
#endif

namespace kt
{
template <template <typename...> typename T = std::deque, typename... Args>
struct args_parser
{
	using key = std::string;
	using value = std::string;

#if defined(KT_STRING_VIEW)
	using key_view = std::string_view;
	using value_view = std::string_view;
#endif

	struct entry
	{
		key k;
		value v;
	};

	using type_t = T<entry, Args...>;

	type_t entries;

	type_t const& parse(int argc, char const* const* argv)
	{
#if defined(KT_STRING_VIEW)
		using strng = std::string_view;
#else
		using strng = std::string;
#endif
		auto key_value = [](strng token, std::size_t equals) -> entry {
			auto k = key(token.substr(0, equals));
			auto v = equals < token.size() - 1 ? key(token.substr(equals + 1)) : value();
			return {std::move(k), std::move(v)};
		};
		entries.clear();
		for (int i = 0; i < argc; ++i)
		{
			strng token = argv[i];
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

	value const* find(key const& key) const
	{
		auto search = std::find_if(entries.begin(), entries.end(), [key](auto const& entry) -> bool { return entry.k == key; });
		if (search != entries.end())
		{
			return &search->v;
		}
		return nullptr;
	}
};

} // namespace kt
