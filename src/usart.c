#include "hw_config.h"
#include "usart.h"
#include "midi.h"
#include "stm32f10x_usart.h"


volatile char BufferUsart2[32];


void USART_2_Configuration(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef   USART_InitStructure;

	// Enable periphals
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);


    /* Define Pin Tx2 as Push-Pull Output */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Define Pin Rx2 as Input Floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);



    // Initialize USART
    USART_InitStructure.USART_BaudRate = 31250;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);

    /* Enable the USART */
    USART_Cmd(USART2, ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;        // we want to configure the USART2 interrupts
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;// this sets the priority group of the USART2 interrupts
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;       // this sets the subpriority inside the group
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          // the USART2 interrupts are globally enabled
    NVIC_Init(&NVIC_InitStructure);

    /* enable the USART2 receive not empty interrupt */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    /* Enable USART2 global interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
}

void USART2_InterruptHandler(void)
{
	static uint8_t rx_index;
	uint16_t uart_status;
	/* catch uart interrupts */
	if((USART_GetITStatus(USART2, USART_IT_FE)) != RESET )
	{
		uart_status = uart_status | MIDI_TC;
	}
	if((USART_GetITStatus(USART2, USART_IT_TC)) != RESET )
	{
		uart_status = uart_status | MIDI_TC;
	}
	if((USART_GetITStatus(USART2, USART_IT_TXE)) != RESET )
	{
		uart_status = uart_status | MIDI_TXE;
	}
	if((USART_GetITStatus(USART2, USART_IT_CTS)) != RESET )
	{
		USART_ClearITPendingBit(USART2, USART_IT_CTS);
	}
	if((USART_GetITStatus(USART2, USART_IT_LBD)) != RESET )
	{
		USART_ClearITPendingBit(USART2, USART_IT_LBD);
	}
	if((USART_GetITStatus(USART2, USART_IT_NE)) != RESET )
	{
		uart_status = uart_status | MIDI_NE;
	}
	if((USART_GetITStatus(USART2, USART_IT_PE)) != RESET )
	{
		uart_status = uart_status | MIDI_PE;
	}
	if((USART_GetITStatus(USART2, USART_IT_IDLE)) != RESET )
	{
		uart_status = uart_status | MIDI_IDLE;
	}
	if((USART_GetITStatus(USART2, USART_IT_RXNE)) != RESET)
	{
		uart_status = uart_status | MIDI_RXNE;
	}
	if((USART_GetITStatus(USART2, USART_IT_ORE)) != RESET)
	{
		uart_status = uart_status | MIDI_ORE;
	}

	if (uart_status) // Received characters modify string
	{
		BufferUsart2[rx_index] = USART_ReceiveData(USART2);
		if(((BufferUsart2[rx_index] & 0xC0) == 0xC0) || ((BufferUsart2[rx_index] & 0xB0) == 0xB0))
		{
			if(rx_index>0)
			{
				BufferUsart2[0]=BufferUsart2[rx_index];
				rx_index=0;
			}
			rx_index++;
		}
		else
		{
			if(rx_index>0)
			{
				midi_message_dispatcher(&BufferUsart2);
				rx_index=0;
			}
		}
		uart_status=0;
	}
}

