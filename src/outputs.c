

#include "outputs.h"
#include "spi.h"


void outputs_hardeware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(OUTPUTS_CLR_PERIPH, ENABLE);
	RCC_APB2PeriphClockCmd(OUTPUTS_RCLK_PERIPH, ENABLE);
	RCC_APB2PeriphClockCmd(OUTPUTS_2ND_POWER_PERIPH, ENABLE);
	RCC_APB2PeriphClockCmd(OUTPUTS_MUTE_POWER_PERIPH, ENABLE);

	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = OUTPUTS_CLR_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(OUTPUTS_CLR_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = OUTPUTS_RCLK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(OUTPUTS_RCLK_GPIO_PORT, &GPIO_InitStructure);

	/* Set Pin to its default state, high in Case of the TPIC6B595 */
	GPIO_WriteBit(OUTPUTS_CLR_GPIO_PORT, OUTPUTS_CLR_PIN, Bit_RESET);
	GPIO_WriteBit(OUTPUTS_RCLK_GPIO_PORT, OUTPUTS_RCLK_PIN, Bit_RESET);

	/* Configure Pin for Second Poweramp */
	GPIO_InitStructure.GPIO_Pin = OUTPUTS_2ND_POWER_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(OUTPUTS_2ND_POWER_GPIO_PORT, &GPIO_InitStructure);
	/* Set it to Zero */
	GPIO_WriteBit(OUTPUTS_2ND_POWER_GPIO_PORT, OUTPUTS_2ND_POWER_PIN, Bit_RESET);

	/* Configure Pin for MUTE */
	GPIO_InitStructure.GPIO_Pin = OUTPUTS_MUTE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(OUTPUTS_MUTE_GPIO_PORT, &GPIO_InitStructure);
	/* Set to Zero */
	GPIO_WriteBit(OUTPUTS_MUTE_GPIO_PORT, OUTPUTS_MUTE_PIN, Bit_RESET);


}

void set_outputs(uint16_t outputconfig)
{
	uint16_t outputdata;
	uint8_t* datapointer;
	/*
	 * 16 Bit data is stored little endian on STM32 but must be transferred big endian style.
	 * So the data is reversed with __REV16
	 */
	outputdata=__REV16(outputconfig);
	datapointer=&outputdata;
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_WRITE_OUTPUTS;
	SPI2_send(2, SPI_BLOCK_WRITE_OUTPUTS,  datapointer, datapointer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
}

void enable_outputs(uint8_t onoff)
{
	uint8_t j;
	static uint16_t data;
	if(onoff == 1)
	{
		data = 0xFFFF;
	}
	else
	{
		data = 0x0000;
	}

	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_WRITE_OUTPUTS;
	SPI2_send(2, SPI_BLOCK_WRITE_OUTPUTS,  &data, &data);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
}

void set_secondpoweramp(uint8_t value)
{
	if(value == 0)
	{
		OUTPUTS_2ND_POWER_GPIO_PORT->BRR = OUTPUTS_2ND_POWER_PIN;
	}
	else
	{
		OUTPUTS_2ND_POWER_GPIO_PORT->BSRR = OUTPUTS_2ND_POWER_PIN;
	}
}

