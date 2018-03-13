#include "spi.h"
#include "eeprom.h"
#include "hw_config.h"
#include "core_cm3.h"

/*
 * The eeprom_data_buffer can be used by software for data,
 * this buffer can be manipulated by the software
 * the eeprom command buffer is to provide the address and other commands to the spi interface
 * this buffer should only be manipulated by the eeprom routines and thus is neither extern nor volatile
 */
volatile uint8_t eeprom_tx_buffer[144];
uint8_t eeprom_rx_buffer[144];
uint8_t eeprom_command_buffer[6];

/*
 * variables that are needed in the spi function
 */
volatile uint32_t eeprom_rx_address;
volatile uint32_t eeprom_tx_address;
volatile uint16_t eeprom_nbytes;

uint8_t eeprom_read_defaultconfig(uint8_t* midichannel, uint8_t* config, uint8_t* pre_mute_delay, uint8_t* post_mute_delay)
{
	uint8_t temp;
	eeprom_read(2048, 4, &eeprom_rx_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		temp++;
	}
	*midichannel=eeprom_rx_buffer[0];
	*config=eeprom_rx_buffer[1];
	*pre_mute_delay=eeprom_rx_buffer[2];
	*post_mute_delay=eeprom_rx_buffer[2];
	return 0;

}

uint8_t eeprom_write_buttonvals(struct button* buttons)
{
	uint8_t i;
	uint8_t temp;
	for(i=0; i<18; i++)
	{
		temp=buttons[i].lowvalue >> 8;
		eeprom_tx_buffer[((4*i)+1)] = temp;
		eeprom_tx_buffer[(4*i)]=buttons[i].lowvalue;
		temp=buttons[i].highvalue >> 8;
		eeprom_tx_buffer[((4*i)+3)] = temp;
		eeprom_tx_buffer[((4*i)+2)] = buttons[i].highvalue;
	}
	eeprom_write(2064, &eeprom_tx_buffer, 144);

	return 0;

}

uint8_t eeprom_read_buttonvals(struct button* buttons)
{
	uint8_t i;
	uint16_t temp;
	eeprom_read(2064, 144, &eeprom_rx_buffer);
	while( 	SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		temp++;
	}
	for(i=0; i<18; i++)
	{
		temp=eeprom_rx_buffer[((4*i)+1)] << 8;
		buttons[i].lowvalue = eeprom_rx_buffer[(4*i)] | temp;
		temp=eeprom_rx_buffer[((4*i)+3)] << 8;
		buttons[i].highvalue = eeprom_rx_buffer[((4*i)+2)] | temp;
	}
	return 0;

}


uint8_t eeprom_write_defaultconfig(uint8_t midichannel, uint8_t config, uint8_t pre_mute_delay, uint8_t post_mute_delay)
{
	eeprom_tx_buffer[0]=midichannel;
	eeprom_tx_buffer[1]=config;
	eeprom_tx_buffer[2]=pre_mute_delay;
	eeprom_tx_buffer[3]=post_mute_delay;
	eeprom_write(2048, &eeprom_tx_buffer, 4);
	return 0;
}

uint8_t eeprom_read_channel(uint8_t midichannel, struct channel *thischannel)
{
	uint8_t temp;
	eeprom_read((16*midichannel), 16, thischannel);
	while( 	SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		temp++;
	}
	return 0;
}

uint8_t eeprom_write_channel(uint8_t midichannel, struct channel *thischannel)
{
	uint8_t temp;
	eeprom_write((16*midichannel), thischannel, 16);
	while( 	SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		temp++;
	}
	return 0;
}

/*
 * Set up FRAM GPIO Ports
 * In Case of the FM25CL64 it is only the Chip Select line
 * The SPI lines are configured in general SPI Configuration
 *
 */

void eeprom_hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(EEPROM_CS_PERIPH , ENABLE);

	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = EEPROM_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(EEPROM_CS_GPIO_PORT, &GPIO_InitStructure);

	/* Set Pin to its default state, high in Case of the FM25CL64B */
	GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_SET);
}


/*
 * Write Disable
 */
uint8_t eeprom_wrdi(void)
{
	eeprom_command_buffer[0]=EEPROM_WRDI;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(1, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return 0;
}

/*
 * Write Enable
 * Needs to be called before writing to EEPROM or status register
 */
uint8_t eeprom_wren(void)
{
	eeprom_command_buffer[0]=EEPROM_WREN;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(1, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return 0;
}

/*
 * Read the EEPROM Status Register
 */
uint8_t eeprom_rdsr(void)
{
	eeprom_command_buffer[0]=EEPROM_RDSR;
	eeprom_command_buffer[1]=0x00;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_EEPROM_WRITE;
	SPI2_send(2, SPI_BLOCK_EEPROM_WRITE, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	return eeprom_command_buffer[1];
}


/*
 * A function that reads nbytes at the given address to the rxbuffer
 */

uint8_t eeprom_read(uint16_t address, uint16_t nbytes, uint32_t rxbuffer_address)
{
	uint16_t revaddress;
	uint8_t* addresspointer;
	eeprom_rx_address=rxbuffer_address;
	eeprom_tx_address=rxbuffer_address;
	eeprom_nbytes = nbytes;
	eeprom_command_buffer[0] = EEPROM_READ;
	revaddress=__REV16(address);
	addresspointer=&revaddress;

	eeprom_command_buffer[1]=addresspointer[0];
	eeprom_command_buffer[2]=addresspointer[1];

	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK = SPI_BLOCK_EEPROM_COMMAND;
	SPI2_send(3, SPI_BLOCK_EEPROM_COMMAND, (uint32_t)eeprom_command_buffer, (uint32_t)rxbuffer_address);
	return 0;
}


/*
 * A function to write n bytes at the given address
 */
uint8_t eeprom_write(uint16_t address, uint32_t txbuffer_address, uint8_t nbytes)
{
	uint8_t tmpdata;
	uint16_t revaddress;
	uint8_t *addresspointer;


	/* initialize variable */
	tmpdata=0xFF;

	/* Set up the data to write */
	eeprom_nbytes = nbytes;
	eeprom_rx_address=txbuffer_address;
	eeprom_tx_address=txbuffer_address;
	/* First of all send just the Write Enable */
	eeprom_wren();
	/*
	 * Check write enabled status
	 * If write enabled failed exit with error
	 */
	tmpdata=eeprom_rdsr();
	if((tmpdata & 0b00000010) != 2)
		return 1;

	/* Next Program the write command into the command buffer */
	eeprom_command_buffer[0]=EEPROM_WRITE;
	/* The address needs to be reversed because of the endianess of the system */
	revaddress = __REV16(address);
	addresspointer=&revaddress;
	eeprom_command_buffer[1]=addresspointer[0];
	eeprom_command_buffer[2]=addresspointer[1];


	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK = SPI_BLOCK_EEPROM_COMMAND;
	SPI2_send(3, SPI_BLOCK_EEPROM_COMMAND, (uint32_t)eeprom_command_buffer, (uint32_t)eeprom_command_buffer);
	tmpdata=0;
	while(SPI2_BLOCK != SPI_BLOCK_FREE)
	{
		if(DMA_GetITStatus(DMA1_IT_TC4))
		{
			tmpdata=2;
		}
	}
	/*
	 * Since we use FRAM in this Project, we don't have to wait for the write to finish.
	 * It finishes as soon as the data is transferred
	 */
	return 0;

}

