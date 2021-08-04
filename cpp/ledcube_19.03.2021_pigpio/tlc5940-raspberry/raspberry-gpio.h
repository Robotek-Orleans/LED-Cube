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

#ifndef RASPBERRY_GPIO_H
#define RASPBERRY_GPIO_H

#include "base-gpio.h"
#include "pigpio-master/pigpio.h"

typedef std::uint8_t PinNum;


/**
* This class is based upon the pigpiod library, and therefore uses
* the functions of that library to control the GPIO pins.
*
* @see https://github.com/joan2937/pigpio
*/
class RaspberryGPIOPin : public BaseGPIOPin, public PigpiodPin
{
public:
	/**
	 * This creates a new pin, based on the given pin number.
	 * @see https://pinout.xyz/ For more informations on GPIO pins
	 */
	RaspberryGPIOPin() : BaseGPIOPin() {}
	RaspberryGPIOPin(PinNum pin_num) : BaseGPIOPin(), pin_num(pin_num) {}
	~RaspberryGPIOPin() { close(); }

	virtual void close();

	virtual void setMode(PinMode pin_mode);

	virtual PinValue getValue() const { return gpioRead(pin_num); }
	virtual int setValue(PinValue value) { return gpioWrite(pin_num, value); }
	virtual inline void pulse() { setHigh(); setLow(); }
	
private:
	PinNum pin_num;
};



class RaspberryClockPin : public RaspberryGPIOPin
{
public:
	/**
	 * This creates a pin for clock systems
	 * @see https://pinout.xyz/ For more informations on GPIO pins
	 */
	RaspberryClockPin() : RaspberryGPIOPin() {}
	RaspberryClockPin(PinNum pin_num, int frequency = 0);
	~RaspberryClockPin();

	int start();
	int start(int frequency);
	void stop();

	int getFrequency() const { return frequency; }

protected:
	int frequency;
};


#endif // RASPBERRY_GPIO_H
