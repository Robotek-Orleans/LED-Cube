#include "ServerFriend.h"

ServerFriend::ServerFriend() {
	m_messageEmitter = 0;
}

ServerFriend::~ServerFriend() {
	m_messageEmitter = 0;
}

// Send a message to the server.
void ServerFriend::setMessageHandler(std::function<void(std::string, std::string)> handler) {
	m_messageEmitter = handler;
}

void ServerFriend::sendMessage(std::string type, std::string content) {
	if (m_messageEmitter) {
		m_messageEmitter(type, content);
	}
}