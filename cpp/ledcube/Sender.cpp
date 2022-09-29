#include "Sender.h"

// Z axis definition
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

Sender::Sender()
	: Thread() {
	tlc = new TLCSin(DATALENGTH);
	// Set the xlat pin to be an output
	bcm2835_gpio_fsel(LAYER1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER4, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER5, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER6, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER7, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(LAYER8, BCM2835_GPIO_FSEL_OUTP);
	turnOff();
}

Sender::~Sender() {
	// Eteind toutes les couches
	stopAndTurnOff();
	if (tlc != nullptr) delete tlc;
	tlc = nullptr;
}

void Sender::setFrame(const Frame *frame) {
	// The main thread WRITE frame
	// Sender's thread READ frame
	// There is no parallel write access (https://stackoverflow.com/a/45749753/12908345)
	m_frame = frame;
	frame_change_counter++;
}

bool Sender::frameIsUpdated() const {
	return frame_change_counter == frame_update_counter;
}

void Sender::stopAndTurnOff() {
	stop();
	join();
	turnOff();
}

void Sender::run() {
	while (canRun()) {
		if (frame_change_counter != frame_update_counter) {
			frame_update_counter = frame_change_counter;
		}
		if (m_frame == nullptr) {
			turnOff();
			usleep(10000);
			continue;
		}

		for (int z = 0; z < 8; z++) {
#ifdef DEBUG
			std::cout << "layer set " << z << std::endl;
#endif
			turnOnLayer(z);
			tlc->send(m_frame->getLayer(z));
		}
	}
}

void Sender::turnOff() {
	bcm2835_gpio_write(LAYER1, true);
	bcm2835_gpio_write(LAYER2, true);
	bcm2835_gpio_write(LAYER3, true);
	bcm2835_gpio_write(LAYER4, true);
	bcm2835_gpio_write(LAYER5, true);
	bcm2835_gpio_write(LAYER6, true);
	bcm2835_gpio_write(LAYER7, true);
	bcm2835_gpio_write(LAYER8, true);
}

void Sender::turnOnLayer(uint8_t zLayer) {
	switch (zLayer) {
	case 1:
		bcm2835_gpio_write(LAYER8, true);
		bcm2835_gpio_write(LAYER1, false);
		return;
	case 2:
		bcm2835_gpio_write(LAYER1, true);
		bcm2835_gpio_write(LAYER2, false);
		return;
	case 3:
		bcm2835_gpio_write(LAYER2, true);
		bcm2835_gpio_write(LAYER3, false);
		return;
	case 4:
		bcm2835_gpio_write(LAYER3, true);
		bcm2835_gpio_write(LAYER4, false);
		return;
	case 5:
		bcm2835_gpio_write(LAYER4, true);
		bcm2835_gpio_write(LAYER5, false);
		return;
	case 6:
		bcm2835_gpio_write(LAYER5, true);
		bcm2835_gpio_write(LAYER6, false);
		return;
	case 7:
		bcm2835_gpio_write(LAYER6, true);
		bcm2835_gpio_write(LAYER7, false);
		return;
	case 0:
		bcm2835_gpio_write(LAYER7, true);
		bcm2835_gpio_write(LAYER8, false);
		return;
	default:
		_ASSERT(0 <= zLayer && zLayer < 8);
		return;
	}
}
