/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Header for main.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
//void EnableTimerInterrupt();
void InitializeTimer();
void Input_IRQHandler(void);
void Input_TimerSetup(void);


struct channel {
	uint8_t rx_channel;
	uint8_t tx_channel;
	uint16_t output_config;
	uint8_t settings;
	uint8_t premute_delay;
	uint8_t postmute_delay;
	/* Add some reserved bytes to get a 16 byte block maybe needed in the future?*/
	uint8_t rsvd1;
	uint8_t rsvd2;
	uint8_t rsvd3;
	uint8_t rsvd4;
	uint8_t rsvd5;
	uint8_t rsvd6;
	uint8_t rsvd7;
	uint8_t rsvd8;
	uint8_t rsvd9;
} __attribute__ ((packed));

extern volatile uint8_t pressed_button;
extern uint8_t RESET_VALUES;
extern volatile uint8_t irqvals;


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/


/* Exported functions ------------------------------------------------------- */ 
#ifdef __cplusplus
}
#endif


#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
