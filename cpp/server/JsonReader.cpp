#include "JsonReader.h"

JsonReader::JsonReader() {
}

JsonReader::JsonReader(const std::string &jsonString) {
	Parse(jsonString.c_str());
}

std::string JsonReader::getString(std::string key, std::string defaultValue) const {
	const char *keyChar = key.c_str();
	return HasMember(keyChar) && (*this)[keyChar].IsString() ? (*this)[keyChar].GetString() : defaultValue;
}

bool JsonReader::getBool(std::string key, bool defaultValue) const {
	const char *keyChar = key.c_str();
	return HasMember(keyChar) && (*this)[keyChar].IsBool() ? (*this)[keyChar].GetBool() : defaultValue;
}

int JsonReader::getInt(std::string key, int defaultValue) const {
	const char *keyChar = key.c_str();
	return HasMember(keyChar) && (*this)[keyChar].IsInt() ? (*this)[keyChar].GetInt() : defaultValue;
}

long long JsonReader::getLongLong(std::string key, long long defaultValue) const {
	const char *keyChar = key.c_str();
	return HasMember(keyChar) && (*this)[keyChar].IsInt64() ? (*this)[keyChar].GetInt64() : defaultValue;
}

double JsonReader::getDouble(std::string key, double defaultValue) const {
	const char *keyChar = key.c_str();
	return HasMember(keyChar) && (*this)[keyChar].IsDouble() ? (*this)[keyChar].GetDouble() : defaultValue;
}
