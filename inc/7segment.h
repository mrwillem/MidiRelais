#ifndef SSEGMENT_H
#define SSEGMENT_H

#include <stdint.h>

/* Function prototypes */
void segmentled_display_number(uint8_t);

void set_moving_text(uint8_t*, uint8_t);
void update_moving_text();
void stop_moving_text();

/* Variable prototypes */
const uint16_t characters[2][32];

uint8_t text_index;
uint8_t *textdata;


#define SEGMENTLED_CS_PIN GPIO_Pin_1
#define SEGMENTLED_CS_GPIO_PORT GPIOB
#define SEGMENTLED_CS_PERIPH RCC_APB2Periph_GPIOB
#define LED_LE_PIN GPIO_Pin_6
#define LED_LE_GPIO_PORT GPIOA
#define LED_LE_PERIPH RCC_APB2Periph_GPIOA
#define LED_OE_PIN GPIO_Pin_7
#define LED_OE_GPIO_PORT GPIOA
#define LED_OE_PERIPH RCC_APB2Periph_GPIOA



#endif //SSEGMENT_H
