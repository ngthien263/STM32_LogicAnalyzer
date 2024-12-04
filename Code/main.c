#include "main.h"
char signal = 0;
<<<<<<< HEAD

void SystemInit()
	{
		}
// Interrupt Service Routine for Timer 2
void TIM2_IRQHandler() {
		static char lastByte;
		//static uint32_t lastTime = 0;
    if (TIM2->SR & TIM_SR_CC1IF) { // Check for capture/compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF; // Clear the interrupt flag
        IC1Val = TIM2->CCR1; // Read the captured value
				if (stringSent) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, '1'); // Send '1' via UART
=======
// Interrupt Service Routine for Timer 2
void TIM2_IRQHandler() {
		static char lastByte;
		static uint32_t lastTime = 0;
    if (TIM2->SR & TIM_SR_CC1IF) { // Check for capture/compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF; // Clear the interrupt flag
        IC1Val = TIM2->CCR1; // Read the captured value
				lastByte = '1';
        if (stringSent) { // If the frequency and duty cycle string has been sent
            USART_send(USART1, lastByte); // Send '1' via UART
>>>>>>> 6a48bd538cc8e77f11f37d035bc7dad0103e400e
        }
        
    } else if (TIM2->SR & TIM_SR_CC2IF) { // Check for capture/compare 2 interrupt flag
        TIM2->SR &= ~TIM_SR_CC2IF; // Clear the interrupt flag
        IC2Val = TIM2->CCR2; // Read the captured value
        lastByte = '0';
        if (stringSent) { // If the frequency and duty cycle string has been sent
<<<<<<< HEAD
            USART_send(USART1, '0'); // Send '1' via UART
=======
            USART_send(USART1, lastByte); // Send '0' via UART
        }
    }
		
		if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF; // Clear the interrupt flag
        uint32_t currentTime = TIM2->CNT;
        
        // Check if there has been no activity for a set time (e.g., 10ms)
        if (currentTime - lastTime > 10) {
            if (stringSent) { // If the frequency and duty cycle string has been sent
                USART_send(USART1, lastByte); // Send last recorded byte via UART when no pulse input
            }
            lastTime = currentTime; // Update last activity time
>>>>>>> 6a48bd538cc8e77f11f37d035bc7dad0103e400e
        }
    }

    // Calculate duty cycle and frequency
    Duty = IC2Val * 100 / IC1Val;
    Freq = (uint32_t)(RCC_GetAbp1Clk() / (IC1Val * (TIM2->PSC + 1)));
    // Updatefrequency or duty cycle if change 
<<<<<<< HEAD
=======
    if (old_freq != Freq || old_duty != (uint32_t)Duty) {
        update++; // Set update flag
        stringSent = 0; // Reset stringSent flag
    }
>>>>>>> 6a48bd538cc8e77f11f37d035bc7dad0103e400e
}

void clearCharArray(char* str) {
    for(int i = 0; str[i] != '\0'; i++) {
        str[i] = '\0';
    }
}
int main() {
    // Configure GPIO and timers  
    TIM1_PWMOC(10, 50);
		USART1_Config(115200);
    TIM2_PWMIC();
    char buffer[50];
<<<<<<< HEAD
		char received_data;

    while (1) {
			
//				received_data = USART_receive(USART1);
//					if (received_data == '1') {
//							update = 1;				
////							//USART_send(USART1, 'D'); // G?i ký t? debug khi nh?n du?c '1'
//					}
				
        	if (update == 1) { // If update flag is set
						old_freq = Freq; // Update old frequency
						old_duty = Duty; // Update old duty cycle
						update = 0; // Reset update flag
						//sprintf(buffer, "F:%u\nD:%u\n", Freq, Duty); // Format frequency and duty cycle string
						USART_str(USART1, (unsigned char *)"abc"); // Send string via UART
						clearCharArray(buffer);
						stringSent = 1;
					} 
				}
=======
    while (1) {
			if (update == 4) { // If update flag is set
					old_freq = Freq; // Update old frequency
					old_duty = Duty; // Update old duty cycle
					update = 0; // Reset update flag
					sprintf(buffer, "F:%u\nD:%u\n", Freq, Duty); // Format frequency and duty cycle string
					USART_str(USART1, (unsigned char *)buffer); // Send string via UART
					clearCharArray(buffer);
					stringSent = 1;
			} 
		}
>>>>>>> 6a48bd538cc8e77f11f37d035bc7dad0103e400e
    return 0;
}
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

// Initialize PWM input capture for Timer 2 function
void TIM2_PWMIC() {
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}