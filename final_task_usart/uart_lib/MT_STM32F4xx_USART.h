//**************************************************************************************************
// Filename							MT_STM32F4xx_USART.h
// Platform     				STM32F4xx
// Author								Aveal (http://microtechnics.ru/)
// Current version  		1.0.0
// Date         				07.03.2013
//**************************************************************************************************

#ifndef MT_STM32F4xx_USART_H
#define MT_STM32F4xx_USART_H



//**************************************************************************************************
// Includes
//**************************************************************************************************

#include <stm32f4xx.h>

//**************************************************************************************************
// Declarations and definitions
//**************************************************************************************************


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

//****************************************** end of file *******************************************
