/**
 * TLC5940 controller library for the Raspberry Pi
 * ===============================================
 *
 * With this library it is possible to control your TLC5940 LED driver
 * from your Raspberry Pi. It is a pure software based solution,
 * and does not require any kernel modifcations or whatsoever.
 *
 * Copyright (c) 2013 Lucas van Dijk <info@return1.net>
 * http://return1.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software
 * is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

 /**
  * Original project : https://github.com/lrvdijk/tlc5940-raspberry
  * Updated for pigpio
 */

#include <iostream>
#include <array>

#include "raspberry-gpio.h"

#ifndef TLC_CONTROLLER_H
#define	TLC_CONTROLLER_H

typedef uint16_t color_t;
const int frequency = 1000;

template <unsigned int NUM>
class TLCController
{
public:
	TLCController(PinNum sin_pin, PinNum sclk_pin, PinNum blank_pin, PinNum dcprg_pin, PinNum vprg_pin, PinNum xlat_pin, PinNum gsclk_pin);
	virtual ~TLCController();

	virtual void setChannel(unsigned int channel, color_t value);
	virtual void setAll(color_t value);
	virtual void clear();

	virtual void update();
	virtual void metronome();

	virtual void startClock();
	virtual void stopClock();

protected:
	virtual void doFirstCycle();
	virtual PinValue getPinValueForChannel(unsigned int channel, unsigned int bit);

	RaspberryGPIOPin sin_p;
	RaspberryGPIOPin sclk;
	RaspberryGPIOPin xlat;
	RaspberryGPIOPin dcprg;
	RaspberryGPIOPin vprg;
	RaspberryClockPin blank;
	RaspberryClockPin gsclk;

	std::array<color_t, NUM * 16> colors;
};


typedef TLCController<1> SingleTLCController;
typedef TLCController<12> _12TLCController;
typedef TLCController<3> TripleTLCController;


#endif	//TLC_CONTROLLER_H

