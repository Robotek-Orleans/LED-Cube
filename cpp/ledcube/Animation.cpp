#include "Animation.h"

Animation::Animation(int frameCount, int frameDuration)
	: frameCount(frameCount), frameDuration(frameDuration) {
	_ASSERT(frameCount > 0 && frameDuration > 0);
	frames = new Frame[frameCount];
}

Animation::Animation(const Animation &animation) {
	frames = new Frame[animation.frameCount];
	for (int i = 0; i < animation.frameCount; i++)
		frames[i] = animation.frames[i];
}

Animation::~Animation() {
	delete[] frames;
	frames = nullptr;
}

void Animation::fill(uint8_t t, uint8_t r, uint8_t g, uint8_t b) {
	_ASSERT(0 <= t && t < frameCount);
	frames[t].fill(r, g, b);
}

void Animation::setLed(uint8_t x, uint8_t y, uint8_t z, uint8_t t, uint8_t r, uint8_t g, uint8_t b) {
	_ASSERT(0 <= t && t < frameCount);
	frames[t].setLed(x, y, z, r, g, b);
}

void Animation::setFrame(int t, const Frame &frame) {
	_ASSERT(0 <= t && t < frameCount);
	frames[t] = frame;
}
