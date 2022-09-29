#pragma once

#include <string>
#include <RapidJson/document.h>

class JsonReader : public rapidjson::Document
{
public:
	JsonReader();
	JsonReader(const std::string &jsonString);

	std::string getString(std::string key, std::string defaultValue) const;
	bool getBool(std::string key, bool defaultValue) const;
	int getInt(std::string key, int defaultValue) const;
	long long getLongLong(std::string key, long long defaultValue) const;
	double getDouble(std::string key, double defaultValue) const;
};