#ifndef USART_H
#define USART_H

extern volatile char BufferUsart2[32];


void USART_2_Configuration(void);

void USART2_InterruptHandler(void);

#define MIDI_TXE	1
#define MIDI_RXNE	2
#define MIDI_TC		4
#define MIDI_CTS	8
#define MIDI_LBD	16
#define MIDI_PE		32
#define MIDI_NE		64
#define MIDI_ORE	128
#define MIDI_IDLE	256

#endif //USART_H
