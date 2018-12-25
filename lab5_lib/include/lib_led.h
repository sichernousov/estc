/*!
\file
\brief Заголовочный файл библиотеки с описанием функций

Данный файл содержит в себе определения основных 
функций, используемых в библиотеке
*/

#ifndef LIB_LED_H
#define LIB_LED_H

#include <stm32f4xx.h>

// Функция инициализации переферии.
void LedInit(void);

// Функция устанавливает заданный в формате RGB цвет на светодиод.
void LedSetColor (uint32_t rgb);

// Функция выключения светодиода.
void LedTurnOff (void);

#endif
