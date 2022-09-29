#include "LEDCubeManager.h"
#include <algorithm>

LEDCubeManager::LEDCubeManager()
	: ServerFriend() {
	cube = new LEDCube();
	cube->setRandomAnimChangeHandler([this]() { animationChanged = true; });
	cube->setRandomAnimEndHandler([this]() { animationEnded = true; });
	playRandomAnimation("");
}

LEDCubeManager::~LEDCubeManager() {
	delete cube;
	cube = nullptr;
}

bool LEDCubeManager::onMessageReceived(mg_connection *c, const JsonReader &jsonDoc, JsonMessage &answer) {
	std::string action = jsonDoc.getString("action", "");
	std::string subAction = action.substr(action.find('.') + 1);
	action = action.substr(0, action.find('.'));

	if (action == "setLed") {
		int x = jsonDoc.getInt("x", 0);
		int y = jsonDoc.getInt("y", 0);
		int z = jsonDoc.getInt("z", 0);
		int color = jsonDoc.getInt("color", 0);
		cube->setLed(x, y, z, color);

		// Send the event to every client subscribed to changes
		JsonMessage eventMsg("ledcubeEvents");
		eventMsg.Add("eventName", "ledChanged");
		eventMsg.Add("x", x);
		eventMsg.Add("y", y);
		eventMsg.Add("z", z);
		eventMsg.Add("color", color);
		sendLEDCubeEvent(eventMsg.stringify());
		answer.Add("success", true);
		return true;
	} else if (action == "animation") {
		// The fileName must contain the extension
		std::string fileName = jsonDoc.getString("fileName", "");
		std::string animation = jsonDoc.getString("animation", "");
		if (fileName != "") {
			// Verify that the fileName is valid (doesn't go outside the animations folder)
			if (fileName.find("..") != std::string::npos) {
				answer.Add("error", "Invalid fileName");
				return true;
			}
		}

		// ex: animation.play
		if (subAction == "play") {
			// Load the animation and play it on the ledcube
			bool loaded = cube->playAnimation(fileName);
			answer.Add("success", loaded);
			answer.Add("fileName", fileName);
			if (!loaded) {
				answer.Add("error", "Could not load animation");
			}
			return true;
		} else if (subAction == "playRandom") {
			std::string filter = jsonDoc.getString("filter", "");
			bool playing = playRandomAnimation(filter);
			answer.Add("success", playing);
			return true;
		} else if (subAction == "save") {
			// Save the animation (and play it eventually)
			answer.Add("fileName", fileName);
			bool overwrite = jsonDoc.getBool("overwrite", false);
			try {
				std::regex invalidCharRegex("[^a-zA-Z0-9À-ÿ_\\.\\-\\/ ]", std::regex_constants::ECMAScript | std::regex_constants::icase);
				fileName = std::regex_replace(fileName, invalidCharRegex, "_");
				bool fileExists = std::filesystem::exists(cube->ANIMATIONS_FOLDER + fileName);
				if (fileName.find("/") != std::string::npos) {
					std::string folder = fileName.substr(0, fileName.find_last_of("/"));
					std::filesystem::create_directories(cube->ANIMATIONS_FOLDER + folder);
				}

				if (fileExists && !overwrite) {
					answer.Add("error", "File already exists");
					return true;
				}

				std::ofstream file(cube->ANIMATIONS_FOLDER + fileName);
				if (!file) {
					std::cout << "Unable to open file \"" << fileName << "\" for writing" << std::endl;
					answer.Add("error", "Unable to open file \"" + fileName + "\" for writing");
					return true;
				}
				file << animation;
				file.close();
			} catch (std::exception e) {
				std::cout << "Error while saving animation: " << e.what() << std::endl;
				answer.Add("error", e.what());
				return true;
			}
			answer.Add("success", true);
			if (jsonDoc.getBool("play", false)) {
				bool played = cube->playAnimation(fileName);
				answer.Add("played", played);
			}
			return true;
		} else if (subAction == "play-no-save") {
			// Play the animation without saving it
			Animation *anim = AnimationLoaders::AnimationLoader().loadAnimation(animation);
			if (anim != nullptr) {
				cube->setAnimation(anim);
			} else {
				answer.Add("error", "Invalid animation");
			}
			answer.Add("success", anim != nullptr);
			return true;
		} else if (subAction == "get") {
			// Load the file
			std::ifstream file(cube->ANIMATIONS_FOLDER + fileName);
			std::string animation((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			answer.Add("animation", animation);
			file.close();
			answer.Add("success", true);
			answer.Add("fileName", fileName);
			return true;
		} else if (subAction == "get-current") {
			// Get the current animation
			const Animation *animation = cube->getCurrentAnimation();
			if (animation != nullptr) {
				std::string animationStr = AnimationLoaders::AnimationLoader().saveAnimation(animation);
				answer.Add("animation", animationStr);
				answer.Add("currentFrameIndex", cube->getCurrentFrameIndex());
				answer.Add("success", true);
			} else {
				answer.Add("success", false);
			}
			return true;
		} else if (subAction == "list") {
			// Get the list of animations
			std::vector<std::string> animations = cube->getAnimations();
			rapidjson::Value array(rapidjson::kArrayType);
			for (std::string animation : animations) {
				array.PushBack(rapidjson::Value(animation.c_str(), answer.GetAllocator()), answer.GetAllocator());
			}
			answer.GetDocument().AddMember("animations", array, answer.GetAllocator());
			answer.Add("success", true);
			return true;
		} else if (subAction == "stop") {
			// Stop the animation

			cube->setAnimation(nullptr);
			answer.Add("success", true);
			return true;
		} else if (subAction == "delete") {
			// Delete the animation
			answer.Add("fileName", fileName);
			if (fileName == "") {
				answer.Add("error", "Invalid fileName");
				return true;
			}
			// Verify that the fileName is valid (doesn't go outside the animations folder)
			if (fileName.find("..") != std::string::npos) {
				answer.Add("error", "Invalid fileName");
				return true;
			}
			// Remove the file
			std::string filePath = cube->ANIMATIONS_FOLDER + fileName;
			if (std::remove(filePath.c_str()) != 0) {
				answer.Add("error", "Error while deleting the file");
				return true;
			}
			answer.Add("success", true);
			return true;
		} else {
			return false;
		}
	} else if (action == "layer") {
		// Get/Set the layer
		std::string layer = jsonDoc.getString("layer", "");
		int layerIndex = jsonDoc.getInt("layerIndex", 0);
		std::string directionStr = jsonDoc.getString("direction", "");
		int direction = directionStr == "X" ? 0 : directionStr == "Y" ? 1
											  : directionStr == "Z"	  ? 2
																	  : 2;
		if (subAction == "get") {
			// Get the layer
			int **layer = cube->getLayer(layerIndex, direction);
			AnimationLoaders::AnimationLoader().layerToJson(layer, answer.GetDocument());
			for (int i = 0; i < YLENGTH; ++i) {
				delete[] layer[i];
			}
			delete[] layer;
			answer.Add("success", true);
			return true;
		} else if (subAction == "set") {
			// Set the layer
			if (!jsonDoc.HasMember("layer")) {
				answer.Add("success", false);
				return true;
			}

			int **layer = AnimationLoaders::AnimationLoader().jsonToLayer(jsonDoc);
			cube->setLayer(layer, layerIndex, direction);

			// Send the event to every client subscribed to changes
			JsonMessage eventMsg("ledcubeEvents");
			eventMsg.Add("eventName", "layerChanged");
			// create lines array
			rapidjson::Value lines(rapidjson::kArrayType);
			int y, x;
			for (y = 0; y < YLENGTH; ++y) {
				rapidjson::Value line(rapidjson::kArrayType);
				for (x = 0; x < XLENGTH; ++x) {
					line.PushBack(layer[y][x], eventMsg.GetAllocator());
				}
				lines.PushBack(line, eventMsg.GetAllocator());
			}
			eventMsg.GetDocument().AddMember("lines", lines, eventMsg.GetAllocator());
			eventMsg.Add("layerIndex", layerIndex);
			eventMsg.Add("direction", direction);
			sendLEDCubeEvent(eventMsg.stringify());

			for (int i = 0; i < YLENGTH; ++i) {
				delete[] layer[i];
			}
			delete[] layer;

			answer.Add("success", true);
			return true;
		} else {
			return false;
		}
	} else if (action == "set-animation-frames-count") {
		int framesCount = jsonDoc.getInt("count", 1);
		int fps = jsonDoc.getInt("fps", 100);
		cube->setAnimation(new Animation(framesCount, fps));
		answer.Add("success", true);
		return true;
	} else if (action == "status") {
		if (subAction == "subscribe") {
			bool subscribe = jsonDoc.getBool("subscribe", true);
			if (subscribe) {
				clientsSubscribedToChanges.push_back(c);
				fillWithCurrentAnimationInfos(answer);
			} else {
				clientsSubscribedToChanges.erase(std::remove(clientsSubscribedToChanges.begin(), clientsSubscribedToChanges.end(), c), clientsSubscribedToChanges.end());
			}
		} else {
			fillWithCurrentAnimationInfos(answer);
		}
		answer.Add("success", true);
		return true;
	} else {
		return false;
	}
}

void LEDCubeManager::processByServer() {
	if (animationChanged) {
		animationChanged = false;

		const Animation *anim = cube->getCurrentAnimation();
		JsonMessage msg("ledcubeEvents");
		msg.Add("eventName", "animationChanged");
		fillWithCurrentAnimationInfos(msg);
		sendLEDCubeEvent(msg.stringify());

	} else if (animationEnded) {
		animationEnded = false;
		long long now = getCurrentTimeMs();
		if (cube->isRandomAnimationRunning() && nextRandomAnimationMinTime < now) {
			playNextRandomAnimation();
		}
	}
}

void LEDCubeManager::onDisconnect(const mg_connection *c) {
	clientsSubscribedToChanges.erase(std::remove(clientsSubscribedToChanges.begin(), clientsSubscribedToChanges.end(), c), clientsSubscribedToChanges.end());
}

bool LEDCubeManager::playRandomAnimation(std::string filter) {
	bool playing = cube->playRandomAnimation(filter);
	nextRandomAnimationMinTime = getCurrentTimeMs() + cube->MIN_TIME_BETWEEN_RANDOM_ANIMATIONS;
	return playing;
}

bool LEDCubeManager::playNextRandomAnimation() {
	bool playing = cube->playNextRandomAnimation();
	nextRandomAnimationMinTime = getCurrentTimeMs() + cube->MIN_TIME_BETWEEN_RANDOM_ANIMATIONS;
	return playing;
}

void LEDCubeManager::fillWithCurrentAnimationInfos(JsonMessage &answer) const {
	CurrentAnimationInfo currentAnimationInfos = cube->getCurrentAnimationInfo();
	answer.Add("currentName", currentAnimationInfos.currentAnimationName);
	answer.Add("currentStartedAt", currentAnimationInfos.currentStartedAt);
	answer.Add("currentFrameIndex", cube->getCurrentFrameIndex());
	answer.Add("nextName", currentAnimationInfos.nextAnimationName);
}

void LEDCubeManager::sendLEDCubeEvent(const rapidjson::StringBuffer &buffer) {
	for (mg_connection *c : clientsSubscribedToChanges) {
		mg_ws_send(c, buffer.GetString(), buffer.GetSize(), WEBSOCKET_OP_TEXT);
	}
}