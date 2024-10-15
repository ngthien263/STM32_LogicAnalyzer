#ifndef __USART_H__
#define __USART_H__

#include "stm32f10x.h"                  // Device header
#include "RCC.h"

void USART_Init(USART_TypeDef* USARTx, uint32_t BAUD_RATE);
void USART_send(USART_TypeDef *USARTx, unsigned char c);
void USART_str(USART_TypeDef *USARTx, unsigned char *str);
unsigned char USART_receive(USART_TypeDef *USARTx);

#endif /* __USART_H__ */