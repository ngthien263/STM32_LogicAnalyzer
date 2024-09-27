#include "main.h"                  
void TIM_Init(volatile void* TIMER, uint32_t Prescaler, uint32_t Reload)
{	
	if(TIMER == TIM1)
	{
		RCC->APB2ENR = RCC_APB2ENR_TIM1EN;
	}
	else if(TIMER == TIM2 || TIMER == TIM3 || TIMER == TIM4 || TIMER == TIM5)
		RCC->APB1ENR |= (1 << (((uint32_t)TIMER - (uint32_t)TIM2_BASE) >>  0xA));
//  else if(TIMER == TIM9 || TIMER == TIM10 || TIMER == TIM11)
//	  RCC->APB2ENR |= (1 << (((uint32_t)TIMER - (uint32_t)TIM9_BASE) >>  0xA));

	((TIM_TypeDef*) TIMER)->CR1 = 0; 
	((TIM_TypeDef*) TIMER)->CNT = 0;
	((TIM_TypeDef*) TIMER)->ARR = Reload;
	((TIM_TypeDef*) TIMER)->PSC = Prescaler;
	((TIM_TypeDef*) TIMER)->CR1 = TIM_CR1_ARPE;
	((TIM_TypeDef*) TIMER)->CR1 = TIM_CR1_CEN ;
}
int main()
{
	RCC->APB1ENR = RCC_APB1ENR_TIM2EN;
  GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_PuPd);
	TIM2->CCMR1 = TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0;
	TIM2->CCMR2 = TIM_CCMR2_CC3S_0 | TIM_CCMR2_CC4S_0;
	TIM2->CCER  = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	while(1)
	{
		
	}
	return 0;
}