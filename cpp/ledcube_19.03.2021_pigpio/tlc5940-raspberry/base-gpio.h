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

#ifndef BASE_GPIO_H
#define BASE_GPIO_H

#include <cstdint>

enum PinMode : std::uint8_t
{
	IN = 0,
	INPUT = 0,
	OUT = 1,
	OUTPUT = 1
};

enum PinValue : std::uint8_t
{
	LOW = 0,
	HIGH = 1
};

/**
 * This is an abstract base class for GPIO pins.
 *
 * Subclasses should allow control of a specific GPIO pin, suitable
 * for the used processor.
 */
class BaseGPIOPin
{
public:
	virtual void open() { setMode(pin_mode); }
	virtual void close() = 0;
	virtual bool is_open() = 0;

	virtual void setMode(PinMode direction) = 0;
	PinMode getMode() const { return pin_mode; };

	// Some shortcuts
	virtual void setOutput() { setMode(PinMode::OUT); }
	virtual void setInput() { setMode(PinMode::IN); }

	virtual PinValue getValue() = 0;
	virtual int setValue(PinValue value) = 0;

	// For the value also some shortcuts
	void setHigh() { setValue(PinValue::HIGH); }
	void setLow() { setValue(PinValue::LOW); }

	inline void pulse() { setHigh(); setLow(); }

protected:
	PinMode pin_mode;
};

#endif // BASE_GPIO_H
