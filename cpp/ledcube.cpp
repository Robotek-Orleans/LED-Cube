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

#include "tlc5940-raspberry/tlc-controller.h"
#include "tlc5940-raspberry/raspberry-gpio.h"

#include <thread>
#include <cstdlib>
#include <signal.h>
#include "PatternThread.h"

bool update_thread_running;
bool update_thread_stop;
PatternThread *pattern_thread;

void update_thread(uint16_t *colors, bool *colors_is_updated)
{
	update_thread_running = true;
	std::cout << "[update_thread] : starting..." << std::endl;

	RaspberryGPIOPin tlc_sin(23);
	RaspberryGPIOPin tlc_sclk(24);
	RaspberryGPIOPin tlc_blank(27);
	RaspberryGPIOPin tlc_dcprg(22);
	RaspberryGPIOPin tlc_vprg(6); // Not used in this example
	RaspberryGPIOPin tlc_xlat(17);
	RaspberryGPIOPin tlc_gsclk(18);

	tlc_sin.setOutput();
	tlc_sclk.setOutput();
	tlc_blank.setOutput();
	tlc_dcprg.setOutput();
	tlc_vprg.setOutput();
	tlc_xlat.setOutput();
	tlc_gsclk.setOutput();

	_12TLCController tlc_controller(&tlc_sin, &tlc_sclk, &tlc_blank, &tlc_dcprg, &tlc_vprg, &tlc_xlat, &tlc_gsclk);

	int last_check = 0;

	std::cout << "[update_thread] : start" << std::endl;

	while (!update_thread_stop)
	{
		if (!*colors_is_updated)
		{
			for (int i = 0; i < PatternThread::COLORS; i++)
			{
				tlc_controller.setChannel(i, colors[i]);
			}
			tlc_controller.update();
			*colors_is_updated = true;
		}
		tlc_controller.metronome(); // ligther than update
	}

	for (int i = 0; i < PatternThread::COLORS; i++)
	{
		tlc_controller.setChannel(i, 0);
	}
	tlc_controller.update();

	update_thread_running = false;
}

void signal_callback_handler(int signum)
{
	std::cout << "Caught signal " << signum << std::endl;

	update_thread_stop = true;

	pattern_thread->close();

	while (update_thread_running)
		sleepms(10);

	// Terminate program
	exit(signum);
}

int main(int argc, char *argv[])
{
	std::cout << "initialisation" << std::endl;
	// Initialize GPIO Pins

	if (wiringPiSetupGpio() == -1)
	{
		throw std::runtime_error("Could not setup wiringPi, running as root?");
	}

	std::cout << "initialized" << std::endl;

	update_thread_running = true;
	update_thread_stop = false;
	pattern_thread = new PatternThread();
	std::thread thread1(update_thread, pattern_thread->getPatternColors(), pattern_thread->isUpdated());
	signal(SIGINT, signal_callback_handler);

	sleepms(1);
	if (argc > 1)
	{
		std::string default_pattern(argv[1], strlen(argv[1]));
		pattern_thread->setDefaultPattern(default_pattern);
	}
	pattern_thread->start();
	std::cout << "started" << std::endl;

	thread1.join();
	pattern_thread->close();
	pattern_thread->join();

	std::cout << "done" << std::endl;
}
