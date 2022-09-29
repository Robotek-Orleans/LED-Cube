#include "ClientAuth.h"

ClientAuth::ClientAuth(struct mg_connection *c, int id)
	: addr(getAddr(c)), id(id) {
	connections.push_back(c);
}

ClientAuth::~ClientAuth() {
	close();
}

const std::string ClientAuth::getAddr(struct mg_connection *c) {
	char addr[32];
	mg_straddr(&c->rem, addr, sizeof(addr));
	return addr;
}

const std::string ClientAuth::getAddr() const {
	return addr;
}

const int ClientAuth::getId() const {
	return id;
}

bool ClientAuth::isClient(const std::string &addr, int id) const {
	return this->addr == addr && this->id == id;
}

void ClientAuth::addConnection(struct mg_connection *c) {
	connections.push_back(c);
}

void ClientAuth::removeConnection(struct mg_connection *c) {
	connections.erase(std::remove(connections.begin(), connections.end(), c), connections.end());
	c->is_draining = true;
}

bool ClientAuth::hasConnection(struct mg_connection *c) const {
	return std::find(connections.begin(), connections.end(), c) != connections.end();
}

int ClientAuth::getConnectionCount() const {
	return connections.size();
}

void ClientAuth::send(const rapidjson::StringBuffer &buffer) {
	for (auto connection : connections)
		mg_ws_send(connection, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
}

void ClientAuth::close() {
	for (auto connection : connections) {
		connection->is_draining = true;
	}
}
