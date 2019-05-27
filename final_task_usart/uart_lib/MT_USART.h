#ifndef MT_USART_H
#define MT_USART_H

#include <stm32f4xx.h>

//Set USART baudrate
#define MT_USART_BAUDRATE										(9600)

// Global variables
extern bool MT_USART_WaitToTransmit;
extern bool MT_USART_WaitToReceive;

// MT_USART functions prototypes
extern void MT_USART_Init(void);
extern void MT_USART_SendData(uint8_t *pSendData, char barrier);
extern void MT_USART_ReceiveData(uint8_t* pReceivedData, char barrier);

extern void USART2_IRQHandler(void);

#endif

