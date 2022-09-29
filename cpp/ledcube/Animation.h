#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include "Frame.h"

class Animation
{
public:
	Animation(int frameCount, int frameDuration);
	Animation(const Animation &animation);
	~Animation();

	void clear();
	void fill(uint8_t t, uint8_t r, uint8_t g, uint8_t b);
	void setLed(uint8_t x, uint8_t y, uint8_t z, uint8_t t, uint8_t r, uint8_t g, uint8_t b);

	void setFrame(int t, const Frame &frame);
	constexpr const Frame *getFrame(int frame) const { return &frames[frame]; }
	constexpr int getFrameCount() const { return frameCount; }
	constexpr int getFrameDuration() const { return frameDuration; }
	std::string getName() const { return name; }
	void setName(std::string name) { this->name = name; }

private:
	Frame *frames = nullptr;
	int frameCount;
	int frameDuration;
	std::string name = "";
};