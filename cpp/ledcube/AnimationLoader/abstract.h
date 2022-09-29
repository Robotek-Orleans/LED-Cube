#pragma once

#include "../Animation.h"
#include <RapidJson/document.h>
#include <regex>

namespace AnimationLoaders {

	// Abstract class
	class AnimationLoader_abstract
	{
	public:
		virtual bool isVersion(const std::string &data) = 0;

		virtual Animation *loadAnimation(std::istream &stream) = 0;
		virtual Animation *loadAnimation(const std::string &data) = 0;
		virtual Animation *loadAnimationFromFile(const std::string &path) = 0;

		virtual bool saveAnimation(const Animation *animation, std::ostream &os) = 0;
		virtual std::string saveAnimation(const Animation *animation) = 0;
		virtual bool saveAnimation(const Animation *animation, std::string path) = 0;
	};

} // namespace AnimationLoaders