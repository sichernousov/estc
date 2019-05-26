#include "MT_STM32F4xx_USART.h"

GPIO_InitTypeDef MT_USART_CfgGPIO;
USART_InitTypeDef MT_USART_CfgUSART;

// Ready to transmit and ready to receive flags
bool MT_USART_WaitToTransmit;
bool MT_USART_WaitToReceive;

// Barriers
char MT_USART_SendBarrier;
char MT_USART_ReceiveBarrier;

// Pointer to the data that will be sent
uint8_t *MT_USART_SendDataPtr;

// Pointer to the buffer where received data will be saved
uint8_t *MT_USART_ReceivedDataPtr;

void MT_USART_Init()
{
	__enable_irq();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&MT_USART_CfgGPIO);
	MT_USART_CfgGPIO.GPIO_Mode = GPIO_Mode_AF;
	MT_USART_CfgGPIO.GPIO_Pin = GPIO_Pin_2;
	MT_USART_CfgGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	MT_USART_CfgGPIO.GPIO_OType = GPIO_OType_PP;
	MT_USART_CfgGPIO.GPIO_PuPd = GPIO_PuPd_UP;
 	GPIO_Init(GPIOA, &MT_USART_CfgGPIO);
	
	MT_USART_CfgGPIO.GPIO_Mode = GPIO_Mode_AF;
	MT_USART_CfgGPIO.GPIO_Pin = GPIO_Pin_3;
	MT_USART_CfgGPIO.GPIO_Speed = GPIO_Speed_50MHz;
	MT_USART_CfgGPIO.GPIO_OType = GPIO_OType_PP;
	MT_USART_CfgGPIO.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &MT_USART_CfgGPIO);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	USART_StructInit(&MT_USART_CfgUSART);
	MT_USART_CfgUSART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	MT_USART_CfgUSART.USART_BaudRate = MT_USART_BAUDRATE;
	USART_Init(USART2, &MT_USART_CfgUSART);	
	
	NVIC_EnableIRQ(USART2_IRQn);
	USART_Cmd(USART2, ENABLE);	
}

void MT_USART_SendData(uint8_t *pSendData, char barrier)
{
	MT_USART_SendDataPtr = pSendData;
    MT_USART_SendBarrier = barrier;
    MT_USART_WaitToTransmit = TRUE;
	
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);	
}

void MT_USART_ReceiveData(uint8_t* pReceivedData, char barrier)
{
    MT_USART_ReceivedDataPtr = pReceivedData;
    MT_USART_ReceiveBarrier = barrier;
    MT_USART_WaitToReceive = TRUE;
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

void USART2_IRQHandler()
{
	
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{	
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
        *MT_USART_ReceivedDataPtr = USART_ReceiveData(USART2);
        if (*MT_USART_ReceivedDataPtr == MT_USART_ReceiveBarrier)
		{
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
            MT_USART_WaitToReceive = FALSE;
        }
        MT_USART_ReceivedDataPtr++;
	}
	
	if (USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		USART_ClearITPendingBit(USART2, USART_IT_TC);
		USART_SendData(USART2, (uint8_t)*MT_USART_SendDataPtr);	
        if (*MT_USART_SendDataPtr == MT_USART_SendBarrier)
		{
			USART_ITConfig(USART2, USART_IT_TC, DISABLE);
            MT_USART_WaitToTransmit = FALSE;
        }
        MT_USART_SendDataPtr++;
	}
	
}
