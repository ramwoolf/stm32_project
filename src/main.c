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

#define DELAY 250u

#include "stm32f4xx.h"

uint16_t delay_count = 0u;
uint8_t mode = 0u;

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

// void Set_mode()

int main(void) { 
    Timer_init();
    LEDs_init();

    while (1) {
        switch (mode) {
            case GREEN_MODE:
                LED_GREEN_ON();
                delay_ms(DELAY);
                LED_GREEN_OFF();
                break;
            case YELLOW_MODE:
                LED_YELLOW_ON();
                delay_ms(DELAY);
                LED_YELLOW_OFF();
                break;
            case RED_MODE:
                LED_RED_ON();
                delay_ms(DELAY);
                LED_RED_OFF();
                break;
            case BLUE_MODE:
                LED_BLUE_ON();
                delay_ms(DELAY);
                LED_BLUE_OFF();
                break;
        }
        (mode > 3u) ? (mode = 0u) : (++mode);
    }
}