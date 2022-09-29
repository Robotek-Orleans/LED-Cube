#include "../ledcube/TLCSin.h"
#include <thread>
#include <chrono>

void bcm2835_gpio_fsel(uint8_t pin, uint8_t mode) {
}

void bcm2835_gpio_write(uint8_t pin, uint8_t on) {
}

TLCSin::TLCSin(int dataLength) : m_dataLength(dataLength) {
}

TLCSin::~TLCSin() {
}

void TLCSin::send(const uint8_t *data) {
}

void TLCSin::xlatPulse() {
}
