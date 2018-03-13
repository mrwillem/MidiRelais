/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file provides main program functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "hw_config.h"
#include <stdio.h>
#include "stm32f10x_crc.h"
#include "midi.h"
#include "usart.h"
#include "spi.h"
#include "eeprom.h"
#include "7segment.h"
#include "outputs.h"
#include "buttons.h"
#include "systemevents.h"

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @defgroup Main
  * @brief Main program body
  * @{
  */

/* External variables --------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#if defined(USE_STM32100E_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08080000)
#elif defined(USE_STM322xG_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08100000)
#elif defined(USE_STM3210C_EVAL)
#define LAST_FLASH_MEMORY_ADDRESS	((uint32_t)0x08040000)
#endif
/* Private macros ------------------------------------------------------------*/

/**
  * @brief   Small printf for GCC/RAISONANCE
  */
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#endif /* __GNUC__ */

/* Private variables ---------------------------------------------------------*/

uint8_t LEDON;
uint8_t RAMPLED;
uint8_t counter;
volatile uint8_t pressed_button;
volatile uint8_t irqvals;
volatile uint16_t systemevent;
volatile uint16_t timer;


/* Private function prototypes -----------------------------------------------*/
void Delay_us(int);
void Timer_Setup(void);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */


int main(void)
{
	uint8_t text[10];
	uint8_t i;
	uint8_t button;
	uint8_t ackmsgno, msgno, msgtype;

	uint8_t globalconfig;
	uint8_t global_pre_mute_delay;
	uint8_t global_post_mute_delay;

	int8_t tmp;
	uint8_t send_data;
	send_data=0;
	int8_t value[16];
	value[1]=0;
	systemevent=0;
	uint8_t current_midichannel;
	uint16_t siebensegmentvalue;
	current_midichannel=0xFF;
	SPI2_BLOCK=SPI_BLOCK_FREE;

	struct channel thischannel;

	/* Startup code for clock etc ... */
	RCC_Configuration();

	/* Device specific NVIC Configuration */
	NVIC_Configuration();


	InitializeTimer();
	/* Load start configuration parameters */
	// LoadConfig();
	midichannel=1;


	/*
	 * Before initializing the SPI hardware setup each SPI hardware part
	 * thus all Chip Select lines should be in theyr default state up on SPI Bus
	 * initialization.
	 */
	eeprom_hardware_init();
	segmentled_hardeware_init();
	outputs_hardeware_init();
	SPI2_BusInit();

	/* Set the outputs to a known state at this early stage */
	enable_outputs(0);

	/*
	 * Setup the pushbuttons and a timer that reads the button status each ms
	 * pushbuttons have to be set up after eeprom because the ads values are
	 * stored in the eeprom
	 */
	//buttonval_init();
	pushbuttoninit();
	Input_TimerSetup();
	/* The following call will set the button state machine to a known state */
	button_dispatcher(32);

	/* Now everything is set up start listening on the midi port */

	midi_setup();
	USART_2_Configuration();
	midi_enable();

	msgno=10;
	ackmsgno=9;
	InitializeTimer();
	/* Infinite main loop */
	msgno=eeprom_wrdi();
	msgno=eeprom_rdsr();
	msgno=eeprom_wren();
	msgno=eeprom_rdsr();
	i=0;

	pressed_button=0;



	eeprom_write_defaultconfig(1, 233, 60, 20);
	/*
	 * Read default values from configuration register and read default channel
	 * Then set default outputs.
	 */
	eeprom_read_defaultconfig(&midichannel, &globalconfig, &global_pre_mute_delay, &global_post_mute_delay);
	eeprom_read_channel(midichannel, &thischannel);
	set_outputs(thischannel.output_config);

	text[0]=12;
	text[1]=13;
	text[2]=14;
	text[3]=15;
	set_moving_text( &text, 4);

	while (1)
	{


		if(systemevent)
		{
			if(systemevent & SYSEV_SECOND_TIMER)
			{
				if( systemevent & SYSEV_MOVING_TEXT)
				{
					update_moving_text();
				}
				if( systemevent & SYSEV_BLINK_TEXT)
				{
					//update_blink_text();
				}
				systemevent= (systemevent & (~SYSEV_SECOND_TIMER));

			}
			if(systemevent & SYSEV_WRITE_CHANNELDATA)
			{
				eeprom_write_channel(midichannel, &thischannel);

				systemevent=systemevent | SYSEV_DISPLAY_MIDICHAN;
				systemevent= (systemevent & (~SYSEV_WRITE_CHANNELDATA));
			}
			if(systemevent & SYSEV_MIDICHAN_RX)
			{
				if(thischannel.rx_channel != midichannel)
				{
					eeprom_read_channel(midichannel, &thischannel);
					if(thischannel.premute_delay < 0xFF)
					{
						if(thischannel.premute_delay != 0)
						{

						}
						// Mute
					}
					else
					{
						if(global_pre_mute_delay < 0xFF && global_pre_mute_delay != 0)
						{
							// Mute
						}
					}
					set_outputs(thischannel.output_config);
					if(thischannel.postmute_delay < 0xFF)
					{

						// Also mute

					}
					else
					{
						if(global_post_mute_delay < 0xFF && global_post_mute_delay != 0)
						{

						}
					}
					segmentled_display_number(midichannel+1);

					// Load Parameters
					// eeprom_read((midichannel*128), 128, &eeprom_rx_buffer);
					// Update Parameters
					// Update Display


				}
				systemevent= (systemevent & (~SYSEV_MIDICHAN_RX));

			}
			if( systemevent & SYSEV_DISPLAY_MIDICHAN )
			{
				stop_moving_text();
				segmentled_display_number(midichannel+1);
				systemevent= (systemevent & (~SYSEV_DISPLAY_MIDICHAN));
			}
			if( systemevent & SYSEV_READINPUTS )
			{
				update_buttons();
				systemevent= (systemevent & (~SYSEV_READINPUTS));
			}
			if(systemevent & SYSEV_INCREASE_TIMER)
			{
				if(timer<0xFFFF)
					timer+=1;
				else
 					timer=0;

				systemevent= (systemevent & (~SYSEV_INCREASE_TIMER));
			}
			if(systemevent & SYSEV_BUTTON_PRESSED)
			{
				thischannel.output_config ^= 1 << pressed_button;

				if(thischannel.output_config & 0b0100000000000000)
				{
					// Set secondary poweramp
					set_secondpoweramp(0);
				}
				else
				{
					set_secondpoweramp(1);
				}
				set_outputs(thischannel.output_config);
				systemevent= (systemevent & (~SYSEV_BUTTON_PRESSED));
			}
		}
	}
}

void InitializeTimer()
{
	/* Setup a Timer each second */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
    TIM_TimeBase_InitStructure.TIM_Prescaler = 7200;
    TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBase_InitStructure.TIM_Period = 5000;
    TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBase_InitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBase_InitStructure);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* Enable the Timer Interrupt */
    NVIC_InitTypeDef nvicStructure;
    nvicStructure.NVIC_IRQChannel = TIM3_IRQn;
    nvicStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvicStructure.NVIC_IRQChannelSubPriority = 0x0;
    nvicStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvicStructure);

    /* All is set up, enable timer */
    TIM_Cmd(TIM3, ENABLE);
}


void TIM3_IRQHandler()
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        systemevent=(systemevent | SYSEV_SECOND_TIMER);
    }
}


void Timer_Setup(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; //ms
    TIM_TimeBaseStructure.TIM_Prescaler = 42 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    //TIM_Cmd(TIM2,ENABLE);
}

void Input_TimerSetup(void)
{

	/* Setup a Timer each 1 ms */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 720;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 200;
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	/* Enable the Timer Interrupt */
	NVIC_InitTypeDef nvicStructure;
	nvicStructure.NVIC_IRQChannel = TIM4_IRQn;
	nvicStructure.NVIC_IRQChannelPreemptionPriority = 3;
	nvicStructure.NVIC_IRQChannelSubPriority = 1;
	nvicStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvicStructure);

	/* All is set up, enable timer */
	TIM_Cmd(TIM4, ENABLE);
}

void Input_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
        systemevent=(systemevent | SYSEV_READINPUTS);
        systemevent=(systemevent | SYSEV_INCREASE_TIMER);
    }
}


void Delay_us( int nTime)
{
    u16 counter=nTime&0xffff;
    TIM_Cmd(TIM2,ENABLE);
    TIM_SetCounter(TIM2,counter);
    while(counter>1)
    {
        counter=TIM_GetCounter(TIM2);
    }
    TIM_Cmd(TIM2,DISABLE);
}



/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
