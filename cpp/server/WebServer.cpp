#include "WebServer.h"

static WebServer *wsInstance = nullptr;

WebServer::WebServer()
	: Thread() {
	wsInstance = this;
	loadPassword();
	clientManager = new ClientManager();
}

WebServer::~WebServer() {
	wsInstance = nullptr;
	friends.clear();
}

void WebServer::addFriend(ServerFriend *sFriend) {
	if (!sFriend) return;
	friends.push_back(sFriend);
	sFriend->setMessageHandler([this](std::string type, std::string content) { this->onMsgFromFriend(type, content); });
}

void WebServer::removeFriend(ServerFriend *sFriend) {
	if (!sFriend) return;
	sFriend->setMessageHandler(nullptr);
	friends.erase(std::remove(friends.begin(), friends.end(), sFriend), friends.end());
	friends.shrink_to_fit();
}

void WebServer::run() {
	std::cout << "[WebServer] Starting on port " << std::to_string(port) << std::endl;

	// Start Web Server
	struct mg_mgr mgr;
	mg_mgr_init(&mgr);																					   // Init manager
	mg_http_listen(&mgr, std::string("http://0.0.0.0:" + std::to_string(port)).c_str(), ev_handler, &mgr); // Setup listener

	// std::cout << LN->setStateCmd("WebServer") << std::endl;

	do {
		try {
			while (canRun()) {
				mg_mgr_poll(&mgr, 100); // Event loop
				for (auto &f : friends) {
					f->processByServer();
				}
			}
			break; // on a arrete correctement
		} catch (const std::exception &error) {
			std::cout << "[WebServer] Error: WebServer suddenly stopped. " << error.what() << std::endl
					  << "Restarting..." << std::endl;
		}
	} while (canRun());

	// Free up all memory allocated
	mg_mgr_free(&mgr);
}

void WebServer::ev_handler(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
	if (wsInstance == nullptr) return;
	switch (ev) {
	case MG_EV_WS_OPEN:
		wsInstance->connectionOpen(c); // Websocket handshake done
		break;

	case MG_EV_CLOSE:
		if (c->is_websocket) {
			wsInstance->connectionClosed(c); // Disconnect Websocket connection
		}
		break;

	case MG_EV_HTTP_MSG:
		wsInstance->handleHttpRequest(c, (struct mg_http_message *)ev_data); // HTTP request
		break;

	case MG_EV_WS_MSG:
		wsInstance->msgReceived(c, (mg_ws_message *)ev_data); // Websocket msg, text or bin
		break;
	}
}

void WebServer::connectionOpen(struct mg_connection *c) {
	if (debug) std::cout << "[WebServer] connection with " << getAddr(c) << " started." << std::endl;
	clientManager->onConnect(c);
}

void WebServer::connectionClosed(struct mg_connection *c) {
	if (debug) std::cout << "[WebServer] connection with " << getAddr(c) << " ended." << std::endl;
	clientManager->onDisconnect(c);
	for (ServerFriend *sFriend : friends) {
		sFriend->onDisconnect(c);
	}
}

void WebServer::handleHttpRequest(struct mg_connection *c, struct mg_http_message *hm) {
	if (mg_http_match_uri(hm, "/ws")) {
		// Upgrade to websocket. From now on, a connection is a full-duplex
		// Websocket connection, which will receive MG_EV_WS_MSG events.
		mg_ws_upgrade(c, hm, NULL);
	} else {
		struct mg_http_serve_opts opts = {}; // Serve local dir

		std::string uri(hm->uri.ptr, hm->uri.len);
		// every web files are in the /web folder
		// they are accessible with /web/... or with /...
		// i.e. /web/login or /login (or /web/login/index.html or /login/index.html)
		// by default we use /web because of source code organization (html: <script src="/web/..."></script>)
		// Every web files are available without authentication
		// But the client has to authenticate with a Web Socket to send and get ledcube informations
		if (uri.find("/web/") == 0) {
			opts.root_dir = ".";
		} else {
			opts.root_dir = webFolder.c_str();
			uri = "/web" + uri;
		}

		if (uri.find("/web/favicon.ico") == 0) return mg_http_serve_file(c, hm, std::string(webFolder + "favicon/favicon.ico").c_str(), &opts);

		mg_http_serve_dir(c, hm, &opts);
	}
}

const std::string WebServer::getAddr(struct mg_connection *c) const {
	char addr[32];
	mg_straddr(&c->rem, addr, sizeof(addr));
	return addr;
}

void WebServer::msgReceived(struct mg_connection *c, mg_ws_message *msg) {
	const std::string addr = getAddr(c);

	struct mg_str d = msg->data;

	try {
		JsonReader jsonReader(std::string(d.ptr, d.len));

		std::string type = jsonReader.getString("type", "");
		std::string date = jsonReader.getString("date", "");

		if (type == "") {
			sendMsg(c, "error", "You sent an invalid message!");
			return;
		}

		if (type == "auth") {
			if (debug) std::cout << "[WebServer] New request received from " << addr << " : authentification" << std::endl;

			if (clientManager->isAuth(c)) {
				sendMsg(c, "connection", "Authentification is already confirmed!");
				return;
			}

			if (jsonReader.getString("key", "") == "" || jsonReader.getString("challenge", "") == "") {
				sendMsg(c, "error", "You sent an invalid message!");
				if (debug) std::cout << "[WebServer] Error message sent by " << addr << " : Invalid message" << std::endl;
				return;
			}

			std::string key = jsonReader.getString("key", "");
			std::string clientChallenge = jsonReader.getString("challenge", "");
			std::string sid = jsonReader.getString("id", "-1");
			int id;
			try {
				if (sid == "")
					id = -1;
				else
					id = std::stoi(sid);
			} catch (std::exception e) {
				id = -1;
			}

			if (debug) {
				std::cout << std::string("---------------------------------------\n")
								 + "auth with       : " + addr + "\n"
								 + "secretKey       : " + key + "\n"
								 + "clientChallenge : " + clientChallenge + "\n"
								 + "id              : " + std::to_string(id) + "\n"
								 + "date            : " + date + "\n"
								 + "---------------------------------------\n";
			}

			clientManager->onAuth(c, addr, clientChallenge, key, password, id);
			return;
		}

		if (!clientManager->isAuth(c)) {
			sendMsg(c, "answer", "You are not authentified.");
			return; // invalid connection
		}

		std::string args = jsonReader.getString("args", "");

		if (debug && type != "status" && type != "ledcube") {
			std::cout << "[WebServer] New request received from " << addr << " : " << type << " at " << date << " (args: " << args << ")" << std::endl;
		}
		if (type == "status") {
			sendStatus(c);
		} else if (type == "command") {
			if (args == "help" || args == "?") {
				sendMsg(c, "answer",
						"help or ?: display this help\n"
						"chat [msg]: send a message to everyone\n"
						"debug (enable/disable): display/edit the current status of debuging c++ part\n"
						"stop: stop LEDCube and WebServer");
			} else if (args.rfind("chat ", 0) == 0) { // si il est en pos 0
				// LN->sendMsgToFriend(args);
				args = args.replace(0, 5, ""); // 5 (espace compris)
				sendMsg(c, "chat", args, true);
			} else if (args.rfind("debug", 0) == 0) {
				args.replace(0, 5, ""); // 5
				if (args.size() > 0) {
					if (args.rfind(" ", 0) == 0) args.replace(0, 1, "");
					debug = (args == "enabled" || args == "enable" || args == "true" || args == "on");
				}
				sendMsg(c, "answer", std::string("Current state for debug: ") + (debug ? "enabled" : "disabled"));
			} else if (args == "stop") {
				std::cout << "[WebServer] Stopped by " << addr << std::endl;
				sendMsg(c, "answer", "WebServer is stopping...", true);
				stop();
			}
		} else if (type == "ledcube") {
			// Send to friend
			JsonMessage answer("answer");
			answer.Add("timestamp", jsonReader.getString("timestamp", ""));
			answer.Add("success", false);
			bool sendAnswer = false;
			try {
				for (auto &f : friends) {
					if (f->onMessageReceived(c, jsonReader, answer)) {
						sendAnswer = true;
						break;
					}
				}
			} catch (std::exception e) {
				sendAnswer = true;
				answer.Add("error", e.what());
				std::cout << "[WebServer/ServerFriend] Error message sent by " << addr << " : " << e.what() << std::endl;
			}
			if (sendAnswer) {
				answer.send(c);
			}
		} else { // Invalid type
			sendMsg(c, "error", "You sent an invalid type!");
			if (debug) std::cout << "[WebServer] Error message sent by " << addr << " : Invalid type" << std::endl;
		}
	} catch (std::exception e) {
		std::cout << "[WebServer] Error message sent by " << addr << " : " << e.what() << std::endl;
	}
}

void WebServer::onMsgFromFriend(std::string type, std::string content) {
	sendMsg(nullptr, type, content, true);
}

void WebServer::sendMsg(struct mg_connection *c, std::string type, std::string arg, bool toEveryone) {
	JsonMessage msg(type);
	msg.Add("content", arg);
	rapidjson::StringBuffer buffer = msg.stringify();

	if (toEveryone) {
		clientManager->send(buffer);
		if (c && !clientManager->isAuth(c)) // pas client = pas encore envoyé
			mg_ws_send(c, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
	} else
		mg_ws_send(c, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
}

void WebServer::sendStatus(mg_connection *c) {
	JsonMessage msg("sysStatus");
	msg.Add("temp", temperature());
	msg.Add("cpuUsage", cpu_usage());
	msg.Add("ramUsage", mem_usage());
	msg.Add("bootTime", boot_time());
	msg.send(c);
	if (debug) std::cout << "[WebServer] SysStatus message sent to " << getAddr(c) << " !" << std::endl;
}

void WebServer::loadPassword() {
	std::ifstream file("./ledcube.env");
	bool portLoaded = false;
	bool passwordLoaded = false;
	bool webFolderLoaded = false;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line.back() == '\r') line.pop_back();
			if (line.rfind("PORT=", 0) == 0) {
				try {
					port = std::stoi(line.replace(0, 5, ""));
					portLoaded = true;
				} catch (std::exception e) {
					std::cout << "[WebServer] Error while loading port: " << e.what() << std::endl;
				}
			} else if (line.rfind("PASSWORD=", 0) == 0) {
				password = line.replace(0, 9, "");
				passwordLoaded = true;
			} else if (line.rfind("WEB_FOLDER=", 0) == 0) {
				webFolder = line.replace(0, 11, "");
				webFolderLoaded = true;
			} else if (line.rfind("DEBUG=", 0) == 0) {
				debug = (line.replace(0, 6, "") == "true");
			}
		}
		file.close();
		std::cout << "[WebServer] Config loaded from ./ledcube.env." << std::endl;
	} else {
		std::cout << "[WebServer] Error: Could not open config file (./ledcube.env)." << std::endl;
	}

	if (portLoaded) {
		if (port == 0) {
			std::cout << "    Error: Invalid port (0). Set to 80." << std::endl;
			port = 80;
		}
		std::cout << "    Port loaded" << std::endl;
	} else {
		port = 80;
		std::cout << "    Error: Could not load port. Default port is '" << port << "'." << std::endl;
	}
	if (passwordLoaded) {
		std::cout << "    Password loaded" << std::endl;
	} else {
		password = "secretLedcube";
		std::cout << "    Error: Could not load password. Default password is '" << password << "'." << std::endl;
	}
	if (webFolderLoaded) {
		std::cout << "    Web folder loaded" << std::endl;
	} else {
		webFolder = "./WEB/";
		std::cout << "    Error: Could not load web folder. Default web folder is '" << webFolder << "'." << std::endl;
	}
	if (webFolder.back() != '/') webFolder += '/';
}
