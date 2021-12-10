#include "TLCSin.h"
#include <string>
#include <fstream>
#include <time.h>

//#include "TLCPattern.h"

#define DATALENGTH 288 // Data for one layer : 8*8*3 = 192 color (int12) => * INT12_TO_INT8 = 288 octets

//Z axis
/*
#define LAYER1 RPI_V2_GPIO_P1_29
#define LAYER2 RPI_V2_GPIO_P1_31
#define LAYER3 RPI_V2_GPIO_P1_33
#define LAYER4 RPI_V2_GPIO_P1_35
#define LAYER5 RPI_V2_GPIO_P1_37
#define LAYER6 RPI_V2_GPIO_P1_36
#define LAYER7 RPI_V2_GPIO_P1_38
#define LAYER8 RPI_V2_GPIO_P1_40
*/

/*
31  -> layer 8
29  -> layer 1
33  -> layer 7
35  -> layer 6
37  -> layer 5
36  -> layer 2
38  -> layer 3
40  -> layer 4

*/
#define LAYER1 RPI_V2_GPIO_P1_29
#define LAYER2 RPI_V2_GPIO_P1_36
#define LAYER3 RPI_V2_GPIO_P1_38
#define LAYER4 RPI_V2_GPIO_P1_40
#define LAYER5 RPI_V2_GPIO_P1_37
#define LAYER6 RPI_V2_GPIO_P1_35
#define LAYER7 RPI_V2_GPIO_P1_33
#define LAYER8 RPI_V2_GPIO_P1_31

#define XLENGTH 8
#define YLENGTH 8
#define ZLENGTH 8

#define LED_GROUP_ONE_COLOR 16 // 2 lines with 1 color (1 X in red)
#define LED_GROUP_TWO_COLOR 32 // 2 lines with 2 colors (1 X in red and blue)
#define LED_GROUP_RGB_COLOR 48 // 2 lines with 3 colors (1 full X in red, blue and green)
#define INT12_TO_INT8 1.5	   // 2 int12 are splitted into 3 int8
#define TLC_NEXT_COLOR 24	   // LED_GROUP_ONE_COLOR * INT12_TO_INT8, number of octets to store 2 lines of red



class LEDCube{
  private:
    TLCSin* tlc;
    int frameTime;
    int numberFrames;
    uint8_t*** tlc_pattern;
  public:
    LEDCube(std::string fileName);
    void start();
    void setLayer(uint8_t nLayer, bool nState);
    void sendLayer(uint8_t nLayer, uint8_t* data);
    void stop();
    void setData(int t, uint8_t ****matrice);
    ~LEDCube();

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
