#include "TLCSin.h"

#ifdef DEBUG
#include <iostream>
#endif
TLCSin::TLCSin(int dataLength)
:m_dataLength(dataLength)
{
  m_emptyArrayLength = GRAYSCALELENGTH - m_dataLength;
  m_dataArray = new uint8_t[GRAYSCALELENGTH];
  memset(m_dataArray, 0, GRAYSCALELENGTH);
  //Init bcm2835
  if ( !bcm2835_init() ){
    printf("bcm2835_init failed. Are you running as root??\n");
    exit(1);
  }
  
  // Set the xlat pin to be an output
  bcm2835_gpio_fsel(XLAT_PIN, BCM2835_GPIO_FSEL_OUTP);

  //Init SPI on bcm 2835
  if ( !bcm2835_spi_begin() ){
    printf("bcm2835_spi_begin failed. Are you running as root??\n");
    exit(1);
  }

  //Setup SPI parameters
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); 	// The default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);              	// The default
  bcm2835_spi_setClockDivider(CLK_DIVIDER);	                // The default
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                 	// The default
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW); 	// the default
 
  xlatPulse();

}

TLCSin::~TLCSin(){
  #ifdef DEBUG
    std::cout << "TLCSin destructor triggered" <<std::endl;
  #endif
  memset(m_dataArray, 0, GRAYSCALELENGTH);
  bcm2835_spi_writenb((const char*) m_dataArray, GRAYSCALELENGTH);
  #ifdef DEBUG
    std::cout << "Empty SPI signal sended" <<std::endl;
  #endif
  bcm2835_spi_end();
  #ifdef DEBUG
    std::cout << "SPI closed" <<std::endl;
  #endif
  //Set BLANK to HIGH for security reasons
  bcm2835_gpio_fsel(BLANK, BCM2835_GPIO_FSEL_OUTP);
  bcm2835_gpio_write(BLANK, HIGH);
  #ifdef DEBUG
    std::cout << "BLANKETTE set to high" <<std::endl;
  #endif
  bcm2835_close();
  if(m_dataArray != nullptr) delete[] m_dataArray;
  #ifdef DEBUG
    std::cout << "TLCSin destructor ended" <<std::endl;
  #endif
}

void TLCSin::send(uint8_t *data)
{
  if(data==nullptr){
    #ifdef DEBUG
    std::cout << "is null" << std::endl;
    #endif
    exit(1);  
  }

  //Replace end of dataArray by LEDs values
  memcpy(&m_dataArray[m_emptyArrayLength],data,m_dataLength);

  #ifdef DEBUG
  std::cout << "ready to be sent" << std::endl;
  #endif

  bcm2835_spi_writenb((const char*) m_dataArray, GRAYSCALELENGTH);
  #ifdef DEBUG
  std::cout << "data sent" << std::endl;
  #endif
  xlatPulse();
}


void TLCSin::xlatPulse(){
        // wait a bit
        bcm2835_delayMicroseconds(1);
	      
        // Turn it on
        bcm2835_gpio_write(XLAT_PIN, HIGH);

        // wait a bit
        bcm2835_delayMicroseconds(1);
        
        // turn it off
        bcm2835_gpio_write(XLAT_PIN, LOW);
        
        // wait a bit
        bcm2835_delayMicroseconds(1);
}
