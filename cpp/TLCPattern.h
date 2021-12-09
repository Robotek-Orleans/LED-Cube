#ifndef TLCPATTERN
#define TLCPATTERN

#include <cstdint>
#include <cstring>

#define LED_WIDTH 8 // One line with 1 color
#define LED_LENGTH 8
#define LED_HEIGHT 8
#define LED_GROUP_ONE_COLOR 16 // 2 lines with 1 color (1 X in red)
#define LED_GROUP_TWO_COLOR 32 // 2 lines with 2 colors (1 X in red and blue)
#define LED_GROUP_RGB_COLOR 48 // 2 lines with 3 colors (1 full X in red, blue and green)
#define INT12_TO_INT8 1.5	   // 2 int12 are splitted into 3 int8
#define TLC_SIZE 288		   // Data for one layer : 8*8*3 = 192 color (int12) => * INT12_TO_INT8 = 288 octets
#define TLC_NEXT_COLOR 24	   // LED_GROUP_ONE_COLOR * INT12_TO_INT8, number of octets to store 2 lines of red

static uint8_t flipBit(uint8_t n);

class TLCPattern
{
public:
	/**
	 * @param duration Nombre de frame du pattern
	 */
	TLCPattern(int duration);
	//TLCPattern(const TLCPattern &pattern);
	~TLCPattern();

	/**
	 * @param t Couche temps
	 * @param matrice Tableau 3D (x,y,c) ; c contient les 3 couleurs (r,g,b)
	 **/
	void setData(int t, uint8_t ****matrice);

	/**
	 * @param t Couche temps
	 * @param z Couche hauteur
	 * @return TLC Compatible
	 **/
	uint8_t *getTLCCompatible(int t, int z);

private:
	int duration;
	/*
	Tableau 3D : (TLCCompatible, z, t)
	TLCCompatible : 2 LED = 6*couleurs (6*12 bits) splité en 9 * 8 bits
	*/
	uint8_t tlc_pattern[2][8][TLC_SIZE];
};

#endif // TLCPATTERN