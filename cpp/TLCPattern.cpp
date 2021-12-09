#include "TLCPattern.h"

TLCPattern::TLCPattern(int duration)
{
	this->duration = duration;
	/*tlc_pattern = new uint8_t **[duration];
	for (int t = 0; t < duration; ++t) {
		tlc_pattern[t] = new uint8_t *[LED_HEIGHT];
		for (int z = 0; z < LED_HEIGHT; ++z) {
			tlc_pattern[t][z] = new uint8_t[TLC_SIZE];
			std::memset(tlc_pattern[t][z], 0, sizeof(uint8_t) * TLC_SIZE);
		}
	}*/
}
/*
TLCPattern::TLCPattern(const TLCPattern &pattern)
{
	duration = pattern.duration;
	//tlc_pattern = new uint8_t **[duration];
	for (int t = 0; t < duration; ++t) {
		//tlc_pattern[t] = new uint8_t *[LED_HEIGHT];
		for (int z = 0; z < LED_HEIGHT; ++z) {
			//tlc_pattern[t][z] = new uint8_t[TLC_SIZE];
			std::memcpy(tlc_pattern[t][z], pattern.tlc_pattern[t][z], sizeof(uint8_t) * TLC_SIZE);
		}
	}
}
*/
TLCPattern::~TLCPattern()
{
	/*for (int t = 0; t < duration; ++t) {
		for (int z = 0; z < LED_HEIGHT; ++z) {
			delete[] tlc_pattern[t][z];
		}
		delete[] tlc_pattern[t];
	}
	delete[] tlc_pattern;*/
}

void TLCPattern::setData(int t, uint8_t ****matrice)
{
	uint8_t *tlc_layer;
	uint8_t **line;
	int z, y, x;
	bool y_bottom;
	int y_group;
	bool x_left;
	int i;
	uint8_t r1, g1, b1, r2, g2, b2;
	int iSplitR, iSplitG, iSplitB;

	for (z = 0; z < LED_HEIGHT; ++z) {
		tlc_layer = tlc_pattern[t][z];
		for (y = 0; y < LED_LENGTH; ++y) {
			y_bottom = (y % 2) == 0; // Partie bas ou partie haute
			y_group = y / 2;
			line = matrice[z][y];
			for (x = 0; x < LED_WIDTH; x += 2) {
				// Traitement de 2 led à la fois

				x_left = (x % LED_WIDTH) < LED_WIDTH / 2; // Partie gauche ou partie droite

				i = y_group * LED_GROUP_RGB_COLOR + x;

				if (x_left) { // Partie gauche
					if (!y_bottom) {
						i += LED_WIDTH;
					}
				} else { // Partie droite
					if (y_bottom) {
						i += LED_WIDTH;
					}
				}

				r1 = line[x][0];
				g1 = line[x][1];
				b1 = line[x][2];
				r2 = line[x + 1][0];
				g2 = line[x + 1][1];
				b2 = line[x + 1][2];

				// Split 2 LED : 6 uint16 en 9 uint8 (3 uint8 pour 3 couleurs, ou encore 6*1.5 uint8)

				iSplitR = i * INT12_TO_INT8;
				uint8_t *tlc_octet = &tlc_layer[iSplitR];
				*(tlc_octet) = r1;
				*(++tlc_octet) = (r2 & 0b11110000) >> 4;
				*(++tlc_octet) = (r2 & 0b00001111) << 4;
				*(tlc_octet += TLC_NEXT_COLOR - 2) = g1;
				*(++tlc_octet) = (g2 & 0b11110000) >> 4;
				*(++tlc_octet) = (g2 & 0b00001111) << 4;
				*(tlc_octet += TLC_NEXT_COLOR - 2) = b1;
				*(++tlc_octet) = (b2 & 0b11110000) >> 4;
				*(++tlc_octet) = (b2 & 0b00001111) << 4;

				// iSplitR = i * INT12_TO_INT8;
				// iSplitG = (i + LED_GROUP_ONE_COLOR) * INT12_TO_INT8;
				// iSplitB = (i + LED_GROUP_TWO_COLOR) * INT12_TO_INT8;
				// LED1{x,y,z} : [r1,g1,b1], LED2{x+1,y,z} : [r2,g2,b2]
				// TLC : [r1" r1'|r2' r3" ... g1" g1'|g2' g3" ... b1" b1'|b2' b3" ...]
				// tlc_layer[iSplitR] = (r1 >> 4);
				// tlc_layer[iSplitR + 1] = (r1 & 0B1111) << 4 | (r2 >> 8);
				// tlc_layer[iSplitR + 2] = (r2 & 0B11111111);
				// tlc_layer[iSplitG] = (g1 >> 4);
				// tlc_layer[iSplitG + 1] = (g1 & 0B1111) << 4 | (g2 >> 8);
				// tlc_layer[iSplitG + 2] = (g2 & 0B11111111);
				// tlc_layer[iSplitB] = (b1 >> 4);
				// tlc_layer[iSplitB + 1] = (b1 & 0B1111) << 4 | (b2 >> 8);
				// tlc_layer[iSplitB + 2] = (b2 & 0B11111111);

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

uint8_t *TLCPattern::getTLCCompatible(int t, int z)
{
	return tlc_pattern[t][z];
}
