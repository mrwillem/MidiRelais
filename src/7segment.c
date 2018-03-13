
#include "7segment.h"
#include "spi.h"
#include "systemevents.h"

/*
 * Some shared variables
 * There should be no need to use them outside of the functions
 */
uint8_t text_index;
uint8_t *textdata;
uint8_t textlength;

void led_controller_hardware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(LED_LE_PERIPH , ENABLE);
	RCC_APB2PeriphClockCmd(LED_OE_PERIPH , ENABLE);
	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LED_LE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_LE_GPIO_PORT, &GPIO_InitStructure);

	/* Set latch enable pin of the led controller to low state */
	GPIO_WriteBit(LED_LE_GPIO_PORT, LED_LE_PIN, Bit_RESET);


	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LED_OE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_OE_GPIO_PORT, &GPIO_InitStructure);

	/* Set the OUTPUT ENABLE Pin of the LED Controller HIGH. This disables output. So all LDRs are off */
	GPIO_WriteBit(LED_OE_GPIO_PORT, LED_OE_PIN, Bit_SET);
}

void segmentled_hardeware_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(SEGMENTLED_CS_PERIPH , ENABLE);

	/* Configure Chip Select (NCS) in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = SEGMENTLED_CS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SEGMENTLED_CS_GPIO_PORT, &GPIO_InitStructure);

	/* Set Pin to its default state, high in Case of the M95M01 */
	GPIO_WriteBit(SEGMENTLED_CS_GPIO_PORT, SEGMENTLED_CS_PIN, Bit_RESET);
}

void segmentled_display_number(uint8_t number)
{
	uint8_t j;
	static uint16_t data;
	data = 0;
	j=0;
	while(number > 128)
		number --;
	if(number<10)
	{
		data=0x0000|characters[1][number];
	}
	else
	{
		if(number > 99)
		{
			data = 0x0300;
			number -=100;
		}
		while(number > 9)
		{
			number-=10;
			j++;
		}
		data=data | characters[0][j]  | characters[1][number];
	}
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_SEGMENTLED_DATA;
	SPI2_send(2, SPI_BLOCK_SEGMENTLED_DATA,  &data, &data);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
}

void set_moving_text(uint8_t *data, uint8_t length)
{
	uint16_t buffer;
	textdata=data;
	textlength=length;
	buffer=0 | characters[0][textdata[0]]  | characters[1][textdata[1]];
	switch(textlength)
	{
	case 1:
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		SPI2_BLOCK=SPI_BLOCK_SEGMENTLED_DATA;
		SPI2_send(1, SPI_BLOCK_SEGMENTLED_DATA,  &buffer, &buffer);
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		break;
	case 2:
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		SPI2_BLOCK=SPI_BLOCK_SEGMENTLED_DATA;
		SPI2_send(2, SPI_BLOCK_SEGMENTLED_DATA,  &buffer, &buffer);
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		break;
	default:
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		SPI2_BLOCK=SPI_BLOCK_SEGMENTLED_DATA;
		SPI2_send(2, SPI_BLOCK_SEGMENTLED_DATA,  &buffer, &buffer);
		while(SPI2_BLOCK != SPI_BLOCK_FREE);
		text_index=0;
		systemevent = systemevent | SYSEV_MOVING_TEXT;
		break;
	}
}

void update_moving_text()
{
	uint16_t buffer;

	if(text_index < textlength)
	{
		text_index++;
	}
	else
	{
		text_index = 0;
	}

	buffer=0 | characters[0][textdata[text_index]]  | characters[1][textdata[1+text_index]];
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
	SPI2_BLOCK=SPI_BLOCK_SEGMENTLED_DATA;
	SPI2_send(2, SPI_BLOCK_SEGMENTLED_DATA,  &buffer, &buffer);
	while(SPI2_BLOCK != SPI_BLOCK_FREE);
}

void stop_moving_text()
{
	systemevent= (systemevent & (~SYSEV_MOVING_TEXT));
}
/*
 * Characters defined
 * 0 1 2 3 4 5 6 7 8 9
 * t H E L P r b C c b
 * n Y Q A g I J U o -
 * _ [SPACE]
 */
const uint16_t characters[2][32] = {
{0xB805, 0x8004, 0x2C05, 0xA405, 0x9404, 0xB401, 0xBC01, 0x8005, 0xBC05, 0xB405,
 0x3C00, 0x9C04, 0x3C01, 0x3800, 0x1C05, 0x0C00, 0xBC00, 0x3801, 0x2C00, 0xAC04,
 0x8C00, 0xB404, 0x9405, 0x9C05, 0xB405, 0x1800, 0xA804, 0xB804, 0xAC00, 0x0400,
 0x2000, 0x0000},
{0x40EA, 0x0082, 0x403A, 0x40B2, 0x00D2, 0x40F0, 0x40F8, 0x4082, 0x40FA, 0x40F2,
 0x0078, 0x00DA, 0x4078, 0x0068, 0x405A, 0x0018, 0x00F8, 0x4068, 0x0038, 0x00BA,
 0x0098, 0x00F2, 0x40D2, 0x40DA, 0x40F2, 0x0048, 0x00AA, 0x00EA, 0x00B8, 0x0010,
 0x0020, 0x0000}
};


