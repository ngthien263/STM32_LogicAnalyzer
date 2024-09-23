#include "main.h"                  // Device header

int main()
{
	RCC->APB2ENR = RCC_APB2ENR_IOPAEN;
	GPIO_SetMode(GPIOA, 8, GPIO_OUTPUT_MODE_10M_PP);
	
	while(1)
	{
			GPIOA->ODR |= (1<<8);
			for(int i = 0; i < 1234; i++);
			GPIOA->ODR &= ~(1<<8);
			for(int i = 0; i < 1234; i++);
	}
	return 0;
}