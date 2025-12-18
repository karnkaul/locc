#include "locc/json_io.hpp"

void locc::from_json(dj::Json const& json, comment::Line& comment) { from_json(json["token"], comment.token); }

void locc::from_json(dj::Json const& json, comment::Block& comment) {
	from_json(json["open"], comment.open);
	from_json(json["close"], comment.close);
}

void locc::from_json(dj::Json const& json, Category& category) {
	from_json(json["name"], category.name);
	for (auto const& filename : json["exact_filenames"].as_array()) { from_json(filename, category.exact_filenames.emplace_back()); }
	for (auto const& extension : json["file_extensions"].as_array()) { from_json(extension, category.file_extensions.emplace_back()); }
}

void locc::from_json(dj::Json const& json, Alphabet& alphabet) {
	for (auto const& comment : json["line_comments"].as_array()) { from_json(comment, alphabet.line_comments.emplace_back()); }
	for (auto const& comment : json["block_comments"].as_array()) { from_json(comment, alphabet.block_comments.emplace_back()); }
}

void locc::from_json(dj::Json const& json, CodeFamily& code_family) {
	from_json(json["alphabet"], code_family.alphabet);
	for (auto const& category : json["categories"].as_array()) { from_json(category, code_family.categories.emplace_back()); }
}

void locc::from_json(dj::Json const& json, Specification& specification) {
	for (auto const& family : json["code_families"].as_array()) { from_json(family, specification.code_families.emplace_back()); }
	for (auto const& category : json["text_categories"].as_array()) { from_json(category, specification.text_categories.emplace_back()); }
	for (auto const& suffix : json["exclude_suffixes"].as_array()) { from_json(suffix, specification.exclude_suffixes.emplace_back()); }
}
