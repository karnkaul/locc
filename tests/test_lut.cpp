#include <klib/unit_test.hpp>
#include <lut.hpp>

namespace {
using namespace locc;

TEST(lut_c_like) {
	auto const& grammar = Grammar::create(Grammar::Type::CLike);
	auto lut = Lut::build({&grammar, 1});

	for (auto const& file_type : grammar.file_types) {
		for (auto const& extension : file_type.extensions) {
			auto const* entry = lut.extension(extension);
			ASSERT(entry != nullptr);
			EXPECT(entry->grammar == &grammar);
			EXPECT(entry->type_name == file_type.type_name);
		}
	}
}

TEST(lut_cmake) {
	auto const& grammar = Grammar::create(Grammar::Type::ShellLike);
	auto lut = Lut::build({&grammar, 1});

	for (auto const& file_type : grammar.file_types) {
		for (auto const& extension : file_type.extensions) {
			auto const* entry = lut.extension(extension);
			ASSERT(entry != nullptr);
			EXPECT(entry->grammar == &grammar);
			EXPECT(entry->type_name == file_type.type_name);
		}
		if (!file_type.exact_filename.empty()) {
			auto const* entry = lut.filename(file_type.exact_filename);
			ASSERT(entry != nullptr);
			EXPECT(entry->grammar == &grammar);
			EXPECT(entry->type_name == file_type.type_name);
		}
	}
}
} // namespace
