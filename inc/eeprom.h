#ifndef EEPROM_H
#define EEPROM_H

/* include buttons.h fpr eeprom_read_buttonvals */
#include "buttons.h"
#define EEPROM_CS_GPIO_PORT GPIOB
#define EEPROM_CS_PERIPH RCC_APB2Periph_GPIOB
#define EEPROM_CS_PIN GPIO_Pin_12


/*
 * Define EEPROM Command bytes
 * Note: The bytes are in reverse order because the eeprom needs MSB first data
 * but the SPI Bus is configured LSB first due to the SPI POTIs
 */

#define EEPROM_WREN 0x06
#define EEPROM_WRDI 0x04
#define EEPROM_RDSR 0x05
#define EEPROM_WRSR 0x01
#define EEPROM_READ 0x03
#define EEPROM_WRITE 0x02

void EEPROM_Hardware_Init(void);
uint8_t eeprom_wrdi(void);
uint8_t eeprom_wren(void);
uint8_t eeprom_rdsr(void);
uint8_t eeprom_read(uint16_t, uint16_t, uint32_t);
uint8_t eeprom_write(uint16_t, uint32_t, uint8_t);
uint8_t eeprom_write_defaultconfig(uint8_t, uint8_t, uint8_t, uint8_t);
uint8_t eeprom_read_defaultconfig(uint8_t* , uint8_t* , uint8_t* , uint8_t* );

uint8_t eeprom_read_buttonvals(struct button*);
uint8_t eeprom_write_buttonvals(struct button*);


extern volatile uint32_t eeprom_rx_address;
extern volatile uint32_t eeprom_tx_address;
extern volatile uint16_t eeprom_nbytes;
extern volatile uint8_t eeprom_tx_buffer[144];
extern uint8_t eeprom_rx_buffer[144];
extern uint8_t eeprom_command_buffer[6];

#endif //EEPROM_H
