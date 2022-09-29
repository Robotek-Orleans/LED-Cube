#pragma once

#include "../include/unistd-compat.h"
#include <fstream>
#include "LEDCubeConst.h"

// A frame at a time t (with 8 layers)
// axis : x(0)=left y(0)=front z(0)=bottom
class Frame
{
public:
	Frame();
	Frame(uint8_t ****rgbArray); // From RGB Array
	Frame(const Frame &frame);
	~Frame();

	int getIndexSplitRed(int x, int y, int z, bool *y_pair) const;
	void setLed(uint8_t x, uint8_t y, uint8_t z, uint8_t r, uint8_t g, uint8_t b);
	unsigned int getLed(uint8_t x, uint8_t y, uint8_t z) const;
	void fill(uint8_t r, uint8_t g, uint8_t b);

	constexpr const uint8_t *getLayer(int z) const {
		_ASSERT(0 <= z && z < LEDCubeConst::LAYERS);
		return tlc_pattern[z];
	}

	void writeBin(std::ofstream &flux);
	void readBin(std::ifstream &flux);
	void toRGBArray(uint8_t ****rgbArray) const;

	Frame *operator=(const Frame &frame);

private:
	uint8_t **tlc_pattern; // TODO : send an int (32 bits) to reduce time in loops (288/4 = 72 int)
};