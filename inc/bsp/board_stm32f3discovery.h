#ifndef _BOARD_STM32F3DISCOVERY
#define _BOARD_STM32F3DISCOVERY

// definition of resoures on atu first STM32F303 discovery board
// headers must be included manualy in C/CPP file

// default LEDS is E8:E15
#define BOARD_N_LEDS 8
#define BOARD_LEDS_GPIO GPIOE
#define BOARD_LEDS_OFS  8
#define BOARD_LEDS_MASK 0xFF00
// unshifted
#define BOARD_LEDS_ALL  0x00FF

#define BOARD_DEFINE_LEDS PinsOut leds( BOARD_LEDS_GPIO, BOARD_LEDS_OFS, BOARD_N_LEDS );

// mini LEDS is E12:E15
#define BOARD_N_LEDS_MINI 4

#define BOARD_DEFINE_LEDS_MINI PinsOut leds( BOARD_LEDS_GPIO, 12, BOARD_N_LEDS_MINI );

#define LED_BSP_BLUE         1
#define LED_BSP_BLUE_0       1
#define LED_BSP_RED          2
#define LED_BSP_RED_0        2
#define LED_BSP_ORANGE       4
#define LED_BSP_ORANGE_0     4
#define LED_BSP_GREEN        8
#define LED_BSP_GREEN_0      8

#define LED_BSP_BLUE_1    0x10
#define LED_BSP_RED_1     0x20
#define LED_BSP_ORANGE_1  0x40
#define LED_BSP_GREEN_1   0x80

#define LED_BSP_IDLE      LED_BSP_ORANGE_1
#define LED_BSP_TX        LED_BSP_RED_0
#define LED_BSP_RX        LED_BSP_GREEN_0
#define LED_BSP_ERR       LED_BSP_RED_1


#define TIM_EXA        TIM8
#define TIM_EXA_STR    "TIM8"
#define TIM_EXA_GPIO   GPIOC
#define TIM_EXA_PINS   (GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9)
#define TIM_EXA_CLKEN  __GPIOC_CLK_ENABLE(); __TIM8_CLK_ENABLE();
#define TIM_EXA_CLKDIS __TIM8_CLK_DISABLE();
#define TIM_EXA_GPIOAF GPIO_AF4_TIM8
#define TIM_EXA_IRQ    TIM8_CC_IRQn

#endif
