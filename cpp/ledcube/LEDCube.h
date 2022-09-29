#pragma once

#include <time.h>
#include <vector>
#include <filesystem>
#include <regex>
#include "LEDCubeConst.h"
#include "Animation.h"
#include "Sender.h"
#include "../Thread.h"
#include "AnimationLoader/AnimationLoader.h"
#include "unistd-compat.h"
#include <functional>

struct CurrentAnimationInfo
{
	bool isRandom;
	std::string currentAnimationName;
	long long currentStartedAt;
	std::string nextAnimationName;
};

class LEDCube : public Thread
{
public:
	std::string ANIMATIONS_FOLDER = "./animations/";
	int MIN_TIME_BETWEEN_RANDOM_ANIMATIONS = 10000;
	LEDCube();
	~LEDCube();

	Animation *loadAnimation(std::string fileName) const;
	bool playAnimation(std::string fileName);
	void setAnimation(Animation *animation);
	Animation *getCurrentAnimation();
	CurrentAnimationInfo getCurrentAnimationInfo() const;

	std::vector<std::string> getAnimations() const;
	std::string getRandomAnimationName(std::string filter) const;
	bool playRandomAnimation(std::string filter);
	bool playNextRandomAnimation();
	constexpr bool isRandomAnimationRunning() const { return random_animation_running; }

	constexpr int getCurrentFrameIndex() const { return *frame_index; }
	const Frame *getCurrentFrame() const;

	void setLed(int x, int y, int z, int color);
	int getLed(int x, int y, int z) const;
	void setLayer(int **colors, int layerIndex, int direction);
	int **getLayer(int layer, int direction) const;

	void setRandomAnimChangeHandler(std::function<void()> handler);
	void setRandomAnimEndHandler(std::function<void()> handler);

	void loadConfig();

protected:
	void run();

private:
	// Write from server thread only
	int animation_change_counter;
	Animation *animation;
	bool random_animation_running = false;
	std::string random_animations_filter = "";
	std::string next_random_animation_name = "";

	// Write from LEDCube thread only
	int animation_update_counter;
	Sender *sender;
	int *frame_index;
	unsigned long long animation_start_time = 0;
	unsigned long long next_frame_time = 0;
	std::function<void()> animation_end_handler = 0;	// Called from LEDCube thread
	std::function<void()> animation_change_handler = 0; // Called from LEDCube thread
};

/*
##############
# CONVERSION #
##############


72 derniers octets -> 2 premières lignes R , G et B -> 24 pour le R , 24 pour le G , 24 pour le B

Schéma lignes (même pattern * 4):
____ __ __ __   __ __ __ ____
|*__*__*__*__\ /__#__#__#__#|
____ __ __ __ X __ __ __ ____
|+__+__+__+__/ \__@__@__@__@|


en partant de la fin

# -> 0 	à 5  pour R , 24 à 29 pour G , 48 à 53 pour B

+ -> 6 	à 11 pour R , 30 à 35 pour G , 54 à 59 pour B

@ -> 12 à 17 pour R , 36 à 41 pour G , 60 à 65 pour B

* -> 18 à 23 pour R , 42 à 47 pour G , 66 à 71 pour B


fichier de base:

r
g       -> pour une LED
b

LED
LED
LED
LED     -> pour un axe X
LED
LED
LED
LED

X
X
X
X       -> pour un axe Y
X
X
X
X

Y
Y
Y
Y       -> pour un axe Z
Y
Y
Y
Y


*/
