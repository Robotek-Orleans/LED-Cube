#include "v1.h"
#include <sstream>

bool AnimationLoaders::AnimationLoader_v1::isVersion(const std::string &data) {

	std::regex versionRegex("^\\d+\\s+\\d+[\\d\\s]+$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	return std::regex_match(data, versionRegex);
}

Animation *AnimationLoaders::AnimationLoader_v1::loadAnimation(std::istream &stream) {
	std::string line;
	std::getline(stream, line);
	int frameCount = std::stoi(line);
	std::getline(stream, line);
	int frameDuration = std::stoi(line) * MS_CLOCK;
	Animation *animation = new Animation(frameCount, frameDuration);

	uint8_t ****transientData = new uint8_t ***[ZLENGTH];

	for (int z = 0; z < ZLENGTH; z++) {
		transientData[z] = new uint8_t **[YLENGTH];
		for (int y = 0; y < YLENGTH; y++) {
			transientData[z][y] = new uint8_t *[XLENGTH];
			for (int x = 0; x < XLENGTH; x++) {
				transientData[z][y][x] = new uint8_t[3];
				memset(transientData[z][y][x], 0, 3);
			}
		}
	}

	for (int t = 0; t < frameCount; t++) {
		for (int z = 0; z < ZLENGTH; z++) {
			for (int y = 0; y < YLENGTH; y++) {
				for (int x = 0; x < XLENGTH; x++) {
					for (int c = 0; c < 3; c++) {
						std::getline(stream, line);
						transientData[z][y][x][c] = std::stoi(line);
					}
				}
			}
		}
		animation->setFrame(t, transientData);
	}

	for (int z = 0; z < ZLENGTH; z++) {
		for (int y = 0; y < YLENGTH; y++) {
			for (int x = 0; x < XLENGTH; x++) {
				delete[] transientData[z][y][x];
			}
			delete[] transientData[z][y];
		}
		delete[] transientData[z];
	}
	delete[] transientData;

	return animation;
}

Animation *AnimationLoaders::AnimationLoader_v1::loadAnimation(const std::string &data) {
	std::istringstream flux(data);
	if (!flux) {
		std::cout << "Erreur lors de la lecture des données" << std::endl;
		return nullptr;
	}
	return loadAnimation(flux);
}

Animation *AnimationLoaders::AnimationLoader_v1::loadAnimationFromFile(const std::string &path) {
	std::ifstream flux(path, std::ios::in);
	if (!flux) {
		std::cout << "Erreur lors de la lecture du fichier \"" << path << "\"" << std::endl;
		return nullptr;
	}
	Animation *animation = loadAnimation(flux);
	std::string name = path.substr(path.find_last_of("\\/") + 1);
	animation->setName(name);
	flux.close();
	return animation;
}

bool AnimationLoaders::AnimationLoader_v1::saveAnimation(const Animation *animation, std::ostream &os) {
	os << animation->getFrameCount() << std::endl;
	os << animation->getFrameDuration() / MS_CLOCK << std::endl;
	int t, z, y, x, c;
	for (t = 0; t < animation->getFrameCount(); t++) {
		for (z = 0; z < ZLENGTH; z++) {
			for (y = 0; y < YLENGTH; y++) {
				for (x = 0; x < XLENGTH; x++) {
					c = animation->getFrame(t)->getLed(x, y, z);
					os << (int)(c >> 16) << std::endl
					   << (int)((c >> 8) & 0xFF) << std::endl
					   << (int)(c & 0xFF) << std::endl;
				}
			}
		}
	}
	return true;
}

std::string AnimationLoaders::AnimationLoader_v1::saveAnimation(const Animation *animation) {
	std::ostringstream flux;
	saveAnimation(animation, flux);
	return flux.str();
}

bool AnimationLoaders::AnimationLoader_v1::saveAnimation(const Animation *animation, std::string path) {
	std::string fileName = path.substr(path.find_last_of('.') + 1);
	std::regex unvalidCharRegex("[^a-zA-Z0-9_]", std::regex_constants::ECMAScript | std::regex_constants::icase);
	fileName = std::regex_replace(fileName, unvalidCharRegex, "_");
	path = path.substr(0, path.find_last_of('.')) + fileName;

	std::ofstream flux(path, std::ios::out | std::ios::trunc);
	if (!flux) {
		std::cout << "Unable to open file \"" << path << "\"!" << std::endl;
		return false;
	}
	saveAnimation(animation, flux);
	flux.close();
	return true;
}