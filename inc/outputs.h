#ifndef _OUTPUTS_H
#define _OUTPUTS_H

#include <stdint.h>


#define OUTPUTS_2ND_POWER_PIN GPIO_Pin_8
#define OUTPUTS_2ND_POWER_GPIO_PORT GPIOA
#define OUTPUTS_2ND_POWER_PERIPH RCC_APB2Periph_GPIOA

#define OUTPUTS_MUTE_PIN GPIO_Pin_9
#define OUTPUTS_MUTE_GPIO_PORT GPIOA
#define OUTPUTS_MUTE_POWER_PERIPH RCC_APB2Periph_GPIOA

#define OUTPUTS_CLR_PIN GPIO_Pin_12
#define OUTPUTS_CLR_GPIO_PORT GPIOA
#define OUTPUTS_CLR_PERIPH RCC_APB2Periph_GPIOA
#define OUTPUTS_RCLK_PIN GPIO_Pin_11
#define OUTPUTS_RCLK_GPIO_PORT GPIOA
#define OUTPUTS_RCLK_PERIPH RCC_APB2Periph_GPIOA

void outputs_hardware_init(void);
void set_outputs(uint16_t);
void enable_outputs(uint8_t);
void set_secondpoweramp(uint8_t);

#endif _OUTPUTS_H
