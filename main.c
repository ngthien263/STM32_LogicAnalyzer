#include "main.h"    
uint32_t IC1Val = 0;
uint32_t IC2Val = 0;
float Freq = 0;
float Duty = 0;
uint32_t msTime = 0;
void TIM2_PWMIC();
void TIM1_PWMOC(uint32_t Freq, uint32_t Duty_Cycle);
void SystemInit()
{
}
void TIM2_IRQHandler()
{
    if(TIM2->SR & TIM_SR_CC1IF)
		{
			IC1Val = TIM2->CCR1;
			TIM2->SR &= ~TIM_SR_CC1IF;
		}
		if(TIM2->SR & TIM_SR_CC2IF)
		{
			IC2Val = TIM2->CCR2;
			TIM2->SR &= ~TIM_SR_CC2IF;
		}
		Duty = IC2Val*100 / IC1Val;
		Freq = RCC_GetAbp1Clk()/IC1Val;
}

int main()
{	
	TIM1_PWMOC(10000, 30);
	TIM2_PWMIC();
	while(1)
	{
		
	}
	return 0;
}

void TIM1_PinConfig()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);	
}

void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle)
{
	TIM1_PinConfig();
	TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

void TIM2_PinConfig()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_FLOAT);
}

void TIM2_PWMIC()
{
	TIM2_PinConfig();
	TIM_PWMICMInit(TIM2);
	
	//Enable Global Interrupt
	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_EnableIRQ(TIM2_IRQn);
}