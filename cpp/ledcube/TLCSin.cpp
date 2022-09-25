#include "TLCSin.h"

#ifdef DEBUG
#include <iostream>
#define LOG_COUT(x) std::cout << x << std::endl;
#else
#define LOG_COUT(x) ((void)0)
#endif // DEBUG

TLCSin::TLCSin(int dataLength) : m_dataLength(dataLength) {
	m_emptyArrayLength = GRAYSCALELENGTH - m_dataLength;
	m_dataArray = new uint8_t[GRAYSCALELENGTH];
	memset(m_dataArray, 0, GRAYSCALELENGTH);
	// Init bcm2835
	if (!bcm2835_init()) {
		printf("bcm2835_init failed. Are you running as root??\n");
		exit(1);
	}

	// Set the xlat pin to be an output
	bcm2835_gpio_fsel(XLAT_PIN, BCM2835_GPIO_FSEL_OUTP);

	// Init SPI on bcm 2835
	if (!bcm2835_spi_begin()) {
		printf("bcm2835_spi_begin failed. Are you running as root??\n");
		exit(1);
	}

	// Setup SPI parameters
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST); // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);				 // The default
	bcm2835_spi_setClockDivider(CLK_DIVIDER);				 // The default
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);				 // The default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW); // the default

	xlatPulse();
}

TLCSin::~TLCSin() {
	LOG_COUT("TLCSin destructor triggered");
	uint8_t *emptyArray = new uint8_t[GRAYSCALELENGTH];
	for (int i = 0; i < GRAYSCALELENGTH; i++)
		emptyArray[i] = 0;
	LOG_COUT("Array ready");
	bcm2835_spi_writenb((const char *)emptyArray, GRAYSCALELENGTH);
	delete[] emptyArray;
	LOG_COUT("Empty SPI signal sended");
	bcm2835_spi_end();
	LOG_COUT("SPI closed");
	// Set BLANK to HIGH for security reasons
	bcm2835_gpio_fsel(BLANK, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(BLANK, HIGH);
	LOG_COUT("BLANKETTE set to high");
	bcm2835_close();
	if (m_dataArray != nullptr) delete[] m_dataArray;
	LOG_COUT("TLCSin destructor ended");
}

void TLCSin::send(const uint8_t *data) {
	_ASSERT(data != nullptr);

	// Replace end of dataArray by LEDs values
	memcpy(&m_dataArray[m_emptyArrayLength], data, m_dataLength);

	LOG_COUT("ready to be sent");

	bcm2835_spi_writenb((const char *)m_dataArray, GRAYSCALELENGTH);
	LOG_COUT("data sent");
	xlatPulse();
}

void TLCSin::xlatPulse() {
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
