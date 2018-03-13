#include "spi.h"
#include "eeprom.h"
#include "7segment.h"
#include "outputs.h"
#include "hw_config.h"

//volatile uint8_t SPI2_BLOCK;
#ifdef _USE_SPI_EEPROM
#include "eeprom.h"
#endif /* _USE_SPI_EEPROM */


volatile uint8_t SPI2_BLOCK;

uint8_t SPI2_send(uint8_t n_bytes, uint8_t periph, uint32_t txdata_address, uint32_t rxdata_address)
{
	uint8_t* tmp;
	tmp=txdata_address;

	/* Set DMA RX and TX Buffers */
	DMA1_Channel4->CMAR = (uint32_t)rxdata_address;
	DMA1_Channel5->CMAR = (uint32_t)txdata_address;

	DMA_SetCurrDataCounter(DMA1_Channel5, n_bytes);
	DMA_SetCurrDataCounter(DMA1_Channel4, n_bytes);

	/* Set or Reset CHIP Select depending on hardware
	 */
	switch( periph )
	{
	case SPI_BLOCK_WRITE_DATA:
		/* Set Chip Select of the DS 1801 Digipots */

	break;
	case SPI_BLOCK_SEGMENTLED_DATA:
		/* Do nothing here, the LED Data is Later written in the DMA TX IRQ pin */
		GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_SET);
	break;
	case SPI_BLOCK_WRITE_OUTPUTS:
		/* Do nothing here, the LED Data is Later written in the DMA TX IRQ pin */
		GPIO_WriteBit(OUTPUTS_RCLK_GPIO_PORT, OUTPUTS_RCLK_PIN, Bit_RESET);
	break;
	case SPI_BLOCK_EEPROM_WRITE:
	case SPI_BLOCK_EEPROM_COMMAND:
		/* Set Chip Select of Flash Device */
		GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_RESET);
	break;
	/* In case of a failure
	 * Reset everything to its default state
	 */
	default:

		// GPIO_WriteBit(LED_OE_GPIO_PORT, LED_OE_PIN, Bit_SET);
		GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_RESET);
		GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_SET);
		SPI2_BLOCK=SPI_BLOCK_FREE;
		return 1;
	break;
	}

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

	DMA_Cmd(DMA1_Channel4, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);

	return 0;
}

void SPI2_BusInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	SPI2_BLOCK=SPI_BLOCK_FREE;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	/* Debuging stuff */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_SET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_13, Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Bit_RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Bit_RESET);

	/* End of Debuging stuff */

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);




	/* Configure SCK and MOSI */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);




	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

	SPI_Cmd(SPI2, ENABLE);


	// DMA Channel 4 - SPI RX
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0x00;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);

	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

	// DMA Channel 5 - SPI TX
	DMA_InitStructure.DMA_BufferSize = 0;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = 0x00;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI2->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

void spi_handleDMA1Ch4Interrupt(void)
{
	//Test on DMA1 Channel4 Transfer Complete interrupt
	if(DMA_GetITStatus(DMA1_IT_TC4))
	{
		//Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global interrupt pending bits
		DMA_ClearITPendingBit(DMA1_IT_GL4);

		//The next line waits for rx please enable only in case of rx
		while (DMA_GetFlagStatus(DMA1_FLAG_TC5) == RESET) {}

		// wait for tx to complete - page 692
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) {}
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET) {}

		DMA_ClearFlag(DMA1_FLAG_GL5); // Clear the global flag



		// Disable DMA
		DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, DISABLE);
		// Is it important to disable the SPI DMA hardware ??
		// SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx | SPI_I2S_DMAReq_Tx, DISABLE);
		DMA_Cmd(DMA1_Channel4, DISABLE);
		DMA_Cmd(DMA1_Channel5, DISABLE);

		while (DMA1_Channel4->CCR & DMA_CCR4_EN);           // wait until DMA is actually off
		while (DMA1_Channel5->CCR & DMA_CCR5_EN);

		// Clear the Interrupt flag
		DMA_ClearFlag(DMA1_FLAG_TC4);

		/* Release Chip Select
		 *
		 */
		switch( SPI2_BLOCK )
		{
		case SPI_BLOCK_WRITE_DATA:
			/* 
			 * Release Chip Select of Digital Potis
			 * and latch data into output registers of LED drivers
			 */

			SPI2_BLOCK=SPI_BLOCK_FREE;
		break;
		case SPI_BLOCK_EEPROM_WRITE:
		case SPI_BLOCK_EEPROM_DATA:
			/* Release Chip Select of Flash Device */
			GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_SET);
			SPI2_BLOCK=SPI_BLOCK_FREE;
		break;
		case SPI_BLOCK_SEGMENTLED_DATA:
			/* Toggle chip select of LED Controler shortly */
			GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_SET);
		    __ASM volatile ("nop");
		    __ASM volatile ("nop");
		    __ASM volatile ("nop");

			GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_RESET);
			SPI2_BLOCK=SPI_BLOCK_FREE;
		break;
		case SPI_BLOCK_WRITE_OUTPUTS:
			/* Toggle chip select of LED Controler shortly */
			GPIO_WriteBit(OUTPUTS_RCLK_GPIO_PORT, OUTPUTS_RCLK_PIN, Bit_SET);
		    __ASM volatile ("nop");
		    __ASM volatile ("nop");
		    __ASM volatile ("nop");

			GPIO_WriteBit(OUTPUTS_RCLK_GPIO_PORT, OUTPUTS_RCLK_PIN, Bit_RESET);
			SPI2_BLOCK=SPI_BLOCK_FREE;
		break;
		case SPI_BLOCK_EEPROM_COMMAND:
			 SPI2_BLOCK=SPI_BLOCK_EEPROM_DATA;
			/*
			 * Initiate the Flash data transfer
			 */
			/* Set EEPROM Data DMA RX and TX Buffers */
			DMA1_Channel4->CMAR = (uint32_t)eeprom_rx_address;
			DMA1_Channel5->CMAR = (uint32_t)eeprom_tx_address;

			DMA_SetCurrDataCounter(DMA1_Channel5, eeprom_nbytes);
			DMA_SetCurrDataCounter(DMA1_Channel4, eeprom_nbytes);


			DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);

			DMA_Cmd(DMA1_Channel4, ENABLE);
			DMA_Cmd(DMA1_Channel5, ENABLE);

		break;
		/*
		 * In case of a failure
		 * Reset everything to its default state
		 */
		default:

			GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_RESET);
			GPIO_WriteBit(EEPROM_CS_GPIO_PORT, EEPROM_CS_PIN, Bit_SET);
			SPI2_BLOCK=SPI_BLOCK_FREE;
		break;
		}


	}
	else
	{
		/* Should not get here */
		SPI2_BLOCK=SPI_BLOCK_FAILURE;
	}
}
