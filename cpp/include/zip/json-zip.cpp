#include "json-zip.h"

std::string jsonzip::encodeText(const std::string &s) {
	gzip::Data data = gzip::AllocateData(s.size());
	memcpy(data->ptr, s.c_str(), s.size());
	gzip::Comp comp(jsonzip::level);
	gzip::DataList data_list = comp.Process(data, true);
	gzip::Data zip = gzip::ExpandDataList(data_list);
	std::string zip_str(zip->ptr, zip->ptr + zip->size);
	std::string base64 = base64_encode(zip_str);
	return '0' + base64;
}

std::string jsonzip::decodeText(const std::string &s) {
	_ASSERT_EXPR(s[0] == '0', "Invalid jsonzip string, must start with '0'");
	std::string base64 = s.substr(1);
	std::string zip_str = base64_decode(base64);
	gzip::Data zip = gzip::AllocateData(zip_str.size());
	memcpy(zip->ptr, zip_str.c_str(), zip_str.size());
	gzip::Decomp decomp;
	std::tuple<bool, gzip::DataList> result = decomp.Process(zip);
	_ASSERT_EXPR(std::get<0>(result), "Invalid jsonzip string, decompression failed");
	gzip::DataList data_list = std::get<1>(result);
	gzip::Data data = gzip::ExpandDataList(data_list);
	std::string s2(data->ptr, data->ptr + data->size);
	return s2;
}

std::string jsonzip::encodeJson(const rapidjson::Document &s) {
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	s.Accept(writer);
	return jsonzip::encodeText(buffer.GetString());
}

JsonReader &jsonzip::decodeJson(const std::string &s, JsonReader &document) {
	std::string s2 = jsonzip::decodeText(s);
	document.Parse(s2.c_str());
	return document;
}
