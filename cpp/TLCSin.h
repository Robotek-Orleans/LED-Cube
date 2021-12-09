#include <bcm2835.h>
#include <stdio.h>
#include <cstring>

#define CLK_DIVIDER BCM2835_SPI_CLOCK_DIVIDER_64

// Pulse on RPi Plug P1 pin 22 (which is GPIO pin 17)
#define XLAT_PIN RPI_V2_GPIO_P1_16

// Grayscale length (4096 / 8)
#define GRAYSCALELENGTH 512


class TLCSin{
    public:
        TLCSin(int dataLength = 24); // 24 Bytes = 1 TLC 
        ~TLCSin();
        void send(uint8_t* data);  
    private:
    	void xlatPulse();
    	int m_dataLength = 0;
    	int m_emptyArrayLength = 0;
    	uint8_t* m_dataArray;
    	
};




/****************
 sclk	-> SCLK0

 gsclk	-> SCLK0

 sin	-> MOSI0

 xlat	-> PIN17

 blank	-> CS0
****************/


/*
dcprg -> to gnd level 

vpprg -> to high level then always low after first xlat pulse

sclk -> clock

gsclk -> grayscale clock

sin -> signal in

xlat -> to low level if idle 

blank à high > 300ns -> LED off
blank à high < 300ns -> reset grayscale counter



MSB first,  rising, 0v idle




4096 * gsclk



transmission de 4096 bits puis blank hight, xlat pulse, blank low  
*/
