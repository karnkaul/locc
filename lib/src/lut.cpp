#include <lut.hpp>

namespace locc {
namespace {
void add_entries(Lut& ret, Grammar const& grammar, FileType const& file_type) {
	auto const entry = Lut::Entry{.grammar = &grammar, .type_name = file_type.type_name};
	for (auto const& extension : file_type.extensions) { ret.by_extension.insert_or_assign(extension, entry); }
	if (!file_type.exact_filename.empty()) { ret.by_filename.insert_or_assign(file_type.exact_filename, entry); }
}
} // namespace

auto Lut::build(std::span<Grammar const> grammars) -> Lut {
	auto ret = Lut{};
	for (auto const& grammar : grammars) {
		for (auto const& file_type : grammar.file_types) {
			if (file_type.exact_filename.empty() && file_type.extensions.empty()) { continue; }
			add_entries(ret, grammar, file_type);
		}
	}
	return ret;
}

auto Lut::extension(std::string_view const ext) const -> Entry const* {
	if (auto const it = by_extension.find(ext); it != by_extension.end()) { return &it->second; }
	return nullptr;
}

auto Lut::filename(std::string_view const name) const -> Entry const* {
	if (auto const it = by_filename.find(name); it != by_filename.end()) { return &it->second; }
	return nullptr;
}
} // namespace locc
