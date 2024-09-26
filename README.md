# STM32_LogicAnalyzer
## Project Overview
This project involves creating a logic analyzer using the STM32F103 microcontroller. The analyzer reads input data, handles PWM signals, and displays the captured data on a GUI.

## Features
- Real-time data capture from multiple channels
- PWM signal handling
- GUI for data visualization
  
## Pin Configuration
  - Timer:     
    - **PA0**: TIM2_CH1 (Input Capture)
    - **PA1**: TIM2_CH2 (Input Capture)
    - **PA2**: TIM2_CH3 (Input Capture)
    - **PA3**: TIM2_CH4 (Input Capture)
  - ADC:
    - **PA4**: ADC1_IN4
    - **PA5**: ADC1_IN5
  - Communication:
    - **PA9**: USART1_TX (UART Communication)
    - **PA10**: USART1_RX (UART Communication)
    - **PA11**: USB_DM (USB Communication)
    - **PA12**: USB_DP (USB Communication)
![image](https://github.com/user-attachments/assets/99c66cb7-2c73-4397-8a55-2bcbb6b4d0a1)

