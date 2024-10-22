#include "TIM.h"
#include "RCC.h"
void TIM_Init(TIM_TypeDef* TIMx, uint32_t Frequency)
{	
	uint32_t RCC_Clock; 
  uint32_t Reload;
  uint16_t Prescaler;
	RCC_Clock = 0; 
  Reload = 0;
  Prescaler = 0;
	//Enable Timer Clock
	if(TIMx == TIM1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		RCC_Clock = RCC_GetAbp2Clk();
	}
	else if(TIMx == TIM2 || TIMx == TIM3 || TIMx == TIM4)
	{
		RCC->APB1ENR |= (1 << (((uint32_t)TIMx - (uint32_t)TIM2_BASE) >>  0xA));
		RCC_Clock = RCC_GetAbp1Clk();
	}
	
	/* Calculate Reload Value */
	Reload = (RCC_Clock/Frequency) - 1;
	while(Reload > 0xFFFF) 
	{
		Prescaler++;
		Reload = (RCC_Clock/(Frequency*(Prescaler + 1))) - 1;
	}
		
	//Reset Timer
	TIMx->CR1 = 0;
	TIMx->CNT = 0;
	
	//Initialize Timer
	TIMx->ARR = Reload;
	TIMx->PSC = Prescaler;

	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_ResetCNT(TIM_TypeDef* TIMx)
{
	TIMx->CR1 &= ~TIM_CR1_CEN;
	TIMx->CNT = 0;
}

void TIM_PWMICMInit(TIM_TypeDef* TIMx)
{
	//Initialize Timer
	RCC->APB1ENR |= (1 << (((uint32_t)TIMx - (uint32_t)TIM2_BASE) >>  0xA));
	TIMx->PSC = 100;
	/* Config Timer for Input Capture PWM Input Mode */
	
	//TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//Enable update event, set UG bit
	TIMx->CR1 &= ~TIM_CR1_UDIS;
	TIMx->EGR |= TIM_EGR_UG;
	//Slave mode: 100 - Reset mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter and generates an update of the registers.
	TIMx->SMCR |= TIM_SMCR_SMS_2 ;
	//101 - Filtered Timer Input 1 (TI1FP1)
	TIMx->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0;
	// Configure Channel 1 in PWM input mode
	TIMx->CCMR1 |= TIM_CCMR1_CC1S_0;              // CC1S = 01, CC1 channel is configured as input, IC1 is mapped on TI1
	TIMx->CCER &= ~TIM_CCER_CC1P;                 // Capture is done on a rising edge of IC1
	// Configure Channel 2 in PWM input mode
	TIMx->CCMR1 |= TIM_CCMR1_CC2S_1;              // CC2S = 10, CC2 channel is configured as input, IC2 is mapped on TI1
	TIMx->CCER |= TIM_CCER_CC2P;                  // Capture is done on a falling edge of IC1	
	// Capture/Compare 1 interrupt enable
	TIMx->DIER  |= TIM_DIER_CC1IE | TIM_DIER_CC2IE;	
	//Enable CH1 & CH2
	TIMx->CCER  |= TIM_CCER_CC1E | TIM_CCER_CC2E;	
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_OC1_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle)
{
	//Initialize Timer
	TIM_Init(TIMx, Frequency);
	TIM_ResetCNT(TIMx);
	
	/* Config Timer for PWM Mode */
	//OC1M = 110, CC1  channel is configure as PWM Mode 1
	TIMx->CCMR1 |= (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);	
	//Preload register on TIMx_CCR1 enabled
	TIMx->CCMR1 |= TIM_CCMR1_OC1PE;
	// TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//OC1 signal is output on the corresponding output pin
	TIMx->CCER |= TIM_CCER_CC1E;
	
	TIMx->BDTR |= TIM_BDTR_OSSI|TIM_BDTR_OSSR;
	//Calculate CCR1 for Duty Cycle
	TIMx->CCR1 = (Duty_Cycle * ((float)(TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_OC2_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle)
{
	//Initialize Timer
	TIM_Init(TIMx, Frequency);
	TIM_ResetCNT(TIMx);
	
	/* Config Timer for PWM Mode */
	//OC2M = 110, CC2  channel is configure as PWM Mode 1
	TIMx->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);	
	//Preload register on TIMx_CCR2 enabled
	TIMx->CCMR1 |= TIM_CCMR1_OC2PE;
	// TIMx_ARR register is buffered
	TIMx->CR1   |= TIM_CR1_ARPE;
	//OC2 signal is output on the corresponding output pin
	TIMx->CCER  |= TIM_CCER_CC2E;
	
	TIMx->BDTR  |= TIM_BDTR_OSSI|TIM_BDTR_OSSR;
	//Calculate CCR1 for Duty Cycle
	TIMx->CCR2 = (Duty_Cycle * ((TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_OC3_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle)
{
	//Initialize Timer
	TIM_Init(TIMx, Frequency);
	TIM_ResetCNT(TIMx);
	
	/* Config Timer for PWM Mode */
	//OC3M = 110, CC3  channel is configure as PWM Mode 1
	TIMx->CCMR2 |= (TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);	
	//Preload register on TIMx_CCR3 enabled
	TIMx->CCMR2 |= TIM_CCMR2_OC3PE;
	// TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//OC1 signal is output on the corresponding output pin
	TIMx->CCER |= TIM_CCER_CC3E;
	
	TIMx->BDTR |= TIM_BDTR_OSSI|TIM_BDTR_OSSR;
	//Calculate CCR1 for Duty Cycle
	TIMx->CCR3 = (Duty_Cycle * ((TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

void TIM_OC4_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle)
{
	//Initialize Timer
	TIM_Init(TIMx, Frequency);
	TIM_ResetCNT(TIMx);
	
	/* Config Timer for PWM Mode */
	//OC4M = 110, CC4  channel is configure as PWM Mode 1
	TIMx->CCMR2 |= (TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);	
	//Preload register on TIMx_CCR4 enabled
	TIMx->CCMR2 |= TIM_CCMR2_OC4PE;
	// TIMx_ARR register is buffered
	TIMx->CR1 |= TIM_CR1_ARPE;
	//OC1 signal is output on the corresponding output pin
	TIMx->CCER |= TIM_CCER_CC4E;
	
	TIMx->BDTR |= TIM_BDTR_OSSI|TIM_BDTR_OSSR;
	//Calculate CCR1 for Duty Cycle
	TIMx->CCR4 = (Duty_Cycle * ((TIMx->ARR + 1) / 100));
	//Reinitialize the counter and generates an update of the registers
	TIMx->EGR  |= TIM_EGR_UG;
	//OC and OCN outputs are enabled
	TIMx->BDTR |= TIM_BDTR_MOE;
	//Enable TIMx
	TIMx->CR1 |= TIM_CR1_CEN;
}

