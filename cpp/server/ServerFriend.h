#pragma once
#include <string>
#include <functional>
#include "JsonMessage.h"
#include <Mongoose/mongoose.h>
#include "JsonReader.h"

// Receive and send messages from/to the server.
class ServerFriend
{
public:
	ServerFriend();
	virtual ~ServerFriend();

	// Receive a message from the server.
	virtual bool onMessageReceived(mg_connection *client, const JsonReader &jsonDoc, JsonMessage &answer) = 0;
	void setMessageHandler(std::function<void(std::string, std::string)> handler);

	// The server calls this method every second.
	virtual void processByServer() = 0;
	virtual void onDisconnect(const mg_connection *c) = 0;

protected:
	// Send a message to the server.
	void sendMessage(std::string type, std::string content);

private:
	std::function<void(std::string, std::string)> m_messageEmitter = 0; // Send a message to the server.
};