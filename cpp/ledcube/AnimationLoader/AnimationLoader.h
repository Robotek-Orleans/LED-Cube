#pragma once

#include "../Animation.h"
#include "abstract.h"
#include "../../server/JsonReader.h"

namespace AnimationLoaders {
	static const int VERSION = 2;

	class AnimationLoader : public AnimationLoader_abstract
	{
	public:
		int getVersionOf(const std::string &data);
		bool isVersion(const std::string &data) override { return false; }

		Animation *loadAnimation(std::istream &stream) override { throw "Not Available"; }
		Animation *loadAnimation(const std::string &data) override;
		Animation *loadAnimationFromFile(const std::string &path) override;

		bool saveAnimation(const Animation *animation, std::ostream &os) override { throw "Not Available"; }
		std::string saveAnimation(const Animation *animation) override;
		bool saveAnimation(const Animation *animation, std::string path) override;

		Animation *jsonToAnimation(const JsonReader &document);
		Frame *jsonToFrame(const JsonReader &document);
		int **jsonToLayer(const JsonReader &document);

		void animationToJson(const Animation *animation, rapidjson::Document &document);
		void frameToJson(const Frame *frame, rapidjson::Document &document);
		void layerToJson(int **layer, rapidjson::Document &document);
	};

} // namespace AnimationLoaders