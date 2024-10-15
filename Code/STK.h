#include "core_cm3.h"                  // Device header

volatile uint32_t msTime = 0;
void STK_Init() {
  SysTick->LOAD = RCC_GetSysClk()/1000 - 1;  // Set reload value for 1ms interval
  SysTick->CTRL.BITS.TICK_INT = SysTick_CTRL_TICKINT_Msk;  // Enable SysTick interrupt
  SysTick->CTRL.BITS.CLK_SOURCE = SysTick_CTRL_CLKSOURCE_Msk;  // Set SysTick clock source to AHB
  SysTick->CTRL.BITS.ENABLE = SysTick_CTRL_ENABLE_Msk;  // Enable SysTick timer
}

void SysTick_Handler()
{
	msTime++;
}

void Delay_ms(uint32_t time)
{
	uint32_t start = msTime;
	while(msTime - start < time);
	msTime = 0;
}