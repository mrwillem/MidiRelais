/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   This file provides targets hardware configuration.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"



 uint8_t powergood(void)
 {
	 uint8_t i, sum;
	 sum=0;
	 for(i=0; i<255; i++)
	 {
		 sum += GPIO_ReadInputDataBit(POWERGOOD_GPIO, POWERGOOD_PIN);
	 }
	 if(sum == 0xFF)
	 {
		 return 0;
	 }
	 return 1;
 }

 void setup_powergood(void)
 {
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	RCC_APB2PeriphClockCmd(POWERGOOD_PERIPH, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = POWERGOOD_PIN;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
 	GPIO_Init(POWERGOOD_GPIO, &GPIO_InitStructure);

 }





/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
