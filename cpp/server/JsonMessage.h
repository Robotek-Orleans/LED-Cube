#pragma once

#ifdef RAPIDJSON_HAS_STDSTRING
#undef RAPIDJSON_HAS_STDSTRING
#endif
#define RAPIDJSON_HAS_STDSTRING 1

#include <string>
#include <RapidJson/document.h>
#include <RapidJson/stringbuffer.h>
#include <RapidJson/writer.h>
#include <Mongoose/mongoose.h>

#define STATUS_CONNECTION_LOGIN_SUCCESS "login.success"
#define STATUS_CONNECTION_LOGIN_WRONG_PASSWORD "login.failed.wrong_password"
#define STATUS_CONNECTION_LOGIN_CANCEL "login.failed.cancel"

class JsonMessage
{
public:
	JsonMessage(std::string type);
	JsonMessage(const JsonMessage &other);
	JsonMessage &Add(std::string key, std::string value);
	JsonMessage &Add(std::string key, const char *value);
	JsonMessage &Add(std::string key, bool value);
	JsonMessage &Add(std::string key, int value);
	JsonMessage &Add(std::string key, long long value);
	JsonMessage &Add(std::string key, double value);
	rapidjson::Document &GetDocument();
	rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &GetAllocator();
	rapidjson::StringBuffer stringify() const;
	void send(mg_connection *c) const;

	static JsonMessage typeConnection(const char status[]) { return JsonMessage("connection").Add("status", status); }

private:
	rapidjson::Document jsonDoc;
};
