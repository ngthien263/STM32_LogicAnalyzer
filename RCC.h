#include "stm32f10x.h"   
#ifndef HSE_CLK 
	#define HSE_CLK  12000000ul
#endif
#define HSI_CLK  8000000ul
uint32_t RCC_GetSysClk();
uint32_t RCC_GetHClk();
uint32_t RCC_GetAbp1Clk();
uint32_t RCC_GetAbp2Clk();
