#include "LEDCube.h"

LEDCube::LEDCube()
	: Thread(), animation(new Animation(1, 100)) {
	sender = new Sender();
	sender->start();
	animation_change_counter = 0;
	animation_update_counter = 0;
	frame_index = new int(0);
	srand(time(NULL));
	loadConfig();
}

LEDCube::~LEDCube() {
	if (sender != nullptr) {
		delete sender;
		sender = nullptr;
	}
	if (animation != nullptr) {
		delete animation;
		animation = nullptr;
	}
	if (frame_index != nullptr) {
		delete frame_index;
		frame_index = nullptr;
	}
}

Animation *LEDCube::loadAnimation(std::string fileName) const {
	// Verify that the fileName is valid (doesn't go outside the animations folder)
	if (fileName == "" || fileName.find("..") != std::string::npos) return nullptr;
	Animation *anim = AnimationLoaders::AnimationLoader().loadAnimationFromFile(ANIMATIONS_FOLDER + fileName);
	return anim;
}

/**
 * @brief Play the animation on the LEDCube
 *
 * @param fileName Just the name of the animation (with the extension)
 * @return true if the animation was loaded successfully
 * @return false if the animation doesn't exist or if the fileName isn't allowed
 */
bool LEDCube::playAnimation(std::string fileName) {
	Animation *anim = loadAnimation(fileName);
	if (!anim) return false;
	setAnimation(anim);
	return true;
}

void LEDCube::setAnimation(Animation *animation) {
	Animation *oldAnimation = this->animation;
	this->animation = animation;
	animation_change_counter++;
	next_random_animation_name = "";
	random_animation_running = false;
	if (!isRunning()) delete oldAnimation;
	// else the previous animation will be deleted when the next frame is set
}

Animation *LEDCube::getCurrentAnimation() {
	return animation;
}

CurrentAnimationInfo LEDCube::getCurrentAnimationInfo() const {
	CurrentAnimationInfo info;
	info.isRandom = random_animation_running;
	info.currentAnimationName = animation ? animation->getName() : "";
	info.currentStartedAt = animation_start_time;
	info.nextAnimationName = next_random_animation_name;
	return info;
}

std::vector<std::string> LEDCube::getAnimations() const {
	// Get animations in the animations folder and subfolders
	std::vector<std::string> animations;
	for (const auto &file : std::filesystem::recursive_directory_iterator(ANIMATIONS_FOLDER)) {
		if (file.is_directory()) continue;
		std::string fileName = file.path().string().substr(ANIMATIONS_FOLDER.length());
		std::replace(fileName.begin(), fileName.end(), '\\', '/');
#ifdef _WIN32
		// Remove every animation with an invalid character for Windows
		if (fileName.find_first_of("\\/:*?\"<>|") != std::string::npos) continue;
		bool invalid = false;
		for (int i = 0; i < fileName.length(); i++) {
			if (fileName[i] == -61) {
				i++;
				continue; // Allowed
			}
			if (fileName[i] < 0) {
				invalid = true;
				break;
			}
			if (fileName[i] < 32 || fileName[i] > 126) continue;
		}
		if (invalid) continue;
#endif
		animations.push_back(fileName);
	}
	return animations;
}

std::string LEDCube::getRandomAnimationName(std::string filter) const {
	std::vector<std::string> files = getAnimations();
	std::vector<std::string> filteredFiles;
	if (filter != "") {
		std::regex filterRegex(filter, std::regex_constants::ECMAScript | std::regex_constants::icase);
		for (std::string file : files) {
			if (std::regex_match(file, filterRegex)) {
				filteredFiles.push_back(file);
			}
		}
	} else {
		filteredFiles = files;
	}
	if (filteredFiles.size() == 0) return "";
	std::string fileName = filteredFiles[rand() % filteredFiles.size()];
	return fileName;
}

bool LEDCube::playRandomAnimation(std::string filter) {
	std::string fileName = getRandomAnimationName(filter);
	if (fileName == "") return false;
	bool playing = playAnimation(fileName);

	next_random_animation_name = getRandomAnimationName(filter);
	random_animation_running = true;
	return playing;
}

bool LEDCube::playNextRandomAnimation() {
	if (next_random_animation_name != "" && playAnimation(next_random_animation_name)) {
		next_random_animation_name = getRandomAnimationName(random_animations_filter);
		random_animation_running = true;
		return true;
	}
	return playRandomAnimation(random_animations_filter);
}

const Frame *LEDCube::getCurrentFrame() const {
	if (!animation) return nullptr;
	return animation->getFrame(*frame_index);
}

void LEDCube::setLed(int x, int y, int z, int color) {
	if (!animation) setAnimation(new Animation(1, 50));
	int r = (color >> 16) & 0xFF;
	int g = (color >> 8) & 0xFF;
	int b = color & 0xFF;
	animation->setLed(x, y, z, 0, r, g, b);
	animation_change_counter++;
}

int LEDCube::getLed(int x, int y, int z) const {
	if (!animation) return 0;
	return getCurrentFrame()->getLed(x, y, z);
}

void LEDCube::setLayer(int **colors, int layerIndex, int direction) {
	if (!animation) setAnimation(new Animation(1, 50));

	int i, j;
	switch (direction) {
	case 0:
		for (i = 0; i < YLENGTH; ++i) {
			for (j = 0; j < ZLENGTH; ++j) {
				setLed(layerIndex, i, j, colors[i][j]);
			}
		}
		break;
	case 1:
		for (i = 0; i < XLENGTH; ++i) {
			for (j = 0; j < ZLENGTH; ++j) {
				setLed(i, layerIndex, j, colors[i][j]);
			}
		}
		break;
	case 2:
		for (i = 0; i < YLENGTH; ++i) {
			for (j = 0; j < XLENGTH; ++j) {
				setLed(j, i, layerIndex, colors[i][j]);
			}
		}
		break;
	}
}

int **LEDCube::getLayer(int layerIndex, int direction) const {
	int **layer = new int *[YLENGTH];
	int i, j;
	switch (direction) {
	case 0:
		for (i = 0; i < YLENGTH; i++) {
			layer[i] = new int[ZLENGTH];
			for (j = 0; j < ZLENGTH; j++) {
				layer[i][j] = getLed(layerIndex, i, j);
			}
		}
		break;
	case 1:
		for (i = 0; i < XLENGTH; i++) {
			layer[i] = new int[ZLENGTH];
			for (j = 0; j < ZLENGTH; j++) {
				layer[i][j] = getLed(i, layerIndex, j);
			}
		}
		break;
	case 2:
		for (i = 0; i < YLENGTH; ++i) {
			layer[i] = new int[XLENGTH];
			for (j = 0; j < XLENGTH; ++j) {
				layer[i][j] = getLed(j, i, layerIndex);
			}
		}
		break;
	}
	return layer;
}

void LEDCube::setRandomAnimChangeHandler(std::function<void()> handler) {
	animation_change_handler = handler;
}

void LEDCube::setRandomAnimEndHandler(std::function<void()> handler) {
	animation_end_handler = handler;
}

void LEDCube::loadConfig() {
	std::ifstream file("./ledcube.env");
	bool animationsFolderLoaded = false;
	bool minTimeLoaded = false;
	if (file.is_open()) {
		std::string line;
		while (std::getline(file, line)) {
			if (line.back() == '\r') line.pop_back();
			if (line.rfind("ANIMATION_FOLDER=", 0) == 0) {
				ANIMATIONS_FOLDER = line.replace(0, 17, "");
				animationsFolderLoaded = true;
			} else if (line.rfind("MIN_TIME_BETWEEN_RANDOM_ANIMATIONS=", 0) == 0) {
				try {
					MIN_TIME_BETWEEN_RANDOM_ANIMATIONS = std::stoi(line.replace(0, 35, ""));
					minTimeLoaded = true;
				} catch (std::invalid_argument &e) {
					std::cerr << "Invalid argument for MIN_TIME_BETWEEN_RANDOM_ANIMATIONS: " << e.what() << std::endl;
				}
			}
		}
		file.close();
		std::cout << "[LEDCube] Loaded config from ledcube.env" << std::endl;
	} else {
		std::cout << "[LEDCube] Error: Could not open ledcube config file (./ledcube.env)." << std::endl;
	}

	if (animationsFolderLoaded) {
		std::cout << "    Animation folder is '" << ANIMATIONS_FOLDER << "'" << std::endl;
	} else {
		ANIMATIONS_FOLDER = "./animations/";
		std::cout << "    Default animations folder is '" << ANIMATIONS_FOLDER << "'." << std::endl;
	}
	if (ANIMATIONS_FOLDER.back() != '/') ANIMATIONS_FOLDER += '/';
	if (minTimeLoaded) {
		std::cout << "    Min time between random animations is " << MIN_TIME_BETWEEN_RANDOM_ANIMATIONS << "ms" << std::endl;
	} else {
		MIN_TIME_BETWEEN_RANDOM_ANIMATIONS = 10000;
		std::cout << "    Default min time between random animations is " << MIN_TIME_BETWEEN_RANDOM_ANIMATIONS << "ms." << std::endl;
	}
}

void LEDCube::run() {
	if (sender == nullptr) return;
	Animation *anim = animation;

	unsigned long long now = getCurrentTimeMs();
	next_frame_time = now + (anim ? anim->getFrameDuration() : 100);
	animation_start_time = now;
	int sleep_time = anim ? std::max(2000, std::min((int)(anim->getFrameDuration() * 100 / MS_CLOCK), 1000000)) : 100000;

	while (canRun()) {
		// Update the frame
		if (anim != animation) {
			Animation *oldAnim = anim;
			animation_update_counter = animation_change_counter;
			anim = animation;
			*frame_index = 0;
			animation_start_time = getCurrentTimeMs();
			if (anim) {
				sender->setFrame(anim->getFrame(*frame_index));
				sleep_time = std::max(2000, std::min((int)(anim->getFrameDuration() * 100 / MS_CLOCK), 1000000));
			} else {
				sender->setFrame(nullptr);
				sleep_time = 100000;
			}
			if (oldAnim != nullptr) {
				while (sender != nullptr && !sender->frameIsUpdated() && canRun()) {
					usleep(1000);
				}
				delete oldAnim;
			}
			if (animation_change_handler) animation_change_handler();
		} else {
			sender->setFrame(anim ? anim->getFrame(*frame_index) : nullptr);
			animation_update_counter = animation_change_counter;
		}

		if (anim) {
			// Wait for the next frame
			while ((now = getCurrentTimeMs()) < next_frame_time && canRun() && anim == animation) {
				// Wait for the next frame
				usleep(sleep_time);
			}
			next_frame_time += anim->getFrameDuration();
			(*frame_index)++;
			if (*frame_index >= anim->getFrameCount()) {
				*frame_index = 0;
				if (animation_end_handler) animation_end_handler();
			}
		} else {
			// Wait for the next frame
			while ((now = getCurrentTimeMs()) < next_frame_time && canRun() && animation_update_counter == animation_change_counter) {
				usleep(100000);
			}
			next_frame_time += 100;
		}
	}
}
