#ifndef __MAIN_H__
#define __MAIN_H__

#include "GPIO.h"
#include "RCC.h"
#include "USART.h"
#include "TIM.h"
#include <stdio.h>
#include <string.h>
volatile uint8_t sendFreqDutyOnce = 0; // Send frequency and duty cycle when there is command from Qt
//Configure USART1
void USART1_Config(uint32_t BAUD_RATE);
// Initialize PWM output for Timer 1 function
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle);
void TIM4_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle);
// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC();
void TIM3_PWMIC();
#endif /* __MAIN_H__ */