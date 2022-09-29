#include "ClientManager.h"

ClientManager::ClientManager() {
}

ClientManager::~ClientManager() {
	for (auto client : clients) {
		delete client;
	}
	clients.clear();
	for (auto client : clientsConnecting) {
		delete client;
	}
	clientsConnecting.clear();
}

void ClientManager::onConnect(mg_connection *c) {
	ClientChallenge *clientC = new ClientChallenge(c);
	clientsConnecting.push_back(clientC);
	clientC->sendChallenge();
}

void ClientManager::onAuth(mg_connection *c, const std::string addr, const std::string &clientChallenge, const std::string &key, const std::string &serverPassword, int id) {
	ClientChallenge *clientC = getClientConnecting(c);
	if (clientC == nullptr) {
		DEBUG_LOG("[WebServer] Client wants to auth but not connected " + addr);
		cancelAuth(c); // No client
	}

	bool logged = clientC->onChallengeAnswered(clientChallenge, key, serverPassword);
	if (!logged) {
		std::cout << "[WebServer] SecretKey isn't correct for " + addr << std::endl;
		JsonMessage::typeConnection(STATUS_CONNECTION_LOGIN_WRONG_PASSWORD).send(c);
		cancelAuth(c, false); // Wrong password
		return;
	}

	if (id != -1) {
		ClientAuth *client = getClient(c);
		if (client != nullptr && client->isClient(addr, id)) {
			client->addConnection(c);
			onAuthConfirmed(c, client);
			return;
		}
	}
	ClientAuth *client = new ClientAuth(c, ++clientId);
	clients.push_back(client);

	onAuthConfirmed(c, client);
}

void ClientManager::onAuthConfirmed(mg_connection *c, ClientAuth *client) {
	JsonMessage::typeConnection(STATUS_CONNECTION_LOGIN_SUCCESS).Add("id", std::to_string(client->getId())).send(c);
	sendClientCount("New client is here");
	ClientChallenge *clientC = getClientConnecting(c);
	removeAuth(clientC);
}

void ClientManager::cancelAuth(mg_connection *c, bool sendMessage) {
	ClientChallenge *clientC = getClientConnecting(c);
	if (clientC != nullptr) {
		removeAuth(clientC);
		if (sendMessage) JsonMessage::typeConnection(STATUS_CONNECTION_LOGIN_CANCEL).send(c);
		c->is_draining = true;
	}
}

void ClientManager::removeAuth(ClientChallenge *clientC) {
	clientsConnecting.erase(std::remove(clientsConnecting.begin(), clientsConnecting.end(), clientC), clientsConnecting.end());
}

void ClientManager::onDisconnect(mg_connection *c) {
	cancelAuth(c);
	ClientAuth *client = getClient(c);
	if (client != nullptr) {
		client->removeConnection(c);
		sendClientCount("Client had left");
	}
}

bool ClientManager::isAuth(mg_connection *c) {
	return getClient(c) != nullptr;
}

ClientAuth *ClientManager::getClient(mg_connection *c) {
	for (auto client : clients) {
		if (client->hasConnection(c)) {
			return client;
		}
	}
	return nullptr;
}

ClientChallenge *ClientManager::getClientConnecting(mg_connection *c) {
	for (auto client : clientsConnecting) {
		if (client->isConnection(c)) {
			return client;
		}
	}
	return nullptr;
}

void ClientManager::send(const rapidjson::StringBuffer &buffer) {
	for (size_t i = 0; i < clients.size(); ++i) {
		clients.at(i)->send(buffer);
	}
}

void ClientManager::send(const rapidjson::StringBuffer &buffer, mg_connection *c) {
	mg_ws_send(c, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
}

void ClientManager::sendClientCount(const std::string &msg) {
	int nb_clients = clients.size();
	int nb_connections = 0;
	for (auto client : clients) {
		nb_connections += client->getConnectionCount();
	}
	JsonMessage json("clientCount");
	json.Add("nb_clients", std::to_string(nb_clients));
	json.Add("nb_connections", std::to_string(nb_connections));
	json.Add("msg", msg);
	send(json.stringify());
}
