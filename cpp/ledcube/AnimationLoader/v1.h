#pragma once

#include "abstract.h"

namespace AnimationLoaders {

	// Version 1 : 2021-2022
	// A text file with the following format:
	//		<frame count>
	//		<duration>
	//		<frame 1>
	//  		[0-256]*192 (color for each LED)
	//		...
	class AnimationLoader_v1 : public AnimationLoader_abstract
	{
	public:
		bool isVersion(const std::string &data) override;

		Animation *loadAnimation(std::istream &stream) override;
		Animation *loadAnimation(const std::string &data) override;
		Animation *loadAnimationFromFile(const std::string &path) override;

		bool saveAnimation(const Animation *animation, std::ostream &os) override;
		std::string saveAnimation(const Animation *animation) override;
		bool saveAnimation(const Animation *animation, std::string path) override;
	};

} // namespace AnimationLoaders