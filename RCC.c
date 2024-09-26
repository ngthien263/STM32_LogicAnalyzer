#include "RCC.h"               // Device header
typedef enum {
    SWS_HSI         = 0,  /* HSI selected as system clock */
    SWS_HSE         = 1,  /* HSE selected as system clock */
    SWS_PLL         = 2,  /* PLL selected as system clock */
    NOT_ALLOWED     = 3   /*          Not allowed         */
} CFGR_SWS;

static const uint16_t AHBpreFactorTable[16U] = {1, 1, 1, 1, 1, 1, 1, 1, 2, 4, 8, 16, 64, 128, 256, 512};
static const uint8_t  APBpreFactorTable[8U] =  {1, 1, 1, 1, 2, 4, 8, 16};

uint32_t RCC_GetSysClk()
{
    uint32_t freq ;
    if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSI) {
        freq = HSI_CLK;
    } 
    else if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_HSE &&  (RCC->CR & RCC_CR_HSEON) == 1) {
        freq =  HSE_CLK;
    } 
    else if ((RCC->CFGR & RCC_CFGR_SWS) == RCC_CFGR_SWS_PLL && (RCC->CR & RCC_CR_PLLON) == 1) {
			  static const uint8_t  PLLMULFactorTable[16U] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 16};
				if(RCC->CFGR & RCC_CFGR_PLLSRC){		
						freq = (RCC->CFGR & RCC_CFGR_PLLXTPRE) ? (HSE_CLK >> 1) : HSE_CLK;		
				}
				else{
						freq = 4000000ul;
				}
				freq *= PLLMULFactorTable[(uint32_t)RCC->CFGR & RCC_CFGR_PLLMULL];
    } 
    else {
        return -1;
    }
    return freq;
}

uint32_t RCC_GetHClk()
{		
    return RCC_GetSysClk() / AHBpreFactorTable[RCC->CFGR & RCC_CFGR_HPRE];
}

uint32_t RCC_GetAbp1Clk()
{
    return RCC_GetHClk() / APBpreFactorTable[RCC->CFGR & RCC_CFGR_PPRE1];
}

uint32_t RCC_GetAbp2Clk()
{
    return RCC_GetHClk() / APBpreFactorTable[RCC->CFGR & RCC_CFGR_PPRE2];
}