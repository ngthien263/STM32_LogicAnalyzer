#include "stm32f10x.h"
typedef enum 
{                 
    GPIO_INPUT_MODE_ANALOG  = (0<<2) | 0,  // 0b0000
    GPIO_INPUT_MODE_FLOAT   = (1<<2) | 0,  // 0b0100
    GPIO_INPUT_MODE_PuPd    = (2<<2) | 0,  // 0b1000

    GPIO_OUTPUT_MODE_10M_PP = (0<<2) | 1,  // 0b0001
    GPIO_OUTPUT_MODE_10M_OD = (1<<2) | 1,  // 0b0101
    GPIO_ALT_MODE_10M_PP    = (2<<2) | 1,  // 0b1001
    GPIO_ALT_MODE_10M_OD    = (3<<2) | 1,  // 0b1101

    GPIO_OUTPUT_MODE_2M_PP  = (0<<2) | 2,  // 0b0010;
    GPIO_OUTPUT_MODE_2M_OD  = (1<<2) | 2,  // 0b0110
    GPIO_ALT_MODE_2M_PP     = (2<<2) | 2,  // 0b1010
    GPIO_ALT_MODE_2M_OD     = (3<<2) | 2,  // 0b1110

    GPIO_OUTPUT_MODE_50M_PP = (0<<2) | 3,  // 0b0011
    GPIO_OUTPUT_MODE_50M_OD = (1<<2) | 3,  // 0b0111
    GPIO_ALT_MODE_50M_PP    = (2<<2) | 3,  // 0b1011
    GPIO_ALT_MODE_50M_OD    = (3<<2) | 3  // 0b1111
} GPIO_MODE;


typedef enum {
    PIN0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7,
    PIN8, PIN9, PIN10, PIN11, PIN12, PIN13, PIN14, PIN15
} GPIO_PIN;

void GPIO_SetMode(volatile GPIO_TypeDef *GPIOx, GPIO_PIN Pin, GPIO_MODE Mode)
{
    if(Pin < 8)
    {
        GPIOx->CRL &= ~(0xF << (Pin * 4u));
        GPIOx->CRL |= Mode << (Pin * 4u);
    }
    else if(Pin >= 8 )
    {
        GPIOx->CRH &= ~(0xF << ((Pin - 8) * 4u));
        GPIOx->CRH |= Mode << ((Pin - 8) * 4u);
    }
}