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

#include "tlc5940-raspberry/tlc5940-controller.h"

#include <iostream>
#include <thread>
#include <chrono>

uint16_t bitpattern;

const PinNum sin_pin = 23,
sclk_pin = 24,
blank_pin = 27,
dcprg_pin = 22,
vprg_pin = 6, // Not used in this example
xlat_pin = 17,
gsclk_pin = 18;

void sleepMs(uint8_t ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void pattern_thread(_12TLCController tlc_controller)
{
	bool reverse = false;
	bitpattern = 1;
	std::cout << "pattern_thread" << std::endl;
	tlc_controller.startClock();
	while (true)
	{

		if (!reverse)
		{
			bitpattern += 10;
		}
		else
		{
			bitpattern -= 10;
		}

		if (bitpattern <= 1)
		{
			reverse = false;
		}
		else if (bitpattern >= 2000)
		{
			reverse = true;
		}
		std::cout << bitpattern << std::endl;

		for (int i = 0; i < 48; i++)
		{
			tlc_controller.setChannel(i, bitpattern);
			//tlc_controller.setChannel(i, (bitpattern & (1 << i)) ? 0xFF : 0);
			//tlc_controller.setChannel(i, 1023);
		}
		tlc_controller.update();

		sleepMs(100);
	}
}

int main()
{
	std::cout << "initialisation" << std::endl;
	// Initialize GPIO Pins

	if (gpioInitialise() < 0)
	{
		throw std::runtime_error("Could not setup pigpiod, running as root?");
	}

	_12TLCController tlc_controller(sin_pin, sclk_pin, blank_pin, dcprg_pin, vprg_pin, xlat_pin, gsclk_pin);

	std::cout << "demarrage" << std::endl;
	pattern_thread(tlc_controller);

	std::cout << "fin" << std::endl;
	sleepMs(100);
}

