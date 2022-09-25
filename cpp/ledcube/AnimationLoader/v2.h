#pragma once

#include "abstract.h"
#include "../../server/JsonReader.h"

namespace AnimationLoaders {

	// Version 2 : 2022
	// A text file with the following format:
	// A json compressed with zlib and converted to base64
	// {
	//		"version": <version>,
	//		"frameCount": <frame count>,
	//		"frameDuration": <duration>,
	//		"frames": [
	//			<frame 1>,
	//			...
	//		]
	// }
	class AnimationLoader_v2 : public AnimationLoader_abstract
	{
	public:
		bool isVersion(const std::string &data) override;

		Animation *loadAnimation(std::istream &stream) override;
		Animation *loadAnimation(const std::string &data) override;
		Animation *loadAnimationFromFile(const std::string &path) override;

		bool saveAnimation(const Animation *animation, std::ostream &os) override;
		std::string saveAnimation(const Animation *animation) override;
		bool saveAnimation(const Animation *animation, std::string path) override;

		Animation *jsonToAnimation(const JsonReader &document);
		Frame *jsonToFrame(const JsonReader &document);
		int **jsonToLayer(const JsonReader &document);

		void animationToJson(const Animation *animation, rapidjson::Document &document);
		void frameToJson(const Frame *frame, rapidjson::Document &document);
		void layerToJson(int **layer, rapidjson::Document &document);

	private:
		Frame *jsonArrayToFrame(const rapidjson::Value &layers);
		void frameToJsonArray(const Frame *frame, rapidjson::Value &layers, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator);

		void deleteTransientData(uint8_t ****transientData);
	};

} // namespace AnimationLoaders