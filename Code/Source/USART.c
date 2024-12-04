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

void USART_receiveString(USART_TypeDef *USARTx, unsigned char *buffer, uint16_t max_size) {
    uint16_t index = 0;

    // Xóa n?i dung cu trong b? d?m
    //memset(buffer, 0, max_size);

    while (1) {
        // Nh?n t?ng ký t? t? UART
        char received_char = (unsigned char)USART_receive(USARTx);

        // Ki?m tra ký t? k?t thúc
        if (received_char == '\n') {
            buffer[index] = '\0'; // Thêm null terminator
            break;
        } else {
            // Luu ký t? vào b? d?m n?u chua d?y
            if (index < max_size - 1) {
                buffer[index++] = received_char;
            } else {
                // N?u b? d?m d?y, t? d?ng k?t thúc
                buffer[index] = '\0';
                break;
            }
        }
    }
}