#if !defined(_WIN32)
#error "This code is only for Windows"
#endif

#include "../ledcube/Sender.h"
#include "ledcubewindow.h"

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
			LedCubeWindow::setFrame(m_frame);
			frame_update_counter = frame_change_counter;
		}
		usleep(1000 * LedCubeWindow::getFrameDelay());
	}
}

void Sender::turnOff() {
	_ASSERT(!isRunning());
	LedCubeWindow::setFrame(nullptr);
}

void Sender::turnOnLayer(uint8_t zLayer) {
	_ASSERT(0 <= zLayer && zLayer < 8);
}
