#pragma once

#define DATALENGTH 288 // Data for one layer : 8*8*3 = 192 color (int12) => * INT12_TO_INT8 = 288 octets

// LEDCube length
#define XLENGTH 8
#define YLENGTH 8
#define ZLENGTH 8

// Geometry correction
#define LED_GROUP_ONE_COLOR 16 // 2 lines with 1 color (1 X in red)
#define LED_GROUP_TWO_COLOR 32 // 2 lines with 2 colors (1 X in red and blue)
#define LED_GROUP_RGB_COLOR 48 // 2 lines with 3 colors (1 full X in red, blue and green)

// 12*2 bits to 8*3 bits convertion
#define INT12_TO_INT8 1.5 // 2 int12 are splitted into 3 int8
#define TLC_NEXT_COLOR 24 // LED_GROUP_ONE_COLOR * INT12_TO_INT8, number of octets to store 2 lines of red

// Clock definition
#define MS_CLOCK (CLOCKS_PER_SEC / 1000)

namespace LEDCubeConst {
	const int LAYERS = ZLENGTH;
	const int LAYER_SIZE = DATALENGTH;
} // namespace LEDCubeConst
