#include "JsonMessage.h"

JsonMessage::JsonMessage(std::string type) {
	jsonDoc.SetObject();
	jsonDoc.AddMember("type", type, jsonDoc.GetAllocator());
}

JsonMessage::JsonMessage(const JsonMessage &other) {
	jsonDoc.CopyFrom(other.jsonDoc, jsonDoc.GetAllocator());
}

JsonMessage &JsonMessage::Add(std::string key, std::string value) {
	rapidjson::Value name = rapidjson::Value(rapidjson::kStringType);
	name.SetString(key.c_str(), key.length(), jsonDoc.GetAllocator());

	rapidjson::Value valueValue = rapidjson::Value(rapidjson::kStringType);
	valueValue.SetString(value.c_str(), value.length(), jsonDoc.GetAllocator());

	if (jsonDoc.HasMember(name)) {
		jsonDoc[name] = valueValue;
	} else {
		jsonDoc.AddMember(name, valueValue, jsonDoc.GetAllocator());
	}
	return *this;
}

JsonMessage &JsonMessage::Add(std::string key, const char *value) {
	return Add(key, std::string(value));
}

JsonMessage &JsonMessage::Add(std::string key, bool value) {
	rapidjson::Value name = rapidjson::Value(rapidjson::kStringType);
	name.SetString(key.c_str(), key.length(), jsonDoc.GetAllocator());

	if (jsonDoc.HasMember(name)) {
		jsonDoc[name] = value;
	} else {
		jsonDoc.AddMember(name, value, jsonDoc.GetAllocator());
	}
	return *this;
}

JsonMessage &JsonMessage::Add(std::string key, int value) {
	rapidjson::Value name = rapidjson::Value(rapidjson::kStringType);
	name.SetString(key.c_str(), key.length(), jsonDoc.GetAllocator());

	if (jsonDoc.HasMember(name)) {
		jsonDoc[name] = value;
	} else {
		jsonDoc.AddMember(name, value, jsonDoc.GetAllocator());
	}
	return *this;
}

JsonMessage &JsonMessage::Add(std::string key, long long value) {
	rapidjson::Value name = rapidjson::Value(rapidjson::kStringType);
	name.SetString(key.c_str(), key.length(), jsonDoc.GetAllocator());

	if (jsonDoc.HasMember(name)) {
		jsonDoc[name] = (int64_t)value;
	} else {
		jsonDoc.AddMember(name, (int64_t)value, jsonDoc.GetAllocator());
	}
	return *this;
}

JsonMessage &JsonMessage::Add(std::string key, double value) {
	rapidjson::Value name = rapidjson::Value(rapidjson::kStringType);
	name.SetString(key.c_str(), key.length(), jsonDoc.GetAllocator());

	if (jsonDoc.HasMember(name)) {
		jsonDoc[name] = value;
	} else {
		jsonDoc.AddMember(name, value, jsonDoc.GetAllocator());
	}
	return *this;
}

rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &JsonMessage::GetAllocator() {
	return jsonDoc.GetAllocator();
}

rapidjson::Document &JsonMessage::GetDocument() {
	return jsonDoc;
}

rapidjson::StringBuffer JsonMessage::stringify() const {
	// Stringify the DOM
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	jsonDoc.Accept(writer);
	return buffer;
}

void JsonMessage::send(mg_connection *c) const {
	rapidjson::StringBuffer buffer = stringify();
	mg_ws_send(c, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
}
