#include "USART.h"
void USART_Init(USART_TypeDef* USARTx, uint32_t BAUD_RATE){
    uint32_t fclk = 0;
    //Enable AFIO RCC
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    //Set pin to USART Mode
    if(USARTx == USART1)
    {
        RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        fclk = RCC_GetAbp2Clk();
    }
    else if(USARTx == USART2)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
        fclk = RCC_GetAbp1Clk();
    }
    else if(USARTx == USART3)
    {
        RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        fclk = RCC_GetAbp1Clk();
    }
    //Calculate the BAUDRATE
	  double USART_DIV = 1.0 * fclk / (BAUD_RATE << 4) ;
    USARTx->BRR |= (uint32_t)USART_DIV << 4;
    USARTx->BRR |= (uint32_t)((USART_DIV - (uint32_t)USART_DIV) * 16);
}

void USART_send(USART_TypeDef *USARTx, unsigned char c)
{
		while(!(USARTx->SR & USART_SR_TXE)){;}
		USARTx->DR = c;
}

void USART_str(USART_TypeDef *USARTx, unsigned char *str)
{
	for(int i = 0; str[i] != '\0'; i++)
	{
		USART_send(USARTx, str[i]);
	}
}

unsigned char USART_receive(USART_TypeDef *USARTx)
{
    while(!(USARTx->SR & USART_SR_RXNE)){;}
    return (char)USARTx->DR;
}