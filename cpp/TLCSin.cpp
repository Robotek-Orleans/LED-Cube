#include "TLCSin.h"
#include <iostream>

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
  bcm2835_spi_end();
  bcm2835_close();
  delete[] m_dataArray;
}

void TLCSin::send(uint8_t *data)
{
  //for(int i =GRAYSCALELENGTH-1; i>m_dataArrayLength-1;i--) m_dataArray[i] = data[i-m_dataArrayLength];
  //for(int i =m_emptyArrayLength; i<GRAYSCALELENGTH;i++) m_dataArray[i] = data[i-m_emptyArrayLength];
  if(data==NULL){
    std::cout << "is null" << std::endl;
    exit(1);  
  }

  memcpy(&m_dataArray[m_emptyArrayLength],data,m_dataLength);
  //std::cout << "ready to be sent" << std::endl;
  bcm2835_spi_writenb((const char*) m_dataArray, GRAYSCALELENGTH);
  //bcm2835_spi_writenb((const char*) data, m_dataLength);
  //std::cout << "data sent" << std::endl;
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
