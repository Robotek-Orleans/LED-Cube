#pragma once
#include "Frame.h"
#include "../Thread.h"
#include "LEDCubeConst.h"
#include "TLCSin.h"

// A thread that sends the frame to the LED cube
class Sender : public Thread
{
public:
	Sender();
	~Sender();

	void setFrame(const Frame *frame);
	bool frameIsUpdated() const;
	void stopAndTurnOff();

protected:
	void run();

private:
	void turnOff();
	void turnOnLayer(uint8_t nLayer);

	const Frame *m_frame = nullptr;
	TLCSin *tlc = nullptr;
	int frame_change_counter = 0;
	int frame_update_counter = 0;
};
