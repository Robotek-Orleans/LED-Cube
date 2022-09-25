#pragma once

#include "ClientAuth.h"
#include "ClientChallenge.h"
#include "JsonMessage.h"
#include <iostream>
#include <algorithm>

#define DEBUG 1
#if DEBUG
#define DEBUG_LOG(x) std::cout << x << std::endl;
#else
#define DEBUG_LOG(x)
#endif

// Store Clients and manage connections/disconnections;
// The client has to auth with the server password or with its cookie-key;
class ClientManager
{
public:
	ClientManager();
	~ClientManager();

	// On connection
	void onConnect(mg_connection *c);
	void onAuth(mg_connection *c, const std::string addr, const std::string &clientChallenge, const std::string &key, const std::string &serverPassword, int id);
	void onAuthConfirmed(mg_connection *c, ClientAuth *client);
	// On disconnection
	void cancelAuth(mg_connection *c, bool sendMessage = true);
	void removeAuth(ClientChallenge *clientC);
	void onDisconnect(mg_connection *c);

	bool isAuth(mg_connection *c);
	ClientAuth *getClient(mg_connection *c);
	ClientChallenge *getClientConnecting(mg_connection *c);

	// Send message to all clients
	void send(const rapidjson::StringBuffer &buffer);
	void send(const rapidjson::StringBuffer &buffer, mg_connection *c);
	void sendClientCount(const std::string &msg);

private:
	int clientId = 0;
	std::vector<ClientAuth *> clients;
	std::vector<ClientChallenge *> clientsConnecting;
};