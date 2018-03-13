#ifndef BUTTONS_H
#define BUTTONS_H

#include "hw_config.h"

#define BUTTON_MODE_NORMAL 0
#define BUTTON_MODE_SETUP 1

 #define SETUPBUTTON_GPIO GPIOA
 #define SETUPBUTTON_PERIPH RCC_APB2Periph_GPIOA
 #define SETUPBUTTON_PIN GPIO_Pin_10


struct button {
	uint16_t lowvalue;
	uint16_t highvalue;
	uint16_t pressed;
};

extern volatile uint16_t ADCBuffer[2];
extern volatile uint8_t buttonnum;
void pushbuttoninit(void);
void update_buttons(void);
void button_dispatcher(uint8_t);
void buttonval_init(void);

#endif //BUTTONS_H
