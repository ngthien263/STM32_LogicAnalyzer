#include "main.h"

char signal = 0;
// Interrupt Service Routine for Timer 2
void TIM2_IRQHandler() {
    static char lastByte_ch1;
    static uint16_t overflow_cnt = 0;
		static char ovf = 0;
    if (TIM2->SR & TIM_SR_CC1IF) { // Check for capture/compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF; // Clear the interrupt flag
        IC1Val_ch1 = TIM2->CCR1 + overflow_cnt * 0xFFFF; // Read the captured value with overflow count
			  ovf++;
        lastByte_ch1 = '1';
        if (stringSent_ch1) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, '1'); // Send '1' via UART
        }
    } else if (TIM2->SR & TIM_SR_CC2IF) { // Check for capture/compare 2 interrupt flag
        TIM2->SR &= ~TIM_SR_CC2IF; // Clear the interrupt flag
        IC2Val_ch1 = TIM2->CCR2 + overflow_cnt * 0xFFFF; // Read the captured value with overflow count
				ovf++;
        lastByte_ch1 = '0';
        if (stringSent_ch1) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, lastByte_ch1); // Send '0' via UART
        }
    }
		if (ovf == 2) {
			overflow_cnt = 0; // Reset overflow count
			ovf = 0;
		}
			
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Clear the interrupt flag
        overflow_cnt++; // Increment overflow count
    }

    // Calculate duty cycle and frequency
    if (IC1Val_ch1 != 0) {
        Duty_ch1 = (IC2Val_ch1 - overflow_cnt*0xFFFF) * 100 / (IC1Val_ch1 - overflow_cnt*0xFFFF);
        Freq_ch1 = (uint32_t)(RCC_GetAbp1Clk() / ((IC1Val_ch1 - overflow_cnt*0xFFFF) * (TIM2->PSC + 1)));
				IC1Val_ch1 = 0;
			IC2Val_ch1 = 0;
        // Update frequency or duty cycle if changed 
        if (old_freq_ch1 != Freq_ch1 || old_duty_ch1 != (uint32_t)Duty_ch1) {
            update_ch1 = 1; // Set update flag
            stringSent_ch1 = 0; // Reset stringSent flag
        }
				
//				if (old_freq_ch1 == Freq_ch1 && old_duty_ch1 == (uint32_t)Duty_ch1) {
//            update_ch1 = 0; // Set update flagg
//        }
    }
}

void TIM3_IRQHandler() {
    static char lastByte_ch2;
    static uint16_t overflow_cnt = 0;
		static char ovf = 0;
    if (TIM3->SR & TIM_SR_CC1IF) { // Check for capture/compare 1 interrupt flag
        TIM3->SR &= ~TIM_SR_CC1IF; // Clear the interrupt flag
        IC1Val_ch2 = TIM3->CCR1 + overflow_cnt * 0xFFFF; // Read the captured value with overflow count
			  ovf++;
        lastByte_ch2 = '1';
        if (stringSent_ch2) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, '1'); // Send '1' via UART
        }
    } else if (TIM3->SR & TIM_SR_CC2IF) { // Check for capture/compare 2 interrupt flag
        TIM3->SR &= ~TIM_SR_CC2IF; // Clear the interrupt flag
        IC2Val_ch2 = TIM3->CCR2 + overflow_cnt * 0xFFFF; // Read the captured value with overflow count
				ovf++;
        lastByte_ch2 = '0';
        if (stringSent_ch2) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, lastByte_ch2); // Send '0' via UART
        }
    }
		if (ovf == 2) {
			overflow_cnt = 0; // Reset overflow count
			ovf = 0;
		}
			
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF; // Clear the interrupt flag
        overflow_cnt++; // Increment overflow count
    }

    // Calculate duty cycle and frequency
    if (IC1Val_ch2 != 0) {
        Duty_ch2 = (IC2Val_ch2 - overflow_cnt*0xFFFF) * 100 / (IC1Val_ch2 - overflow_cnt*0xFFFF);
        Freq_ch2 = (uint32_t)(RCC_GetAbp1Clk() / ((IC1Val_ch2 - overflow_cnt*0xFFFF) * (TIM2->PSC + 1)));
				IC1Val_ch2 = 0;
				IC2Val_ch2 = 0;
        // Update frequency or duty cycle if changed 
        if (old_freq_ch2 != Freq_ch2 || old_duty_ch2 != (uint32_t)Duty_ch2) {
            update_ch2 = 1; // Set update flag
            stringSent_ch2 = 0; // Reset stringSent flag
        }
				
//				if (old_freq_ch2 == Freq_ch2 && old_duty_ch2 == (uint32_t)Duty_ch2) {
//            update_ch2 = 0; // Set update flagg
//        }
    }
}

int main() {
    // Configure GPIO and timers  
    TIM1_PWMOC(10, 70);
    USART1_Config(115200);
    TIM2_PWMIC();
    TIM3_PWMIC();
    char buffer[50];
    while (1) {
        if (update_ch1 == 1) { // If update flag is set
            old_freq_ch1 = Freq_ch1; // Update old frequency
            old_duty_ch1 = Duty_ch1; // Update old duty cycle
            update_ch1 = 0; // Reset update flag
            sprintf(buffer, "NF:%u\nD:%u\n", Freq_ch1, Duty_ch1); // Format frequency and duty cycle string
            USART_str(USART1, (unsigned char *)buffer); // Send string via UART
            //clearCharArray(buffer);
            stringSent_ch1 = 1;
        } 
        if(update_ch1 > 1) update_ch1 = 0;
        if (update_ch2 == 1) { // If update flag is set
            old_freq_ch2 = Freq_ch2; // Update old frequency
            old_duty_ch2 = Duty_ch2; // Update old duty cycle
            update_ch2 = 0; // Reset update flag
            sprintf(buffer, "NF:%u\nD:%u\n", Freq_ch2, Duty_ch2); // Format frequency and duty cycle string
            USART_str(USART1, (unsigned char *)buffer); // Send string via UART
            //clearCharArray(buffer);
            stringSent_ch2 = 1;
        } 
				if(update_ch2 > 1) update_ch2 = 0;
    }
    return 0;
}

// Other supporting functions remain the same


//Configure USART1
void USART1_Config(uint32_t BAUD_RATE)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	// Initialize UART1
  USART_Init(USART1, BAUD_RATE);
	// Configure GPIO for UART
  GPIO_SetMode(GPIOA, 9, GPIO_ALT_MODE_50M_PP);
  GPIO_SetMode(GPIOA, 10, GPIO_INPUT_MODE_FLOAT);
	// UART1 Enable
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

// Configure GPIO for Timer 1 and UART
void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1 function
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

// Configure GPIO for Timer 2
void TIM2_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_FLOAT);
}
void TIM3_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 6, GPIO_INPUT_MODE_FLOAT);
}
// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC() {
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM3_PWMIC() {
    TIM3_PinConfig();
    TIM_PWMICMInit(TIM3);
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}