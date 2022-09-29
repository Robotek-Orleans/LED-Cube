#include "Frame.h"

Frame::Frame() {
	tlc_pattern = new uint8_t *[LEDCubeConst::LAYERS];
	for (int z = 0; z < LEDCubeConst::LAYERS; ++z) {
		tlc_pattern[z] = new uint8_t[LEDCubeConst::LAYER_SIZE];
		memset(tlc_pattern[z], 0, LEDCubeConst::LAYER_SIZE);
	}
}

// From RGB Array
Frame::Frame(uint8_t ****rgbArray)
	: Frame() {
	uint8_t *tlc_layer;
	uint8_t ***rgbLayer;
	int z, y, x;
	bool x_group_left;
	int x_group;
	bool y_front;
	int i;
	uint8_t temp;
	uint8_t r1, g1, b1, r2, g2, b2;
	int iSplitR;
	uint8_t *tlc_octet;

	for (z = 0; z < ZLENGTH; ++z) {
		// Inversion de couleurs
		temp = rgbArray[z][1][3][1];
		rgbArray[z][1][3][1] = rgbArray[z][1][3][2];
		rgbArray[z][1][3][2] = temp;

		rgbLayer = rgbArray[z];
		tlc_layer = tlc_pattern[z];
		for (x = 0; x < XLENGTH; ++x) {
			x_group_left = (x % 2) == 0; // Partie gauche ou droite du groupe
			x_group = x / 2;
			for (y = 0; y < YLENGTH; y += 2) {
				// Traitement de 2 led à la fois (un groupe = 2 lignes sur y)
				y_front = y < YLENGTH / 2; // Partie avant ou arrière

				i = x_group * LED_GROUP_RGB_COLOR + (6 - y);

				if (y_front == x_group_left) {
					// Front & left or Back & right => reverse
					i += YLENGTH;
				}

				r1 = rgbLayer[y + 1][x][0];
				g1 = rgbLayer[y + 1][x][1];
				b1 = rgbLayer[y + 1][x][2];
				r2 = rgbLayer[y][x][0];
				g2 = rgbLayer[y][x][1];
				b2 = rgbLayer[y][x][2];

				// Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

				iSplitR = DATALENGTH - i * INT12_TO_INT8 - 1;

				tlc_octet = &tlc_layer[iSplitR];
				*(tlc_octet) = (r1 & 0b00001111) << 4;
				*(tlc_octet - 1) = (r1 & 0b11110000) >> 4;
				*(tlc_octet - 2) = r2;
				tlc_octet -= TLC_NEXT_COLOR;
				*(tlc_octet) = (g1 & 0b00001111) << 4;
				*(tlc_octet - 1) = (g1 & 0b11110000) >> 4;
				*(tlc_octet - 2) = g2;
				tlc_octet -= TLC_NEXT_COLOR;
				*(tlc_octet) = (b1 & 0b00001111) << 4;
				*(tlc_octet - 1) = (b1 & 0b11110000) >> 4;
				*(tlc_octet - 2) = b2;

				_ASSERT(&tlc_layer[0] <= tlc_octet && tlc_octet < &tlc_layer[DATALENGTH]); // Corruption

				/**
				 * Exemple pour une couleur :
				 * 2 couleur 12 bits : r1=0bAAAAABBBBCCCC et r2=0bDDDDEEEEFFFF
				 * (binaire avec les régions représentées par des symboles)
				 *
				 * octet 1 : 0bAAAABBBB
				 * octet 2 : 0bCCCCDDDD
				 * octet 3 : 0bEEEEFFFFs
				 */
			}
		}
	}
}

Frame::Frame(const Frame &frame) {
	tlc_pattern = new uint8_t *[LEDCubeConst::LAYERS];
	for (int z = 0; z < LEDCubeConst::LAYERS; ++z) {
		tlc_pattern[z] = new uint8_t[LEDCubeConst::LAYER_SIZE];
		memcpy(tlc_pattern[z], frame.tlc_pattern[z], LEDCubeConst::LAYER_SIZE);
	}
}

Frame::~Frame() {
	for (int z = 0; z < LEDCubeConst::LAYERS; ++z) {
		delete[] tlc_pattern[z];
	}
	delete[] tlc_pattern;
	tlc_pattern = nullptr;
}

int Frame::getIndexSplitRed(int x, int y, int z, bool *y_pair) const {
	_ASSERT(0 <= x && x < XLENGTH && 0 <= y && y < YLENGTH && 0 <= z && z < ZLENGTH);
	bool x_bottom = (x % 2) == 0; // Partie bas ou partie haute
	int x_group = x / 2;
	bool y_pair2 = (y % 2) == 1;
	if (y_pair) {
		*y_pair = y_pair2;
	}
	if (y_pair2) --y;
	bool y_front = y < YLENGTH / 2; // Partie gauche ou partie droite
	int i = x_group * LED_GROUP_RGB_COLOR + (6 - y);

	if (y_front == x_bottom) {
		// Front & left or Back & right => reverse
		i += XLENGTH;
	}

	// Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

	return DATALENGTH - (i)*INT12_TO_INT8 - 1;
}

void Frame::setLed(uint8_t x, uint8_t y, uint8_t z, uint8_t r, uint8_t g, uint8_t b) {
	bool y_pair = false;
	int iSplitR = getIndexSplitRed(x, y, z, &y_pair);
	uint8_t *tlc_octet;

	if (x == 3 && y == 1) {
		// Inversion de 2 couleurs
		uint8_t tmp = g;
		g = b;
		b = tmp;
	}

	if (y_pair) {
		tlc_octet = &tlc_pattern[z][iSplitR];
		*(tlc_octet) = (r & 0b00001111) << 4;
		*(tlc_octet - 1) = (r & 0b11110000) >> 4;
		tlc_octet -= TLC_NEXT_COLOR;
		*(tlc_octet) = (g & 0b00001111) << 4;
		*(tlc_octet - 1) = (g & 0b11110000) >> 4;
		tlc_octet -= TLC_NEXT_COLOR;
		*(tlc_octet) = (b & 0b00001111) << 4;
		*(tlc_octet - 1) = (b & 0b11110000) >> 4;
	} else {
		tlc_octet = &tlc_pattern[z][iSplitR - 2];
		*(tlc_octet) = r;
		tlc_octet -= TLC_NEXT_COLOR;
		*(tlc_octet) = g;
		tlc_octet -= TLC_NEXT_COLOR;
		*(tlc_octet) = b;
	}

	_ASSERT(&tlc_pattern[z][0] <= tlc_octet && tlc_octet < &tlc_pattern[z][DATALENGTH]); // Corruption
}

unsigned int Frame::getLed(uint8_t x, uint8_t y, uint8_t z) const {
	bool y_pair = false;
	int iSplitR = getIndexSplitRed(x, y, z, &y_pair);
	uint8_t *tlc_octet;

	uint8_t r = 0, g = 0, b = 0;

	if (y_pair) {
		tlc_octet = &tlc_pattern[z][iSplitR];
		r = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;
		tlc_octet -= TLC_NEXT_COLOR;
		g = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;
		tlc_octet -= TLC_NEXT_COLOR;
		b = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;

	} else {
		tlc_octet = &tlc_pattern[z][iSplitR - 2];
		r = *(tlc_octet);
		tlc_octet -= TLC_NEXT_COLOR;
		g = *(tlc_octet);
		tlc_octet -= TLC_NEXT_COLOR;
		b = *(tlc_octet);
	}

	_ASSERT(&tlc_pattern[z][0] <= tlc_octet && tlc_octet < &tlc_pattern[z][DATALENGTH]); // Corruption

	if (x == 3 && y == 1) {
		// Inversion de 2 couleurs
		uint8_t tmp = g;
		g = b;
		b = tmp;
	}

	return (r << 16) | (g << 8) | b;
}

void Frame::fill(uint8_t r, uint8_t g, uint8_t b) {
	uint8_t *tlc_layer;
	int z, y, x;
	bool x_bottom;
	int x_group;
	bool y_left;
	int i;
	int iSplitR;
	uint8_t *tlc_octet;

	for (z = 0; z < ZLENGTH; ++z) {
		tlc_layer = tlc_pattern[z];
		for (x = 0; x < XLENGTH; ++x) {
			x_bottom = (x % 2) == 0; // Partie bas ou partie haute
			x_group = x / 2;
			for (y = 0; y < YLENGTH; y += 2) {
				// Traitement de 2 led à la fois
				y_left = y >= XLENGTH / 2; // Partie gauche ou partie droite

				i = x_group * LED_GROUP_RGB_COLOR + (7 - y);

				if (y_left) { // Partie gauche
					if (!x_bottom) {
						i += XLENGTH;
					}
				} else { // Partie droite
					if (x_bottom) {
						i += XLENGTH;
					}
				}

				// Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

				iSplitR = DATALENGTH - (i)*INT12_TO_INT8 - 1;

				tlc_octet = &tlc_layer[iSplitR];
				*(tlc_octet) = (r & 0b00001111) << 4;
				*(tlc_octet - 1) = (r & 0b11110000) >> 4;
				*(tlc_octet - 1) = r;
				tlc_octet -= TLC_NEXT_COLOR;
				*(tlc_octet) = (g & 0b00001111) << 4;
				*(tlc_octet - 1) = (g & 0b11110000) >> 4;
				*(tlc_octet - 1) = g;
				tlc_octet -= TLC_NEXT_COLOR;
				*(tlc_octet) = (b & 0b00001111) << 4;
				*(tlc_octet - 1) = (b & 0b11110000) >> 4;
				*(tlc_octet - 1) = b;

				_ASSERT(&tlc_pattern[z][0] <= tlc_octet && tlc_octet < &tlc_pattern[z][DATALENGTH]);

				/**
				 * Exemple pour une couleur :
				 * 2 couleur 12 bits : r1=0bAAAAABBBBCCCC et r2=0bDDDDEEEEFFFF
				 * (binaire avec les régions représentées par des symboles)
				 *
				 * octet 1 : 0bAAAABBBB
				 * octet 2 : 0bCCCCDDDD
				 * octet 3 : 0bEEEEFFFFs
				 */
			}
		}
	}
}

void Frame::writeBin(std::ofstream &flux) {
	flux.write((char *)tlc_pattern, sizeof(tlc_pattern));
}
void Frame::readBin(std::ifstream &flux) {
	flux.read((char *)tlc_pattern, sizeof(tlc_pattern));
}

void Frame::toRGBArray(uint8_t ****rgbArray) const {

	if (tlc_pattern == nullptr) {
		// fill with 0
		for (int z = 0; z < ZLENGTH; ++z) {
			for (int y = 0; y < YLENGTH; ++y) {
				for (int x = 0; x < XLENGTH; ++x) {
					rgbArray[z][y][x][0] = 0;
					rgbArray[z][y][x][1] = 0;
					rgbArray[z][y][x][2] = 0;
				}
			}
		}
		return;
	}

	uint8_t *tlc_layer;
	int z, y, x;
	bool x_group_left;
	int x_group;
	bool y_front;
	int i;
	int iSplitR;
	uint8_t temp;
	uint8_t *tlc_octet;

	for (z = 0; z < ZLENGTH; ++z) {
		tlc_layer = tlc_pattern[z];
		for (x = 0; x < XLENGTH; ++x) {
			x_group_left = (x % 2) == 0; // Partie bas ou partie haute
			x_group = x / 2;
			for (y = 0; y < YLENGTH; y += 2) {
				// Traitement de 2 led à la fois
				y_front = y < XLENGTH / 2; // Partie gauche ou partie droite

				i = x_group * LED_GROUP_RGB_COLOR + (6 - y);

				if (y_front == x_group_left) {
					// Front & left or Back & right => reverse
					i += XLENGTH;
				}

				// Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

				iSplitR = DATALENGTH - (i)*INT12_TO_INT8 - 1;

				tlc_octet = &tlc_layer[iSplitR];
				rgbArray[z][y + 1][x][0] = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;
				rgbArray[z][y][x][0] = *(tlc_octet - 2);
				tlc_octet -= TLC_NEXT_COLOR;
				rgbArray[z][y + 1][x][1] = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;
				rgbArray[z][y][x][1] = *(tlc_octet - 2);
				tlc_octet -= TLC_NEXT_COLOR;
				rgbArray[z][y + 1][x][2] = *(tlc_octet) >> 4 | *(tlc_octet - 1) << 4;
				rgbArray[z][y][x][2] = *(tlc_octet - 2);

				tlc_octet -= 2;
				_ASSERT(&tlc_layer[0] <= tlc_octet && tlc_octet < &tlc_layer[DATALENGTH]); // Corruption
			}
		}

		// Inversion de couleurs
		temp = rgbArray[z][1][3][1];
		rgbArray[z][1][3][1] = rgbArray[z][1][3][2];
		rgbArray[z][1][3][2] = temp;
	}
}

Frame *Frame::operator=(const Frame &frame) {
	for (int z = 0; z < LEDCubeConst::LAYERS; ++z) {
		memcpy(tlc_pattern[z], frame.tlc_pattern[z], LEDCubeConst::LAYER_SIZE);
	}
	return this;
}