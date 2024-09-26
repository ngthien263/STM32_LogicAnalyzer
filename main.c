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
	int a = RCC_GetSysClk();
	int b = RCC_GetAbp1Clk();
	int c = RCC_GetAbp2Clk();
	int d = RCC_GetHClk();
	while(1)
	{
			GPIOA->ODR |= (1<<8);
			for(int i = 0; i < 1234; i++);
			GPIOA->ODR &= ~(1<<8);
			for(int i = 0; i < 1234; i++);
	}
	return 0;
}