#include "ClientChallenge.h"

// Generate a random challenge (32 bytes)
std::string generateRandomString(int length) {
	std::string result;
	for (int i = 0; i < length; i++) {
		result += (char)('a' + rand() % 26);
	}
	return result;
}

ClientChallenge::ClientChallenge(struct mg_connection *c)
	: connection(c), serverChallenge(generateRandomString(32)) {
}

ClientChallenge::~ClientChallenge() {
}

std::string ClientChallenge::getChallenge() const {
	return serverChallenge;
}

bool ClientChallenge::onChallengeAnswered(const std::string &clientChallenge, const std::string &receivedKey, const std::string &serverPassword) {
	// First hash serverPassword + clientChallenge without salt (not random)
	std::string cookieKey = hash(serverPassword, clientChallenge);

	// Then receivedKey must be a hash of cookieKey + serverChallenge
	bool valid = bcrypt::validatePassword(cookieKey + serverChallenge, receivedKey);

	return valid;
}

bool ClientChallenge::isConnection(mg_connection *connection) {
	return this->connection == connection;
}

void ClientChallenge::sendChallenge() {
	JsonMessage("auth").Add("content", "challenge").Add("challenge", serverChallenge).send(connection);
}

void node_bcrypt(const char *key, size_t key_len, const char *salt, char *encrypted);
std::string ClientChallenge::hash(const std::string key, const std::string salt) {
	std::string hash(61, '\0');
	node_bcrypt(key.c_str(), key.size(), salt.c_str(), &hash[0]);
	hash.resize(60);
	return hash;
}
