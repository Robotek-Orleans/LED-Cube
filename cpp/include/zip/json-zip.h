#pragma once

#include "base64.h"
#include "gzip_cpp.h"
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>
#include <RapidJson/writer.h>
#include "../../server/JsonReader.h"
#include <unistd-compat.h>

namespace jsonzip {
	static const gzip::Comp::Level level = gzip::Comp::Level::Max; // level 9
	static const bool last_block = true;						   // same result as pako.js when true

	std::string encodeText(const std::string &s);
	std::string decodeText(const std::string &s);

	std::string encodeJson(const rapidjson::Document &s);
	JsonReader &decodeJson(const std::string &s, JsonReader &document);

} // namespace jsonzip