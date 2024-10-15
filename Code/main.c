#include "main.h"    
uint32_t IC1Val = 0;
uint32_t IC2Val = 0;
float Freq = 0;
float Duty = 0;
uint32_t msTime = 0;
char signal_present = 0;
void TIM2_PWMIC();
void TIM1_PWMOC(uint32_t Freq, uint32_t Duty_Cycle);
//void SystemInit()
//{
//}
void TIM2_IRQHandler()
{
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        TIM2->SR &= ~TIM_SR_CC1IF;  
        IC1Val = TIM2->CCR1;  				
    }
    if (TIM2->SR & TIM_SR_CC2IF)
    {
        TIM2->SR &= ~TIM_SR_CC2IF;  
        IC2Val = TIM2->CCR2;  
    }
    Duty = (float)IC2Val * 100 / IC1Val;  
    Freq = (float)RCC_GetAbp1Clk() / IC1Val;  

    if (TIM2->SR & TIM_SR_UIF)
    {
				TIM2->CNT = 0;
        TIM2->SR &= ~TIM_SR_UIF;  
    }
}

int main()
{	
	TIM1_PWMOC(10000, 50);
	TIM2_PWMIC();
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIO_SetMode(GPIOB, 11, GPIO_OUTPUT_MODE_50M_PP);
	
	while(1)
	{
	 
	}
	return 0;
}

void TIM1_PinConfig()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);	
	GPIO_SetMode(GPIOA, 9, GPIO_ALT_MODE_50M_PP);	
}

void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle)
{
	TIM1_PinConfig();
	TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

void TIM2_PinConfig()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	GPIOA->ODR |= GPIO_ODR_ODR0;
	GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_PuPd);
}

void TIM2_PWMIC()
{
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}
