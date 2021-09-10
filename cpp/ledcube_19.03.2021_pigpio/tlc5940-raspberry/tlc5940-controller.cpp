#include "tlc5940-controller.h"

template<unsigned int NUM>
TLCController<NUM>::TLCController(PinNum sin_pin, PinNum sclk_pin, PinNum blank_pin, PinNum dcprg_pin, PinNum vprg_pin, PinNum xlat_pin, PinNum gsclk_pin) :
	sin_p(sin_pin), sclk(sclk_pin), blank(blank_pin), dcprg(dcprg_pin), vprg(vprg_pin), xlat(xlat_pin), gsclk(gsclk)
{
	// envoi d'un paquet d'informations à la mémoire du TLC
	sin_p.setOutput();// pin de valeur
	sin_p.setLow();

	sclk.setOutput();// tick pour le sin_pin
	sclk.setLow();

	xlat.setOutput();// passe le paquet de l'input shift register vers le registre de stockage
	xlat.setLow();

	// configuration de la mémoire du TLC
	dcprg.setOutput();// switch DC Register / DC EEPROM
	dcprg.setLow();

	vprg.setOutput();// switch DC Register / GS Register
	vprg.setLow();

	// un paquet complet se compose de 4096 pulsations de gsclk
	// chaque paquet est séparé par un blank qui fait le reset du GSCounter
	blank.start(frequency * 1);
	gsclk.start(frequency * 4096);
	
	colors.fill(0);
	doFirstCycle();
}

template<unsigned int NUM>
void TLCController<NUM>::setChannel(unsigned int channel, color_t value)
{
	colors.at(channel) = value;
}

template<unsigned int NUM>
void TLCController<NUM>::setAll(color_t value)
{
	colors.fill(value);
}

template<unsigned int NUM>
void TLCController<NUM>::clear()
{
	colors.fill(0);
}

template<unsigned int NUM>
TLCController<NUM>::~TLCController()
{
	sin_p.close();
	sclk.close();
	xlat.close();
	dcprg.close();
	vprg.close();
	blank.close();
	gsclk.close();
}

template<unsigned int NUM>
void TLCController<NUM>::update()
{
	stopClock();
	blank.pulse();
	
	// Start with the highest channel
	int channel_counter = (NUM * 16) - 1;
	int gsclk_counter = 0;

	while (gsclk_counter < 4096) {
		if (channel_counter >= 0)
		{
			// Send the first 12 bits of the color to the TLC, MSB first
			for (int i = 11; i >= 0; i--)
			{
				PinValue value = getPinValueForChannel(channel_counter, i);

				sin_p.setValue(value);
				sclk.pulse();

				gsclk.pulse();
				gsclk_counter++;
			}

			channel_counter--;
		}
		else
		{
			sin_p.setLow();

			gsclk.pulse();
			gsclk_counter++;
		}
	}

	// If we reach here all color data has been sent to the TLC
	// And the full PWM cycle has been completed
	// Send a blank signal (so the internal GSCLK counter of the TLC is reset to zero)
	// and pulse the XLAT signal, so all data is latched in
	xlat.pulse();

	startClock();
}

template<unsigned int NUM>
void TLCController<NUM>::metronome()
{
	blank.pulse();
	for (int i = 0; i < 4096; i++) {
		gsclk.pulse();
	}
	blank.pulse();
}


template<unsigned int NUM>
void TLCController<NUM>::startClock()
{
	blank.start();
	gsclk.start();
}

template<unsigned int NUM>
void TLCController<NUM>::stopClock()
{
	blank.stop();
	gsclk.stop();
}

template<unsigned int NUM>
void TLCController<NUM>::doFirstCycle()
{
	vprg.pulse();
	TLCController<NUM>::updateInit();
	update();
	TLCController<NUM>::updatePost();
	sclk.pulse();
}

template<unsigned int NUM>
PinValue TLCController<NUM>::getPinValueForChannel(unsigned int channel, unsigned int bit)
{
	return (colors.at(channel) & (1 << bit)) >> bit;
}
