#ifndef _BOARD_STM32F446_NUCLEO64_H
#define _BOARD_STM32F446_NUCLEO64_H

#define _BOARD_CFG_DEFINED

// definition of resoures STM32F446R nucleo 64 board
// headers must be included manualy in C/CPP file

#define def_stksz 512


// default: single LED on A5
#define BOARD_N_LEDS_MINI 1
#define BOARD_LEDS_GPIO_MINI GPIOA
#define BOARD_LEDS_OFS_MINI  5
#define BOARD_LEDS_MASK_MINI 0x0020
// unshifted
#define BOARD_LEDS_ALL_MINI  0x01

// not so-extra LEDS is C0:C3
#define BOARD_N_LEDS 4
#define BOARD_LEDS_GPIOX C
#define BOARD_LEDS_OFS  0

#define BOARD_DEFINE_LEDS_MINI PinsOut leds( BOARD_LEDS_GPIO_MINI, BOARD_LEDS_OFS_MINI, BOARD_N_LEDS_MINI );

#define LED_BSP_YELLOW_MINI 1

#define LED_BSP_RED       1
#define LED_BSP_RED_0     1
#define LED_BSP_YELLOW    2
#define LED_BSP_YELLOW_0  2
#define LED_BSP_GREEN     4
#define LED_BSP_GREEN_0   4
#define LED_BSP_BLUE      8
#define LED_BSP_BLUE_0    8

#define LED_BSP_IDLE      LED_BSP_BLUE
#define LED_BSP_TX        LED_BSP_RED
#define LED_BSP_RX        LED_BSP_GREEN
#define LED_BSP_ERR       LED_BSP_YELLOW

#define BOARD_BTN0_EXIST   1
#define BOARD_BTN0_GPIOX   C
#define BOARD_BTN0_N       13
#define BOARD_BTN0_ACTIVE_DOWN 0
#define BOARD_BTN0_IRQNAME  EXTI15_10


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
#define TIM_EXA_IRQ    TIM1_CC_IRQn
#define TIM_EXA_IRQHANDLER    TIM1_CC_IRQHandler

#define SD_EXA_CK_GPIO   GPIOB
#define SD_EXA_CK_PIN    GPIO_PIN_2
#define SD_EXA_D0_GPIO   GPIOC
#define SD_EXA_D0_PIN    GPIO_PIN_8
#define SD_EXA_CMD_GPIO  GPIOD
#define SD_EXA_CMD_PIN   GPIO_PIN_2
#define SD_EXA_CLKEN     __HAL_RCC_SDIO_CLK_ENABLE();  __HAL_RCC_GPIOB_CLK_ENABLE();  __HAL_RCC_GPIOC_CLK_ENABLE();  __HAL_RCC_GPIOD_CLK_ENABLE();
#define SD_EXA_CLKDIS    __HAL_RCC_SDIO_CLK_DISABLE();
#define SD_EXA_GPIOAF    GPIO_AF12_SDIO

#define BOARD_UART_DEFAULT            USART2
#define BOARD_UART_DEFAULT_NAME       "USART2"
#define BOARD_UART_DEFAULT_GPIO       GPIOA
#define BOARD_UART_DEFAULT_GPIO_PINS  ( GPIO_PIN_2 | GPIO_PIN_3 )
#define BOARD_UART_DEFAULT_GPIO_AF    GPIO_AF7_USART2
#define BOARD_UART_DEFAULT_ENABLE     __USART2_CLK_ENABLE(); __GPIOA_CLK_ENABLE();
#define BOARD_UART_DEFAULT_DISABLE    __USART2_CLK_DISABLE();
#define BOARD_UART_DEFAULT_IRQ        USART2_IRQn
#define BOARD_UART_DEFAULT_IRQHANDLER USART2_IRQHandler

#define BOARD_I2C_DEFAULT               I2C1
#define BOARD_I2C_DEFAULT_NAME          "I2C1"
#define BOARD_I2C_DEFAULT_SPEED         100000
#define BOARD_I2C_DEFAULT_GPIO_SCL      GPIOB
#define BOARD_I2C_DEFAULT_GPIO_SDA      GPIOB
#define BOARD_I2C_DEFAULT_GPIO_PIN_SCL  GPIO_PIN_8
#define BOARD_I2C_DEFAULT_GPIO_PIN_SDA  GPIO_PIN_9
#define BOARD_I2C_DEFAULT_GPIO_AF       GPIO_AF4_I2C1
#define BOARD_I2C_DEFAULT_ENABLE        __I2C1_CLK_ENABLE(); __GPIOB_CLK_ENABLE();
#define BOARD_I2C_DEFAULT_DISABLE       __I2C1_CLK_DISABLE();
#define BOARD_I2C_DEFAULT_IRQ           I2C1_EV_IRQn
#define BOARD_I2C_DEFAULT_IRQHANDLER    I2C1_EV_IRQHandler


#define BOARD_SPI_DEFAULT               SPI2
#define BOARD_SPI_DEFAULT_NAME          "SPI2"
#define BOARD_SPI_DEFAULT_GPIO_SCK      GPIOB
#define BOARD_SPI_DEFAULT_GPIO_PIN_SCK  GPIO_PIN_13
#define BOARD_SPI_DEFAULT_GPIO_MISO     GPIOB
#define BOARD_SPI_DEFAULT_GPIO_PIN_MISO GPIO_PIN_14
#define BOARD_SPI_DEFAULT_GPIO_MOSI     GPIOB
#define BOARD_SPI_DEFAULT_GPIO_PIN_MOSI GPIO_PIN_15
#define BOARD_SPI_DEFAULT_GPIO_SNSS     GPIOB
// here number, as input to PinsOut
#define BOARD_SPI_DEFAULT_GPIO_PIN_SNSS 1
#define BOARD_SPI_DEFAULT_GPIO_EXT1     GPIOB
#define BOARD_SPI_DEFAULT_GPIO_PIN_EXT1 4
#define BOARD_SPI_DEFAULT_GPIO_EXT2     GPIOB
#define BOARD_SPI_DEFAULT_GPIO_PIN_EXT2 5
#define BOARD_SPI_DEFAULT_GPIO_AF       GPIO_AF5_SPI2
#define BOARD_SPI_DEFAULT_ENABLE        __SPI2_CLK_ENABLE(); __GPIOB_CLK_ENABLE();
#define BOARD_SPI_DEFAULT_DISABLE       __SPI2_CLK_DISABLE();
#define BOARD_SPI_DEFAULT_IRQ           SPI2_IRQn
#define BOARD_SPI_DEFAULT_IRQHANDLER    SPI2_IRQHandler


#define BOARD_MOTOR_DEFAULT_GPIO        GPIOA
#define BOARD_MOTOR_DEFAULT_PIN0        8

#define BOARD_1W_DEFAULT_GPIO           GPIOC
#define BOARD_1W_DEFAULT_PIN            GPIO_PIN_4

// A0, A1, A6, A7 (0,1,5,7)
#define BOARD_ADC_DEFAULT_DEV           ADC1
#define BOARD_ADC_DEFAULT_EN            __HAL_RCC_ADC1_CLK_ENABLE();
#define BOARD_ADC_DEFAULT_DIS           __HAL_RCC_ADC1_CLK_DISABLE();
#define BOARD_ADC_DEFAULT_GPIO0         GPIOA
#define BOARD_ADC_DEFAULT_PIN0          GPIO_PIN_0
#define BOARD_ADC_DEFAULT_CH0           ADC_CHANNEL_0
#define BOARD_ADC_DEFAULT_GPIO1         GPIOA
#define BOARD_ADC_DEFAULT_PIN1          GPIO_PIN_1
#define BOARD_ADC_DEFAULT_CH1           ADC_CHANNEL_1
#define BOARD_ADC_DEFAULT_GPIO2         GPIOA
#define BOARD_ADC_DEFAULT_PIN2          GPIO_PIN_6
#define BOARD_ADC_DEFAULT_CH2           ADC_CHANNEL_6
#define BOARD_ADC_DEFAULT_GPIO3         GPIOA
#define BOARD_ADC_DEFAULT_PIN3          GPIO_PIN_7
#define BOARD_ADC_DEFAULT_CH3           ADC_CHANNEL_7
#define BOARD_ADC_MEM_MAX               (1024*64)

#define BOARD_USB_DEFAULT_GPIO       GPIOA
#define BOARD_USB_DEFAULT_DPDM_PINS  ( GPIO_PIN_11 | GPIO_PIN_12 )
#define BOARD_USB_DEFAULT_VBUS_PIN   GPIO_PIN_9
#define BOARD_USB_DEFAULT_ID_PIN     GPIO_PIN_10
#define BOARD_USB_DEFAULT_GPIO_AF    GPIO_AF10_OTG_FS
#define BOARD_USB_DEFAULT_ENABLE     __GPIOA_CLK_ENABLE(); __HAL_RCC_USB_OTG_FS_CLK_ENABLE(); __HAL_RCC_SYSCFG_CLK_ENABLE();
#define BOARD_USB_DEFAULT_DISABLE    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
#define BOARD_USB_DEFAULT_IRQ        OTG_FS_IRQn
#define BOARD_USB_DEFAULT_IRQHANDLER OTG_FS_IRQHandler
#define BOARD_USB_DEFAULT_IRQ_PRTY   14

#define BOARD_CONSOLE_DEFINES         UART_CONSOLE_DEFINES( USART2 );
#define BOARD_CONSOLE_DEFINES_UART    UART_CONSOLE_DEFINES( USART2 );
#define BOARD_PROLOG                  STD_PROLOG_UART;
#define BOARD_CREATE_STD_TASKS        CREATE_STD_TASKS;
#define BOARD_POST_INIT_BLINK         delay_ms( PROLOG_LED_TIME ); leds.write( 0x00 ); delay_ms( PROLOG_LED_TIME );

#endif
