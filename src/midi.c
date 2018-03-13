#include <stdio.h>
#include <stdint.h>
#include "hw_config.h"
#include "midi.h"
#include "main.h"
#include "systemevents.h"

volatile uint8_t midichannel;
volatile uint8_t midiccvalue;

void midi_setup(void)
{
	/* Setup Ports for midi level translator */
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(MIDI_EN_PERIPH, ENABLE);
 	GPIO_InitStructure.GPIO_Pin =  MIDI_EN_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(MIDI_EN_GPIO, &GPIO_InitStructure);
 	midi_disable();
}
void midi_enable(void)
{
	/* Enable the midi level translator */
	MIDI_EN_GPIO->BSRR = MIDI_EN_PIN;
}

void midi_disable(void)
{
	/* Disable the MIDI level translator */
	MIDI_EN_GPIO->BRR = MIDI_EN_PIN;
}


/*
 * This function takes care of serial transmitted midi messages.
 *
 */
uint8_t midi_message_dispatcher(uint8_t *RxBuffer)
{
	/*
	 * Test for program change
	 * A program change message startx with 0xC0 which indicates the program change following the program number-
	 *
	 */
	if((RxBuffer[0] & 0xC0) == 0xC0)
	{
		if(RxBuffer[1] != 255)
		{
			if(midichannel != RxBuffer[1])
			{
				midichannel=RxBuffer[1];
				systemevent = systemevent | SYSEV_MIDICHAN_RX;
				return 0;
			}
			else
			{
					return 0;
			}
		}
	}
	if(RxBuffer[1] > 128)
	{
		/*
		 * Some shit happend during serial Rx. We missed the midi message. Reset Serial rx counter
		 */
		return 0;


	}
	/*
	 * Test for Controller message
	 * A controller message is indicated by 0xB0 - 0xBF followed by the Value
	 */
	else if((RxBuffer[0] & 0xB0) == 0xB0)
	{
		if(RxBuffer[1] != 255)
		{
			if(midiccvalue != RxBuffer[1])
			{
				midiccvalue=RxBuffer[1];
				systemevent = systemevent | SYSEV_MIDICC_RX;
			}
			return 0;
		}
	}

	return 1;
}


