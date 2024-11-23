#pragma once
#include <djson/json.hpp>
#include <locc/grammar.hpp>

namespace locc {
void to_json(dj::Json& out, LineComment const& line_comment);
void from_json(dj::Json const& json, LineComment& out);

void to_json(dj::Json& out, BlockComment const& block_comment);
void from_json(dj::Json const& json, BlockComment& out);

void to_json(dj::Json& out, FileType const& file_type);
void from_json(dj::Json const& json, FileType& out);

void to_json(dj::Json& out, Grammar const& grammar);
void from_json(dj::Json const& json, Grammar& out);
} // namespace locc
