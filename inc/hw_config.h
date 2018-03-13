/**
  ******************************************************************************
  * @file    hw_config.h
  * @author  MCD Application Team
  * @version V2.0.0
  * @date    11-July-2011
  * @brief   Target config file module.
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
#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

/** @addtogroup Embedded_GUI_Example
  * @{
  */

/** @addtogroup HWConfig
  * @{
  */

#ifdef __cplusplus
 extern "C" {
#endif 
   
/* define board */
#define USE_STM32_MIDIIO_V1

#define _USE_SPI_EEPROM



/* We use std periph driver */
#define USE_STDPERIPH_DRIVER

/* Includes */
#include "stm32f10x.h"
#include <stdio.h>

   
/*
 * Define Hardware Specific IO Ports and so on
 *
 */

 /* define Port Settings of powergood pin */
 #define POWERGOOD_GPIO GPIOB
 #define POWERGOOD_PERIPH RCC_APB2Periph_GPIOB
 #define POWERGOOD_PIN GPIO_Pin_7

 /* define Port Settings of Internal Bootup Select pin */
 #define BOOTSELECT_GPIO GPIOB
 #define BOOTSELECT_PERIPH RCC_APB2Periph_GPIOB
 #define BOOTSELECT_PIN GPIO_Pin_8


 /*
  * Some device specific basic functions that dont fit in other categories
  */

 uint8_t powergood(void);
 void setup_powergood(void);
   
#ifdef __cplusplus
}
#endif

#endif /* __HARDWARE_CONFIG_H */

   
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
