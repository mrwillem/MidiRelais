/*
 * systemevents.h
 *
 *  Created on: 12.03.2018
 *      Author: willem
 */

#ifndef SYSTEMEVENTS_H_
#define SYSTEMEVENTS_H_

/*
 * The variable should be declared in main.c
 */
extern volatile uint16_t systemevent;

#define SYSEV_MIDICHAN_RX 0b0000000000000001
#define SYSEV_MIDICC_RX 0b0000000000000010
#define SYSEV_BUTTON_PRESSED 0b0000000000000100
#define SYSEV_READINPUTS 0b0000000000001000
#define SYSEV_INCREASE_TIMER 0b0000000000010000
#define SYSEV_SECOND_TIMER 0b0000000000100000
#define SYSEV_MOVING_TEXT 0b0000000001000000
#define SYSEV_BLINK_TEXT 0b0000000010000000
#define SYSEV_DISPLAY_MIDICHAN 0b0000000100000000
#define SYSEV_WRITE_CHANNELDATA 0b0000001000000000

#endif /* SYSTEMEVENTS_H_ */
