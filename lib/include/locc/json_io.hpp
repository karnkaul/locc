#pragma once
#include "locc/specification.hpp"
#include <djson/json.hpp>

namespace locc {
void from_json(dj::Json const& json, comment::Line& comment);
void from_json(dj::Json const& json, comment::Block& comment);
void from_json(dj::Json const& json, Category& category);
void from_json(dj::Json const& json, Alphabet& alphabet);
void from_json(dj::Json const& json, CodeFamily& code_family);
void from_json(dj::Json const& json, Specification& specification);
} // namespace locc
