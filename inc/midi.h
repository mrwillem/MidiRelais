
#ifndef MIDI_H
#define MIDI_H

#define MIDI_EN_GPIO GPIOA
#define MIDI_EN_PERIPH RCC_APB2Periph_GPIOA
#define MIDI_EN_PIN GPIO_Pin_1



extern uint8_t midioutstate;
extern volatile uint8_t midichannel;
extern volatile uint8_t midicontrolvalue;


void midi_setup(void);
void midi_enable(void);
void midi_disable(void);
uint8_t midi_message_dispatcher(uint8_t* RxBuffer);


#endif //MIDI_H
