#ifndef __TIM_H__
#define __TIM_H__

#include "stm32f10x.h"                  // Device header

void TIM_PWMICMInit(TIM_TypeDef* TIMx);
void TIM_OC1_PWMM1_Init(TIM_TypeDef* TIMx, uint32_t Frequency, uint32_t Duty_Cycle);
#endif /* __TIM_H__ */