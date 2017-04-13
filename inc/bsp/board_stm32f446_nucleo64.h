#ifndef _BOARD_STM32F446_NUCLEO64_H
#define _BOARD_STM32F446_NUCLEO64_H

// definition of resoures STM32F446R nucleo 64 board
// headers must be included manualy in C/CPP file

// default: single LED on A5
#define BOARD_N_LEDS 1
#define BOARD_LEDS_GPIO GPIOA
#define BOARD_LEDS_OFS  5
#define BOARD_LEDS_MASK 0x0020
// unshifted
#define BOARD_LEDS_ALL  0x01

// extra LEDS is C0:C3
#define BOARD_N_LEDS_EX 4
#define BOARD_LEDS_GPIO_EX GPIOC
#define BOARD_LEDS_OFS_EX  0
#define BOARD_LEDS_MASK_EX 0x000F
// unshifted
#define BOARD_LEDS_ALL_EX  0x0F

#define BOARD_DEFINE_LEDS PinsOut leds( BOARD_LEDS_GPIO, BOARD_LEDS_OFS, BOARD_N_LEDS );
#define BOARD_DEFINE_LEDS_EX PinsOut leds( BOARD_LEDS_GPIO_EX, BOARD_LEDS_OFS_EX, BOARD_N_LEDS_EX );

#define LED_BSP_YELLOW    1

#define LED_BSP_RED_EX      1
#define LED_BSP_RED_EX      1
#define LED_BSP_RED_0_EX    1
#define LED_BSP_YELLOW_EX   2
#define LED_BSP_YELLOW_0_EX 2
#define LED_BSP_GREEN_EX    4
#define LED_BSP_GREEN_0_EX  4
#define LED_BSP_BLUE_EX     8
#define LED_BSP_BLUE_0_EX   8

#define LED_BSP_IDLE      LED_BSP_BLUE_EX
#define LED_BSP_TX        LED_BSP_RED_EX
#define LED_BSP_RX        LED_BSP_GREEN_EX
#define LED_BSP_ERR       LED_BSP_YELLOW_EX


#define TIM_EXA        TIM1
#define TIM_EXA_STR    "TIM1"
#define TIM_EXA_GPIO   GPIOA
#define TIM_EXA_PIN1   GPIO_PIN_8
#define TIM_EXA_PIN2   GPIO_PIN_9
#define TIM_EXA_PIN3   GPIO_PIN_10
#define TIM_EXA_PIN4   GPIO_PIN_11
#define TIM_EXA_PINS   ( TIM_EXA_PIN1 | TIM_EXA_PIN2 | TIM_EXA_PIN3 | TIM_EXA_PIN4 )
#define TIM_EXA_CLKEN  __GPIOA_CLK_ENABLE(); __TIM1_CLK_ENABLE();
#define TIM_EXA_CLKDIS __TIM1_CLK_DISABLE();
#define TIM_EXA_GPIOAF GPIO_AF1_TIM1
#define TIM_EXA_IRQ    TIM1_BRK_TIM9_IRQn


#endif
