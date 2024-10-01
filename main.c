#include "main.h"                  
void TIM2_IRQHandler()
{
    
}

int main()
{
	// Enable clock for GPIOA
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// Enable clock for TIM2
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	// Configure Channel 1 in PWM input mode
	TIM2->CCMR1 |= (1 << 0);              // CC1S = 01, CC1 channel is configured as input, IC1 is mapped on TI1
	TIM2->CCER &= ~TIM_CCER_CC1P;         // Capture is done on a rising edge of IC1
	
	// Configure Channel 2 in PWM input mode
	TIM2->CCMR2 |= (1 << 8);              // CC2S = 10, CC2 channel is configured as input, IC2 is mapped on TI1
	TIM2->CCER |= TIM_CCER_CC2P;          // Capture is done on a falling edge of IC1
	
	// Trigger selection: Filtered Timer Input 1
	TIM2->SMCR |= (5 << 4);
	
	// Slave mode selection: Reset Mode
	TIM2->SMCR |= (1 << 3);
	
	// Enable capture for Channel 1
	TIM2->CCER |= TIM_CCER_CC1E;  
	
	// Enable capture for Channel 2
	TIM2->CCER |= TIM_CCER_CC2E;  
	
	// Enable TIM2
	TIM2->CR1 |= TIM_CR1_CEN;
	
	TIM2->CCER  |= TIM_CCER_CC1E;  //CH1 Capture enable
	TIM2->CCER  |= TIM_CCER_CC2E;  //CH2 Capture enable 
	TIM2->CR1 |= TIM_CR1_CEN;
	
	
//	RCC->APB2ENR = RCC_APB2ENR_IOPCEN;
//	TIM2->DIER |= TIM_DIER_CC1IE;
//  NVIC_EnableIRQ(TIM2_IRQn);
	while(1)
	{
		
	}
	return 0;
}