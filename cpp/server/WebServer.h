#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <numeric>
#include <unistd-compat.h>
#include <vector>

#include <RapidJson/document.h>
#include <RapidJson/writer.h>
#include <RapidJson/stringbuffer.h>

#include "ClientManager.h"
#include "../Thread.h"
#include "HardwareMonitor.h"
#include "ServerFriend.h"

class WebServer : public Thread
{
public:
	WebServer();
	~WebServer();

	void addFriend(ServerFriend *sFriend);
	void removeFriend(ServerFriend *sFriend);

protected:
	void run();

private:
	static void ev_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data);
	void connectionOpen(struct mg_connection *c);
	void connectionConfirmed(struct mg_connection *c);
	void connectionClosed(struct mg_connection *c);
	void handleHttpRequest(struct mg_connection *c, struct mg_http_message *hm);

	const std::string getAddr(struct mg_connection *nc) const;

	void msgReceived(struct mg_connection *c, mg_ws_message *p);
	void onMsgFromFriend(std::string type, std::string content);

	void sendMsg(struct mg_connection *nc, std::string type, std::string arg, bool toEveryone = false);
	void sendStatus(struct mg_connection *nc);

	void loadPassword();

	bool debug = false; // Can be changed with commands

	// Connections
	std::string password = "secretLedcube";
	int port = 80;
	std::string webFolder = "./web/";
	ClientManager *clientManager = nullptr;
	std::vector<ServerFriend *> friends;
	int clientsId = 0;
};
