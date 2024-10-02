#include "TIM.h"

void TIM_Init(TIM_TypeDef* TIMx, uint16_t Prescaler, uint32_t Reload)
{	
	if(TIMx == TIM1)
	{
		RCC->APB2ENR = RCC_APB2ENR_TIM1EN;
	}
	else if(TIMx == TIM2 || TIMx == TIM3 || TIMx == TIM4 || TIMx == TIM5)
		RCC->APB1ENR |= (1 << (((uint32_t)TIMx - (uint32_t)TIM2_BASE) >>  0xA));
/*else if(TIMER == TIM9 || TIMER == TIM10 || TIMER == TIM11)
	  RCC->APB2ENR.REG |= (1 << (((uint32_t)TIMER - (uint32_t)TIM9_BASEADDR) >>  0xA))
*/
	((TIM_TypeDef*)TIMx)->CR1 = 0;
	((TIM_TypeDef*)TIMx)->CNT = 0;
	((TIM_TypeDef*)TIMx)->ARR = Reload;
	((TIM_TypeDef*)TIMx)->PSC = Prescaler;
	((TIM_TypeDef*)TIMx)->CR1 |= TIM_CR1_ARPE;
	((TIM_TypeDef*)TIMx)->CR1 |= TIM_CR1_CEN;
}

void TIM_ResetCNT(TIM_TypeDef* TIMx)
{
	((TIM_TypeDef*)TIMx)->CR1 &= ~TIM_CR1_CEN;
	((TIM_TypeDef*)TIMx)->CNT = 0;
}

void TIM_PWMICM(TIM_TypeDef* TIMx, uint16_t Prescaler)
{
	TIM_Init(TIMx, Prescaler, 0xFFFF);
	TIM_ResetCNT(TIMx);
	// Configure Channel 1 in PWM input mode
	TIMx->CCMR1 |= (1 << 0);              // CC1S = 01, CC1 channel is configured as input, IC1 is mapped on TI1
	TIMx->CCER &= ~TIM_CCER_CC1P;         // Capture is done on a rising edge of IC1
	
	// Configure Channel 2 in PWM input mode
	TIMx->CCMR2 |= (1 << 8);              // CC2S = 10, CC2 channel is configured as input, IC2 is mapped on TI1
	TIMx->CCER |= TIM_CCER_CC2P;          // Capture is done on a falling edge of IC1
	
	// Trigger selection: Filtered Timer Input 1
	TIMx->SMCR |= (5 << 4);
	
	// Slave mode selection: Reset Mode
	TIMx->SMCR |= (1 << 3);
	
	// Enable capture for Channel 1
	TIMx->CCER |= TIM_CCER_CC1E;  
	
	// Enable capture for Channel 2
	TIMx->CCER |= TIM_CCER_CC2E;  
	
	// Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
	
	TIMx->CCER  |= TIM_CCER_CC1E;  //CH1 Capture enable
	TIMx->CCER  |= TIM_CCER_CC2E;  //CH2 Capture enable 
	TIMx->CR1   |= TIM_CR1_CEN;
}