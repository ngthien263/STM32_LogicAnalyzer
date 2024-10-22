#ifndef __MAIN_H__
#define __MAIN_H__

#include "GPIO.h"
#include "RCC.h"
#include "USART.h"
#include "TIM.h"
#include <stdio.h>

// Declare global variables and flags
volatile uint32_t IC1Val = 0; // CCR value when catching rising edge
volatile uint32_t IC2Val = 0; // CCR value when catching falling edge
volatile uint32_t Freq = 0;		// Input frequency
volatile int Duty = 0;				// Input Duty Cycle
uint32_t old_freq = 0;
uint32_t old_duty = 0;
volatile int stringSent = 0; // Flag to check if the frequency and duty cycle string has been sent
volatile int update = 0; 		 // Flag to indicate if the frequency and duty cycle need to be updated

//Configure USART1
void USART1_Config(uint32_t BAUD_RATE);
// Initialize PWM output for Timer 1 function
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle);
// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC();
#endif /* __MAIN_H__ */