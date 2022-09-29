#pragma once

#include <RapidJson/stringbuffer.h>
#include <Mongoose/mongoose.h>
#include <string>
#include <vector>
#include <algorithm>

// One client has one address
// But can have multiple connections/ports
// The client has a cookie-key to authenticate
class ClientAuth
{
public:
	ClientAuth(struct mg_connection *c, int id);
	~ClientAuth();

	static const std::string getAddr(struct mg_connection *c);
	const std::string getAddr() const;
	const int getId() const;
	bool isClient(const std::string &addr, int id) const;

	void addConnection(struct mg_connection *c);
	void removeConnection(struct mg_connection *c);
	bool hasConnection(struct mg_connection *c) const;
	int getConnectionCount() const;

	void send(const rapidjson::StringBuffer &buffer);
	void close();

private:
	int id;
	const std::string addr;
	std::vector<struct mg_connection *> connections;
	const std::string challenge;
};
