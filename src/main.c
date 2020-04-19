#define LED_GREEN_ON() GPIO_SetBits(GPIOD, GPIO_Pin_12)
#define LED_GREEN_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_12)
#define LED_YELLOW_ON() GPIO_SetBits(GPIOD, GPIO_Pin_13)
#define LED_YELLOW_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_13)
#define LED_RED_ON() GPIO_SetBits(GPIOD, GPIO_Pin_14)
#define LED_RED_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_14)
#define LED_BLUE_ON() GPIO_SetBits(GPIOD, GPIO_Pin_15)
#define LED_BLUE_OFF() GPIO_ResetBits(GPIOD, GPIO_Pin_15)

#define GREEN_MODE 0u
#define YELLOW_MODE 1u
#define RED_MODE 2u
#define BLUE_MODE 3u

#define USER_BUTTON_READ GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)

#define DELAY 250u

#include "stm32f4xx.h"

#include <stdio.h>

uint16_t delay_count = 0u;
uint8_t mode = 0u;

char buf[100];
uint8_t sendCount = 0u;

void SysTick_Handler(void) {
    if (delay_count > 0u) {
        --delay_count;
    }
}

void delay_ms(uint16_t delay_value_ms) {
    delay_count = delay_value_ms;
    while (delay_count) {}
}

void Timer_init() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    SysTick_Config(SystemCoreClock/1000);
    return;
}

void LEDs_init() {
    GPIO_InitTypeDef GPIO_Init_LED;
    
    GPIO_Init_LED.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init_LED.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init_LED.GPIO_OType = GPIO_OType_PP;
    GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOD, &GPIO_Init_LED);
    return;
}

void user_button_init() {
    GPIO_InitTypeDef GPIO_Init_UserButton;
    
    GPIO_Init_UserButton.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init_UserButton.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init_UserButton.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init_UserButton.GPIO_OType = GPIO_OType_PP;
    GPIO_Init_UserButton.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(GPIOA, &GPIO_Init_UserButton);
}

void USART_ini() {
    GPIO_InitTypeDef GPIO_Init_USART;
    USART_InitTypeDef USART_InitUser;
    
    GPIO_Init_USART.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init_USART.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init_USART.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init_USART.GPIO_OType = GPIO_OType_PP;
    GPIO_Init_USART.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(GPIOA, &GPIO_Init_USART);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    USART_InitUser.USART_BaudRate = 9600;
    USART_InitUser.USART_WordLength = USART_WordLength_8b;
    USART_InitUser.USART_StopBits = USART_StopBits_1;
    USART_InitUser.USART_Parity = USART_Parity_No;
    USART_InitUser.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitUser.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART2, &USART_InitUser);
    NVIC_EnableIRQ(USART2_IRQn);
    USART_Cmd(USART2, ENABLE);
}

void USART2_IRQHandler(void) {
    if (USART_GetItStatus(USART2, USART_IT_TXE) == SET) {
        USART_ClearITPendingBit(USART2, USART_IT_TXE);

        if (buf[sendCount] != 0) {
            USART_SendData(USART2, buf[sendCount]);
            ++sendCount;
        }
        else {
            sendCount = 0u;
            USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
        }
        
    }
}

void SendToPCWithInt() {
    USART_SendData(USART2, buf[sendCount]);
    ++sendCount;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void SendText(char const* str_p) {
    uint8_t i = 0u;
    while (str_p[i] != '\0') {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET) {}
        USART_SendData(USART2, str_p[i]);
        ++i;
    }
}

int main(void) { 
    Timer_init();
    USART_ini();
    sprintf(buf, "Hello world\n\r");

    while(1) {
        delay_ms(500);
        SendToPCWithInt();
    }
}