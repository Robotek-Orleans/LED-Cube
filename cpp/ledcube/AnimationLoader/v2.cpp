#include "v2.h"
#include <sstream>
#include "../../include/zip/json-zip.h"

bool AnimationLoaders::AnimationLoader_v2::isVersion(const std::string &data) {
	std::regex versionRegex("^2\\s+[a-zA-Z0-9\\+/=]+$", std::regex_constants::ECMAScript | std::regex_constants::icase);

	std::string dataCopy = data.length() > 26000 ? data.substr(0, 26000) : data;
	return std::regex_match(dataCopy, versionRegex);
}

Animation *AnimationLoaders::AnimationLoader_v2::loadAnimation(std::istream &stream) {
	std::string version;
	std::string data;
	std::getline(stream, version, ' ');
	std::getline(stream, data);
	_ASSERT_EXPR(isVersion(version + " " + data), "Invalid animation data");

	// decompress data
	JsonReader document;
	jsonzip::decodeJson(data, document);
	return jsonToAnimation(document);
}

Animation *AnimationLoaders::AnimationLoader_v2::loadAnimation(const std::string &data) {
	std::stringstream ss(data);
	return loadAnimation(ss);
}

Animation *AnimationLoaders::AnimationLoader_v2::loadAnimationFromFile(const std::string &path) {
	std::ifstream flux(path, std::ios::in);
	if (!flux) {
		std::cout << "Erreur lors de la lecture du fichier \"" << path << "\"" << std::endl;
		return nullptr;
	}
	Animation *animation = loadAnimation(flux);
	if (animation != nullptr) {
		std::string name = path.substr(path.find_last_of("\\/") + 1);
		animation->setName(name);
	}
	flux.close();
	return animation;
}

bool AnimationLoaders::AnimationLoader_v2::saveAnimation(const Animation *animation, std::ostream &os) {
	JsonReader document;
	animationToJson(animation, document);
	os << "2 ";
	os << jsonzip::encodeJson(document);
	return true;
}

std::string AnimationLoaders::AnimationLoader_v2::saveAnimation(const Animation *animation) {
	std::ostringstream flux;
	saveAnimation(animation, flux);
	return flux.str();
}

bool AnimationLoaders::AnimationLoader_v2::saveAnimation(const Animation *animation, std::string path) {
	std::ofstream flux(path, std::ios::out | std::ios::trunc);
	if (!flux) {
		std::cout << "Unable to open file \"" << path << "\"!" << std::endl;
		return false;
	}
	saveAnimation(animation, flux);
	flux.close();
	return true;
}

Animation *AnimationLoaders::AnimationLoader_v2::jsonToAnimation(const JsonReader &document) {
	// get version
	if (!document.HasMember("version") || !document["version"].IsInt()) {
		std::cerr << "Invalid animation data (no version)" << std::endl;
		return nullptr;
	}
	int version = document["version"].GetInt();
	_ASSERT_EXPR(version == 2, "AnimationLoader_v2::loadAnimation: version mismatch");

	// get content type
	if (!document.HasMember("contentType") || !document["contentType"].IsString()) {
		std::cerr << "Invalid animation data (no contentType)" << std::endl;
		return nullptr;
	}
	std::string contentType = document["contentType"].GetString();
	_ASSERT_EXPR(contentType == "animation", "AnimationLoader_v2::loadAnimation: content type mismatch");

	// get frame count
	if (!document.HasMember("frameCount") || !document["frameCount"].IsInt()) {
		std::cerr << "Invalid animation data (no frame count)" << std::endl;
		return nullptr;
	}
	int frameCount = document["frameCount"].GetInt();

	// get frame duration
	if (!document.HasMember("frameDuration") || !document["frameDuration"].IsInt()) {
		std::cerr << "Invalid animation data (no frame duration)" << std::endl;
		return nullptr;
	}
	int frameDuration = document.getInt("frameDuration", 50);

	// get name
	std::string name = document.getString("name", "");

	// get frame data
	if (!document.HasMember("frames") || !document["frames"].IsArray()) {
		std::cerr << "Invalid animation data (no frames)" << std::endl;
		return nullptr;
	}
	const rapidjson::Value &frames = document["frames"];

	// create animation
	Animation *animation = new Animation(frameCount, frameDuration);
	animation->setName(name);

	// iterate through frames
	for (int t = 0; t < frameCount; ++t) {
		// get frame
		Frame *frame = jsonArrayToFrame(frames[t]);
		animation->setFrame(t, *frame);
		delete frame;
	}

	return animation;
}

Frame *AnimationLoaders::AnimationLoader_v2::jsonToFrame(const JsonReader &document) {
	// get version
	int version = document.HasMember("version") && document["version"].IsInt() ? document["version"].GetInt() : 0;
	_ASSERT_EXPR(version == 2, "AnimationLoader_v2::loadFrame: version mismatch");

	// get content type
	std::string contentType = document.HasMember("contentType") && document["contentType"].IsString() ? document["contentType"].GetString() : "";
	_ASSERT_EXPR(contentType == "frame", "AnimationLoader_v2::loadFrame: content type mismatch");

	// get frame data
	if (!document.HasMember("layers") || !document["layers"].IsArray()) {
		std::cerr << "Invalid frame data (no layers)" << std::endl;
		return nullptr;
	}
	const rapidjson::Value &layers = document["layers"];

	return jsonArrayToFrame(layers);
}

int **AnimationLoaders::AnimationLoader_v2::jsonToLayer(const JsonReader &document) {
	// get version
	int version = document.HasMember("version") && document["version"].IsInt() ? document["version"].GetInt() : 0;
	_ASSERT_EXPR(version == 2, "AnimationLoader_v2::loadLayer: version mismatch");

	// get content type
	std::string contentType = document.HasMember("contentType") && document["contentType"].IsString() ? document["contentType"].GetString() : "";
	_ASSERT_EXPR(contentType == "layer", "AnimationLoader_v2::loadLayer: content type mismatch");

	// get layer data
	if (!document.HasMember("lines") || !document["lines"].IsArray()) {
		std::cerr << "Invalid layer data (no data)" << std::endl;
		return nullptr;
	}
	const rapidjson::Value &lines = document["lines"];

	// create transient data
	int **layer = new int *[YLENGTH];

	// iterate through lines
	int y, x;
	for (y = 0; y < YLENGTH; ++y) {
		layer[y] = new int[XLENGTH];
		for (x = 0; x < XLENGTH; ++x) {
			layer[y][x] = lines[y][x].GetInt();
		}
	}

	return layer;
}

void AnimationLoaders::AnimationLoader_v2::animationToJson(const Animation *animation, rapidjson::Document &document) {
	document.SetObject();

	// set version, frame count, and frame duration, contentType
	document.AddMember("version", 2, document.GetAllocator());
	document.AddMember("frameCount", animation->getFrameCount(), document.GetAllocator());
	document.AddMember("frameDuration", (int)(animation->getFrameDuration()), document.GetAllocator());
	document.AddMember("contentType", "animation", document.GetAllocator());
	document.AddMember("name", rapidjson::Value(animation->getName().c_str(), animation->getName().length()), document.GetAllocator());

	// create frames array
	rapidjson::Value frames(rapidjson::kArrayType);
	rapidjson::Value layers(rapidjson::kArrayType);
	for (int t = 0; t < animation->getFrameCount(); ++t) {
		// create frame
		frameToJsonArray(animation->getFrame(t), layers, document.GetAllocator());
		frames.PushBack(layers, document.GetAllocator());
	}
	document.AddMember("frames", frames, document.GetAllocator());
}

void AnimationLoaders::AnimationLoader_v2::frameToJson(const Frame *frame, rapidjson::Document &document) {
	document.SetObject();

	// set version, contentType
	document.AddMember("version", 2, document.GetAllocator());
	document.AddMember("contentType", "frame", document.GetAllocator());

	// create layers array
	rapidjson::Value layers(rapidjson::kArrayType);
	frameToJsonArray(frame, layers, document.GetAllocator());
	document.AddMember("layers", layers, document.GetAllocator());
}

void AnimationLoaders::AnimationLoader_v2::layerToJson(int **layer, rapidjson::Document &document) {
	document.SetObject();

	// set version, contentType
	document.AddMember("version", 2, document.GetAllocator());
	document.AddMember("contentType", "layer", document.GetAllocator());

	// create lines array
	rapidjson::Value lines(rapidjson::kArrayType);
	int y, x;
	for (y = 0; y < YLENGTH; ++y) {
		rapidjson::Value line(rapidjson::kArrayType);
		for (x = 0; x < XLENGTH; ++x) {
			line.PushBack(layer[y][x], document.GetAllocator());
		}
		lines.PushBack(line, document.GetAllocator());
	}
	document.AddMember("lines", lines, document.GetAllocator());
}

Frame *AnimationLoaders::AnimationLoader_v2::jsonArrayToFrame(const rapidjson::Value &layers) {
	int z, y, x, c;
	uint8_t ****transientData = new uint8_t ***[ZLENGTH];

	// iterate through layers
	for (z = 0; z < ZLENGTH; ++z) {
		transientData[z] = new uint8_t **[YLENGTH];
		for (y = 0; y < YLENGTH; ++y) {
			transientData[z][y] = new uint8_t *[XLENGTH];
			for (x = 0; x < XLENGTH; ++x) {
				c = layers[z][y][x].GetInt();
				transientData[z][y][x] = new uint8_t[3];
				transientData[z][y][x][0] = (c >> 16) & 0xFF;
				transientData[z][y][x][1] = (c >> 8) & 0xFF;
				transientData[z][y][x][2] = c & 0xFF;
			}
		}
	}

	Frame *frame = new Frame(transientData);
	deleteTransientData(transientData);
	return frame;
}

void AnimationLoaders::AnimationLoader_v2::frameToJsonArray(const Frame *frame, rapidjson::Value &layers, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) {
	layers.SetArray();
	int z, y, x, c;
	for (z = 0; z < ZLENGTH; ++z) {
		layers.PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
		for (y = 0; y < YLENGTH; ++y) {
			layers[z].PushBack(rapidjson::Value(rapidjson::kArrayType), allocator);
			for (x = 0; x < XLENGTH; ++x) {
				c = frame->getLed(x, y, z);
				layers[z][y].PushBack(c, allocator);
			}
		}
	}
}

void AnimationLoaders::AnimationLoader_v2::deleteTransientData(uint8_t ****transientData) {
	int z, y, x;
	for (z = 0; z < ZLENGTH; ++z) {
		for (y = 0; y < YLENGTH; ++y) {
			for (x = 0; x < XLENGTH; ++x) {
				delete[] transientData[z][y][x];
			}
			delete[] transientData[z][y];
		}
		delete[] transientData[z];
	}
	delete[] transientData;
}
