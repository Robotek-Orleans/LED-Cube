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

#include "raspberry-gpio.h"

void RaspberryGPIOPin::close()
{
	setLow();
	setMode(PinMode::IN);// off
}

void RaspberryGPIOPin::setMode(PinMode pin_mode)
{
	// Call Pigpiod function
	gpioSetMode(pin_num, pin_mode ? PI_OUTPUT : PI_INPUT);
	this->pin_mode = pin_mode;
}




RaspberryClockPin(PinNum pin_num, int frequency) : RaspberryGPIOPin(pin_num)
{
	this->frequency = frequency;
	setOutput();// TODO: peut-être gpioSetMode(pin_num, PI_ALT0);
}
RaspberryClockPin::~RaspberryClockPin()
{
	stop();
}

int RaspberryClockPin::start()
{
	return hardware_clock(pi, pin_num, frequency);
}

int RaspberryClockPin::start(int frequency)
{
	this->frequency = frequency;
	return start();
}

int RaspberryClockPin::stop()
{
	return hardware_clock(pi, pin_num, 0);
}