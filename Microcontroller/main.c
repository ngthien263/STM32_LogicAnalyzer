#include "main.h"

typedef struct {
    volatile uint32_t IC1Val;
    volatile uint32_t IC2Val;
    volatile uint32_t Freq;
    volatile uint32_t Duty;
	  uint32_t oldFreq;
		uint32_t oldDuty;
	  volatile int stringSent;
		volatile int update;
} TimerChannel_typedef;

volatile TimerChannel_typedef Timer2Channel = {0};
volatile TimerChannel_typedef Timer3Channel = {0};

void USART1_IRQHandler(void) {
    if (USART1->SR & USART_SR_RXNE) { // Check if data is received
        NVIC_DisableIRQ(TIM2_IRQn);
        NVIC_DisableIRQ(TIM3_IRQn); // Temporarily disable interrupts for TIM2 and TIM3
        char receivedByte = USART1->DR; // Read received data
        if (receivedByte == '1') { // Command "Start" from Qt
            sendFreqDutyOnce = 1; 
            Timer2Channel.stringSent = 1;
            Timer3Channel.stringSent = 1;
        }
    }
}

void TIM2_IRQHandler(void) {
		static uint16_t ovf = 0;
		static uint16_t OverflowCount = 0;
		
		//Read rising and falling edge value from CNT
    if (TIM2->SR & TIM_SR_CC1IF) {
        TIM2->SR &= ~TIM_SR_CC1IF;
        Timer2Channel.IC1Val = TIM2->CCR1 + OverflowCount * 0xFFFF;
        ovf++;
				if(Timer2Channel.stringSent) {
					USART_send(USART1, '1');
				}
        
    } else if (TIM2->SR & TIM_SR_CC2IF) {
        TIM2->SR &= ~TIM_SR_CC2IF;
        Timer2Channel.IC2Val = TIM2->CCR2 + OverflowCount * 0xFFFF;
        ovf++;
        USART_send(USART1, '1');
    }
		
		// Reset overflow counters when two captures have occurred
    if (ovf == 2) {
        OverflowCount = 0;
        ovf = 0;
    }
		
		// Handle timer overflow
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR &= ~TIM_SR_UIF;
        OverflowCount++;
    }
		
		//Calculate Freq and Duty from Timer2
    if (Timer2Channel.IC1Val != 0) {
        Timer2Channel.Duty = (Timer2Channel.IC2Val - OverflowCount * 0xFFFF) * 100 /
														 (Timer2Channel.IC1Val - OverflowCount * 0xFFFF);
        Timer2Channel.Freq = (uint32_t)(RCC_GetAbp1Clk() / 
														 ((Timer2Channel.IC1Val - OverflowCount * 0xFFFF) * (TIM2->PSC + 1)));
        Timer2Channel.IC1Val = 0;
        Timer2Channel.IC2Val = 0;
    }
		
		//Check if input change
		if (Timer2Channel.oldFreq != Timer2Channel.Freq || Timer2Channel.oldDuty != (uint32_t)Timer2Channel.Duty) {
					Timer2Channel.update = 1;
					Timer2Channel.stringSent = 0; 
		}	
}

void TIM3_IRQHandler(void) {
		static uint16_t ovf = 0;
		static uint16_t OverflowCount = 0;
	
		//Read rising and falling edge value from CNT
    if (TIM3->SR & TIM_SR_CC1IF) {
        TIM3->SR &= ~TIM_SR_CC1IF;
        Timer3Channel.IC1Val = TIM3->CCR1 + OverflowCount * 0xFFFF;
        ovf++;
        if(Timer3Channel.stringSent) {
					USART_send(USART1, '2');
				}
        
    } else if (TIM3->SR & TIM_SR_CC2IF) {
        TIM3->SR &= ~TIM_SR_CC2IF;
        Timer3Channel.IC2Val = TIM3->CCR2 + OverflowCount * 0xFFFF;
        ovf++;
        USART_send(USART1, '2');
    }

		// Reset overflow counters when two captures have occurred
    if (ovf == 2) {
        OverflowCount = 0;
        ovf = 0;
    }
		
		// Handle timer overflow
    if (TIM3->SR & TIM_SR_UIF) {
        TIM3->SR &= ~TIM_SR_UIF;
        OverflowCount++;
    }
		
		//Calculate Freq and Duty from Timer2
    if (Timer3Channel.IC1Val != 0) {
        Timer3Channel.Duty = (Timer3Channel.IC2Val - OverflowCount * 0xFFFF) * 100 /
															(Timer3Channel.IC1Val - OverflowCount * 0xFFFF);
        Timer3Channel.Freq = (uint32_t)(RCC_GetAbp1Clk() / 
															((Timer3Channel.IC1Val - OverflowCount * 0xFFFF) * (TIM3->PSC + 1)));
        Timer3Channel.IC1Val = 0;
        Timer3Channel.IC2Val = 0;
    }
		
		//Check if input change
		if (Timer3Channel.oldFreq != Timer3Channel.Freq || Timer3Channel.oldDuty != (uint32_t)Timer3Channel.Duty) {
            Timer3Channel.update = 1; // Set update flag
            Timer3Channel.stringSent = 0; // Reset stringSent flag
        }
}

int main() {
    // Configure GPIO and timers  
    TIM1_PWMOC(15, 80);
		TIM4_PWMOC(10, 60);
		USART1_Config(115200);
		TIM2_PWMIC();
		TIM3_PWMIC();
		char buffer[30];
    while (1) {
			//Send Freq and Duty when there is signal from software
			if (sendFreqDutyOnce) {
					Timer2Channel.oldFreq = Timer2Channel.Freq; // Update old frequency
					Timer2Channel.oldDuty = Timer2Channel.Duty; // Update old duty cycle
					sprintf(buffer, "NC1F:%uD:%u\n", Timer2Channel.Freq, Timer2Channel.Duty);
					USART_str(USART1, (unsigned char *)buffer);
					memset(buffer, 0, sizeof(buffer));
					Timer3Channel.oldFreq = Timer3Channel.Freq; // Update old frequency
					Timer3Channel.oldDuty = Timer3Channel.Duty; // Update old duty cycle
					Timer3Channel.update = 0; // Reset update flag
					Timer2Channel.update = 0; // Reset update flag
					sprintf(buffer, "NC2F:%uD:%u\n", Timer3Channel.Freq, Timer3Channel.Duty);
					USART_str(USART1, (unsigned char *)buffer);
					memset(buffer, 0, sizeof(buffer));
					sendFreqDutyOnce = 0; 
					NVIC_EnableIRQ(TIM2_IRQn); 
					NVIC_EnableIRQ(TIM3_IRQn);
			}

			//Check if Timer2 input change
			if (Timer2Channel.update == 1) {
					Timer2Channel.oldFreq = Timer2Channel.Freq; // Update old frequency
					Timer2Channel.oldDuty = Timer2Channel.Duty; // Update old duty cycle
					sprintf(buffer, "NC1F:%uD:%u\n", Timer2Channel.Freq, Timer2Channel.Duty);
					USART_str(USART1, (unsigned char *)buffer);
					memset(buffer, 0, sizeof(buffer));
					Timer2Channel.update = 0;
					Timer2Channel.stringSent = 1;
			}
			
			//Check if Timer3 input change
			if (Timer3Channel.update == 1) {
					Timer3Channel.oldFreq = Timer3Channel.Freq; // Update old frequency
					Timer3Channel.oldDuty = Timer3Channel.Duty; // Update old duty cycle
					sprintf(buffer, "NC2F:%uD:%u\n", Timer3Channel.Freq, Timer3Channel.Duty);
					USART_str(USART1, (unsigned char *)buffer);
					memset(buffer, 0, sizeof(buffer));
					Timer3Channel.update = 0;
					Timer3Channel.stringSent = 1;
			}
	}
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
	USART1->CR1 |= USART_CR1_RXNEIE; //Enable RX interrupt
	// UART1 Enable
  USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
	NVIC_SetPriority(USART1_IRQn, 0); 
	NVIC_EnableIRQ(USART1_IRQn);
}

// Configure GPIO for Timer 1
void TIM1_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 8, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1
void TIM1_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM1_PinConfig();
    TIM_OC1_PWMM1_Init(TIM1, Frequency, Duty_Cycle);
}

// Initialize PWM output for Timer 4
void TIM4_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    GPIO_SetMode(GPIOB, 6, GPIO_ALT_MODE_50M_PP);
}

// Initialize PWM output for Timer 1 
void TIM4_PWMOC(uint32_t Frequency, uint32_t Duty_Cycle) {
    TIM4_PinConfig();
    TIM_OC1_PWMM1_Init(TIM4, Frequency, Duty_Cycle);
}

// Configure GPIO for Timer 2
void TIM2_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 0, GPIO_INPUT_MODE_FLOAT);
}

// Configure GPIO for Timer 
void TIM3_PinConfig() {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIO_SetMode(GPIOA, 6, GPIO_INPUT_MODE_FLOAT);
}

// Initialize PWM input capture for Timer 2 
void TIM2_PWMIC() {
    TIM2_PinConfig();
    TIM_PWMICMInit(TIM2);
    NVIC_SetPriority(TIM2_IRQn, 1);
    NVIC_EnableIRQ(TIM2_IRQn);
}

// Initialize PWM input capture for Timer 3
void TIM3_PWMIC() {
    TIM3_PinConfig();
    TIM_PWMICMInit(TIM3);
    NVIC_SetPriority(TIM3_IRQn, 1);
    NVIC_EnableIRQ(TIM3_IRQn);
}