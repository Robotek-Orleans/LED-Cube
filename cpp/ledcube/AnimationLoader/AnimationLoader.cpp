#include "AnimationLoader.h"
#include "v1.h"
#include "v2.h"

// Default animation saver
// The animation loader is chosen based on the data of the animation
#define AnimationSaver_Default AnimationLoaders::AnimationLoader_v2
#define AnimationSaver_Json AnimationLoaders::AnimationLoader_v2

int AnimationLoaders::AnimationLoader::getVersionOf(const std::string &data) {
	if (AnimationLoader_v1().isVersion(data)) {
		return 1;
	} else if (AnimationLoader_v2().isVersion(data)) {
		return 2;
	} else {
		throw std::invalid_argument("Unknown version");
		return -1;
	}
}

Animation *AnimationLoaders::AnimationLoader::loadAnimation(const std::string &data) {
	int version = getVersionOf(data);
	switch (version) {
	case 1:
		return AnimationLoader_v1().loadAnimation(data);
	case 2:
		return AnimationLoader_v2().loadAnimation(data);
	default:
		throw std::invalid_argument("Unknown version");
		return nullptr;
	}
}

bool stringEndsWith(const std::string &str, const std::string &suffix) {
	return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

Animation *AnimationLoaders::AnimationLoader::loadAnimationFromFile(const std::string &path) {
	int lastSlash = std::max((int)path.find_last_of("/"), (int)path.find_last_of("\\"));
	std::string fileName = path.substr(lastSlash + 1);
	Animation *anim = nullptr;
	int isVersion[2] = {0, 0};
	if (stringEndsWith(fileName, ".json.txt")) {
		// Special extension => try v2 first
		try {
			anim = AnimationLoader_v2().loadAnimationFromFile(path);
			if (anim) return anim;
			isVersion[1] = -2;
		} catch (std::exception e) {
			isVersion[1] = -1; // Not v2
		}
	}
	if (fileName.find_last_of(".") == -1) {
		// No extension => try v1
		try {
			anim = AnimationLoader_v1().loadAnimationFromFile(path);
			if (anim) return anim;
			isVersion[0] = -2;
		} catch (std::exception e) {
		}
		isVersion[0] = -1; // Not v1
	}
	try {
		if (isVersion[1] == 0) { // v2 not tried
			anim = AnimationLoader_v2().loadAnimationFromFile(path);
			if (anim) return anim;
			isVersion[0] = -2;
		}
	} catch (std::exception e) {
		isVersion[1] = -1; // Not v2
	}
	std::cerr << "Unknown animation file version" << std::endl;
	return nullptr;
}

std::string AnimationLoaders::AnimationLoader::saveAnimation(const Animation *animation) {
	return AnimationSaver_Default().saveAnimation(animation);
}

bool AnimationLoaders::AnimationLoader::saveAnimation(const Animation *animation, std::string path) {
	return AnimationSaver_Default().saveAnimation(animation, path);
}

Animation *AnimationLoaders::AnimationLoader::jsonToAnimation(const JsonReader &document) {
	return AnimationSaver_Json().jsonToAnimation(document);
}
Frame *AnimationLoaders::AnimationLoader::jsonToFrame(const JsonReader &document) {
	return AnimationSaver_Json().jsonToFrame(document);
}
int **AnimationLoaders::AnimationLoader::jsonToLayer(const JsonReader &document) {
	return AnimationSaver_Json().jsonToLayer(document);
}

void AnimationLoaders::AnimationLoader::animationToJson(const Animation *animation, rapidjson::Document &document) {
	AnimationSaver_Json().animationToJson(animation, document);
}
void AnimationLoaders::AnimationLoader::frameToJson(const Frame *frame, rapidjson::Document &document) {
	AnimationSaver_Json().frameToJson(frame, document);
}
void AnimationLoaders::AnimationLoader::layerToJson(int **layer, rapidjson::Document &document) {
	AnimationSaver_Json().layerToJson(layer, document);
}