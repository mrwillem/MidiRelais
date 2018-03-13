#ifndef _SPI_H
#define _SPI_H
#include "hw_config.h"
#define SPI_BLOCK_FREE 0
#define SPI_BLOCK_FAILURE 1
/*
 * For flash writes we use three different .
 * After the FLASH WRITE COMMAND STATE the interrupt handler should not pull CS High
 * but instead should initiate the data transfer
 *
 */


#define SPI_BLOCK_WRITE_DATA 2
#define SPI_BLOCK_WRITE_OUTPUTS 10
#define SPI_BLOCK_SEGMENTLED_DATA 8

extern volatile uint8_t SPI2_BLOCK;

#ifdef _USE_SPI_EEPROM
#define SPI_BLOCK_EEPROM_WRITE 4
#define SPI_BLOCK_EEPROM_COMMAND 5
#define SPI_BLOCK_EEPROM_DATA 6
#endif // _USE_SPI_EEPROM

uint8_t SPI2_send(uint8_t, uint8_t, uint32_t , uint32_t);
void SPI2_BusInit(void);
void spi_handleDMA1Ch4Interrupt(void);
#endif //_SPI_H
