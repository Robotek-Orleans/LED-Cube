#pragma once
#include "LEDCube.h"
#include "../server/ServerFriend.h"

class LEDCubeManager : public ServerFriend
{
public:
	LEDCubeManager();
	~LEDCubeManager();
	void start() { cube->start(); };
	void join() { cube->join(); };
	void stop() { cube->stop(); };
	// Receive a message from the server.
	bool onMessageReceived(mg_connection *client, const JsonReader &jsonDoc, JsonMessage &answer) override;

	void processByServer() override;
	void onDisconnect(const mg_connection *c) override;

private:
	LEDCube *cube = nullptr;
	std::vector<mg_connection *> clientsSubscribedToChanges;

	// Random animations
	bool animationChanged = false;
	bool animationEnded = false;
	long long nextRandomAnimationMinTime = 0;

	bool playRandomAnimation(std::string filter);
	bool playNextRandomAnimation();

	void fillWithCurrentAnimationInfos(JsonMessage &answer) const;
	void sendLEDCubeEvent(const rapidjson::StringBuffer &buffer);
};