// A Sender wihtout BCM2835 or any Window.
// The cube isn't rendered at all.

#include "../ledcube/Sender.h"

Sender::Sender()
	: Thread() {
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
	return true;
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
		usleep(1000 * 100);
	}
}

void Sender::turnOff() {
	_ASSERT(!isRunning());
}

void Sender::turnOnLayer(uint8_t zLayer) {
	_ASSERT(0 <= zLayer && zLayer < 8);
}
