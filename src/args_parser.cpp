#include <algorithm>
#include <string_view>
#include <args_parser.hpp>

std::deque<ap::entry> ap::parse(int argc, char const* const* argv)
{
	auto key_value = [](std::string_view token, std::size_t equals) -> std::pair<key, value> {
		auto k = key(token.substr(0, equals));
		auto v = equals < token.size() - 1 ? key(token.substr(equals + 1)) : value();
		return {std::string(k), std::string(v)};
	};
	std::deque<entry> ret;
	for (int i = 0; i < argc; ++i)
	{
		std::string_view token = argv[i];
		if (token.size() > 1)
		{
			if (token.at(0) == '-')
			{
				if (token.at(1) == '-')
				{
					token = token.substr(2);
					key k;
					value v;
					std::size_t const equals = token.find('=');
					if (equals != std::string::npos)
					{
						auto [k, v] = key_value(token, equals);
						ret.push_back({std::move(k), std::move(v)});
					}
					else
					{
						ret.push_back({std::string(token), {}});
					}
				}
				else
				{
					token = token.substr(1);
					std::size_t const equals = token.find('=');
					if (equals != std::string::npos)
					{
						auto [k, v] = key_value(token, equals);
						ret.push_back({std::move(k), std::move(v)});
					}
					else
					{
						for (auto c : token)
						{
							ret.push_back({ap::key(1, c), {}});
						}
					}
				}
			}
			else
			{
				ret.push_back({ap::key(token), {}});
			}
		}
	}
	return ret;
}

ap::value const* ap::find(std::deque<entry> const& entries, key const& key)
{
	auto search = std::find_if(entries.begin(), entries.end(), [key](auto const& entry) -> bool { return entry.first == key; });
	if (search != entries.end())
	{
		auto& [_, value] = *search;
		return &value;
	}
	return nullptr;
}
