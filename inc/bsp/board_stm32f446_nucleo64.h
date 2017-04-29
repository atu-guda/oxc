#ifndef _BOARD_STM32F446_NUCLEO64_H
#define _BOARD_STM32F446_NUCLEO64_H

// definition of resoures STM32F446R nucleo 64 board
// headers must be included manualy in C/CPP file

// default: single LED on A5
#define BOARD_N_LEDS_MINI 1
#define BOARD_LEDS_GPIO_MINI GPIOA
#define BOARD_LEDS_OFS_MINI  5
#define BOARD_LEDS_MASK_MINI 0x0020
// unshifted
#define BOARD_LEDS_ALL_MINI  0x01

// not so-extra LEDS is C0:C3
#define BOARD_N_LEDS 4
#define BOARD_LEDS_GPIO GPIOC
#define BOARD_LEDS_OFS  0
#define BOARD_LEDS_MASK 0x000F
// unshifted
#define BOARD_LEDS_ALL  0x0F

#define BOARD_DEFINE_LEDS PinsOut leds( BOARD_LEDS_GPIO, BOARD_LEDS_OFS, BOARD_N_LEDS );
#define BOARD_DEFINE_LEDS_MINI PinsOut leds( BOARD_LEDS_GPIO_MINI, BOARD_LEDS_OFS_MINI, BOARD_N_LEDS_MINI );

#define LED_BSP_YELLOW_MINI 1

#define LED_BSP_RED         1
#define LED_BSP_RED         1
#define LED_BSP_RED_0       1
#define LED_BSP_YELLOW      2
#define LED_BSP_YELLOW_0    2
#define LED_BSP_GREEN       4
#define LED_BSP_GREEN_0     4
#define LED_BSP_BLUE        8
#define LED_BSP_BLUE_0      8

#define LED_BSP_IDLE      LED_BSP_BLUE
#define LED_BSP_TX        LED_BSP_RED
#define LED_BSP_RX        LED_BSP_GREEN
#define LED_BSP_ERR       LED_BSP_YELLOW


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

#define SD_EXA_CK_GPIO   GPIOB
#define SD_EXA_CK_PIN    GPIO_PIN_2
#define SD_EXA_D0_GPIO   GPIOC
#define SD_EXA_D0_PIN    GPIO_PIN_8
#define SD_EXA_CMD_GPIO  GPIOD
#define SD_EXA_CMD_PIN   GPIO_PIN_2
#define SD_EXA_CLKEN     __HAL_RCC_SDIO_CLK_ENABLE();  __HAL_RCC_GPIOB_CLK_ENABLE();  __HAL_RCC_GPIOC_CLK_ENABLE();  __HAL_RCC_GPIOD_CLK_ENABLE();
#define SD_EXA_CLKDIS    __HAL_RCC_SDIO_CLK_DISABLE();
#define SD_EXA_GPIOAF    GPIO_AF12_SDIO

#endif