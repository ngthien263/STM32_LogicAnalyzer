#include "main.h"

// Interrupt Service Routine for Timer 2
void TIM2_IRQHandler() {
    if (TIM2->SR & TIM_SR_CC1IF) { // Check for capture/compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF; // Clear the interrupt flag
        IC1Val = TIM2->CCR1; // Read the captured value
        if (stringSent) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, '1'); // Send '1' via UART
        }
    } else if (TIM2->SR & TIM_SR_CC2IF) { // Check for capture/compare 2 interrupt flag
        TIM2->SR &= ~TIM_SR_CC2IF; // Clear the interrupt flag
        IC2Val = TIM2->CCR2; // Read the captured value
        if (stringSent) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, '0'); // Send '0' via UART
        }
    }

    // Calculate duty cycle and frequency
    Duty = IC2Val * 100 / IC1Val;
    Freq = (uint32_t)(RCC_GetAbp1Clk() / (IC1Val * (TIM2->PSC + 1)));

    // Updatefrequency or duty cycle if change 
    if (old_freq != Freq || old_duty != (uint32_t)Duty) {
        update = 1; // Set update flag
        stringSent = 0; // Reset stringSent flag
    }
}

int main() {
    // Configure GPIO and timers
    TIM1_PWMOC(20000, 30);
    TIM2_PWMIC();

    char buffer[50];
    int check = 0;

    while (1) {
        if (update == 1) { // If update flag is set
            old_freq = Freq; // Update old frequency
            old_duty = Duty; // Update old duty cycle
            update = 0; // Reset update flag
            sprintf(buffer, "F:%uD:%u\n", Freq, Duty); // Format frequency and duty cycle string
            USART_str(USART1, (unsigned char *)buffer); // Send string via UART
            check++;
        }
        if (check >= 2) { // After sending the string twice
            check = 0; // Reset check counter
            stringSent = 1; // Set stringSent flag
        }
    }
    return 0;
}

//Configure USART1
void USART1_Config(uint32_t BAUD_RATE)
{
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
    GPIO_SetMode(GPIOA, 6, GPIO_ALT_MODE_50M_PP);
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

// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC() {
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}