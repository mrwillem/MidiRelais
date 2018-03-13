/*
 * buttons.c
 *
 *  Created on: 30.01.2018
 *      Author: willem
 */


#include "buttons.h"
#include "stm32f10x_adc.h"
#include "spi.h"
#include "main.h"
#include "systemevents.h"


volatile uint16_t ADCBuffer[2];

volatile uint16_t buttonstat;
volatile uint8_t buttonnum;
struct button buttons[18];

/*
 * This function is to write some initial values for the lower and higher values of the buttons.
 * It should be called only once on a brand new system.
 */
void buttonval_init(void)
{
	buttons[0].lowvalue=60;
	buttons[0].highvalue=602;
	buttons[1].lowvalue=601;
	buttons[1].highvalue=955;
	buttons[2].lowvalue=954;
	buttons[2].highvalue=1310;
	buttons[3].lowvalue=1309;
	buttons[3].highvalue=1675;
	buttons[4].lowvalue=1674;
	buttons[4].highvalue=2071;
	buttons[5].lowvalue=2070;
	buttons[5].highvalue=2519;
	buttons[6].lowvalue=2518;
	buttons[6].highvalue=3049;
	buttons[7].lowvalue=3048;
	buttons[7].highvalue=3720;
	buttons[8].lowvalue=3719;
	buttons[8].highvalue=5100;
	buttons[9].lowvalue=60;
	buttons[9].highvalue=602;
	buttons[10].lowvalue=601;
	buttons[10].highvalue=955;
	buttons[11].lowvalue=954;
	buttons[11].highvalue=1310;
	buttons[12].lowvalue=1309;
	buttons[12].highvalue=1675;
	buttons[13].lowvalue=1674;
	buttons[13].highvalue=2071;
	buttons[14].lowvalue=2070;
	buttons[14].highvalue=2519;
	buttons[15].lowvalue=2518;
	buttons[15].highvalue=3049;
	buttons[16].lowvalue=3048;
	buttons[16].highvalue=3720;
	buttons[17].lowvalue=3719;
	buttons[17].highvalue=5100;
	eeprom_write_buttonvals(&buttons);
}

/*
 * A function that reads the ADC Buffer and does some software debouncing
 * A press is registered if the button is pressed for 8 ms
 * Buttons 0-8 are on ADC0
 * Buttons 9-17 on ADC1
 */

void update_buttons(void)
{
	uint8_t i;
	static uint32_t buttontrigger;


	for(i=0;i<9;i++)
	{
		if((ADCBuffer[1] > buttons[i].lowvalue) && (ADCBuffer[1] < buttons[i].highvalue))
		{
			buttons[i].pressed = (buttons[i].pressed << 1) | 0x01;
			if(buttons[i].pressed == 0xFFFF)
				buttontrigger = (buttontrigger | 0x0001<<i);
		}
		else
		{
			buttons[i].pressed = (buttons[i].pressed << 1) & 0b1111111111111110;
			/*
			 * Check if the button was previously pressed and got released
			 */
			if((buttons[i].pressed == 0x0000) && (buttontrigger == (0x0001<<i)))
			{
				button_dispatcher(i);
				buttontrigger = (buttontrigger & (~(0x0001<<i)));
			}
		}
	}
	for(i=0;i<9;i++)
	{
		if((ADCBuffer[0] > buttons[i+9].lowvalue) && (ADCBuffer[0] < buttons[i+9].highvalue))
		{
			buttons[i+9].pressed = (buttons[i+9].pressed << 1) | 0x01;
			if(buttons[i+9].pressed == 0xFFFF)
				buttontrigger = (buttontrigger | 0x0001 << (i+9));
		}
		else
		{
			buttons[i+9].pressed = (buttons[i+9].pressed << 1) & 0b1111111111111110;
			/*
			 * Check if the button was previously pressed and got released
			 */
			if((buttons[i+9].pressed == 0x0000) && (buttontrigger == (0x0001 << (i+9))))
			{
				button_dispatcher(i+9);
				buttontrigger = (buttontrigger & (~(0x0001 << (i+9))));
			}
		}
	}
}

/*
 * The button dispatcher keeps track of button states
 *
 */
void button_dispatcher(uint8_t button)
{
	/*
	 * There are 16 Buttons in total.
	 *
	 * 1 Setup 4 trigger 4 mono 6 stereo 2 poweramp 2 up down 1 Mute
	 * 4 + 4 + 6 + 2 + 1 + 1 = 16
	 *
	 */
	/* This variable keeps track of the mode of the buttons */
	static uint8_t mode;
	static uint8_t text[20];
	if(mode == BUTTON_MODE_NORMAL)
	{
		switch(button)
		{
		case 32:
			break;
		case 17:
			mode = BUTTON_MODE_SETUP;
			GPIO_SetBits(SETUPBUTTON_GPIO, SETUPBUTTON_PIN);
			break;
		case 14:
			pressed_button = 4;
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		case 13:
			pressed_button = 5;
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		case 12:
			pressed_button = 2;
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		case 11:
			pressed_button = 3;
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		case 1:
			pressed_button=14;
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		case 0:
			break;

		default:
			/*
			 * The value send will be 16 - button because buttons are in reverse order.
			 * Number 17 is the Setup button. Button 16 should trigger event 0, Button 15 event 1 and so on
			 */

			pressed_button = (16-button);
			systemevent=(systemevent | SYSEV_BUTTON_PRESSED);
			break;
		}
	}
	else
	{
		switch(button)
		{
		case 32:
			mode = BUTTON_MODE_NORMAL;
			break;

		case 17:
			mode = BUTTON_MODE_NORMAL;

			systemevent=systemevent | SYSEV_DISPLAY_MIDICHAN;
			GPIO_ResetBits(SETUPBUTTON_GPIO, SETUPBUTTON_PIN);
			break;
		case 16:
			text[0]=5;
			text[1]=10;
			text[2]=28;
			text[3]=15;
			text[4]=12;
			text[5]=31;
			text[6]=14;
			text[7]=15;
			text[8]=12;
			text[9]=5;
			text[10]=5;
			text[11]=31;
			text[12]=21;
			text[13]=31;
			text[14]=28;
			text[15]=15;
			text[16]=31;
			text[17]=20;
			text[18]=31;
			set_moving_text(&text, 18);
			break;
		case 11:
			systemevent=systemevent | SYSEV_WRITE_CHANNELDATA;
			break;


		default:
			/*
			 * This default should be executed at startup if mode is not BUTTON_MODE_NORMAL
			 * If mode is already BUTTON_MODE_NORMAL at startup the system is in a defined state
			 */
			break;
		}
	}
}

void pushbuttoninit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;


	uint8_t i;

	eeprom_read_buttonvals(&buttons);

	ADC_Cmd(ADC1, DISABLE);
	ADC_DMACmd(ADC1, DISABLE);
	DMA_Cmd(DMA1_Channel1, DISABLE);
	ADC_DeInit(ADC1);

	GPIO_InitStructure.GPIO_Pin = SETUPBUTTON_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SETUPBUTTON_GPIO, &GPIO_InitStructure);

	ADCBuffer[0] = ADCBuffer[1]  = 0;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);


	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_BufferSize = 2;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCBuffer;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	DMA_Cmd(DMA1_Channel1, ENABLE);

	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, DISABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_HT, DISABLE);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TE, DISABLE);



	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_NbrOfChannel = 2;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;

	ADC_Init(ADC1, &ADC_InitStructure);

	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_239Cycles5);

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
