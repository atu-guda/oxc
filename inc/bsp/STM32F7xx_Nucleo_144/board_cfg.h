#ifndef _BOARD_STM32F746_NUCLEO_H
#define _BOARD_STM32F746_NUCLEO_H

#define _BOARD_CFG_DEFINED

// definition of resoures on STM32F746IGT Nucleo144 board
// headers must be included manualy in C/CPP file

#define def_stksz 512


// main MY LEDS is D4:D7
#define BOARD_N_LEDS 4
#define BOARD_LEDS_GPIOX D
#define BOARD_LEDS_OFS  4

// extra is first on-board user LED B0, more : B7, B14
#define BOARD_N_LEDS_EXTRA 1

#define BOARD_DEFINE_LEDS_EXTRA PinsOut leds( GPIOB, 0, BOARD_N_LEDS_EXTRA );

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
#define LED_BSP_ERR       LED_BSP_BLUE

// TODO: fix
#define BOARD_BTN0_EXIST   1
#define BOARD_BTN0_GPIOX   C
#define BOARD_BTN0_N       13
#define BOARD_BTN0_ACTIVE_DOWN 0
#define BOARD_BTN0_IRQNAME  EXTI15_10



#define SD_EXA_CK_GPIO   GPIOC
#define SD_EXA_CK_PIN    GPIO_PIN_12
#define SD_EXA_D0_GPIO   GPIOC
#define SD_EXA_D0_PIN    GPIO_PIN_8
#define SD_EXA_CMD_GPIO  GPIOD
#define SD_EXA_CMD_PIN   GPIO_PIN_2
#define SD_EXA_CLKEN     __HAL_RCC_SDIO_CLK_ENABLE();  __HAL_RCC_GPIOC_CLK_ENABLE();  __HAL_RCC_GPIOD_CLK_ENABLE();
#define SD_EXA_CLKDIS    __HAL_RCC_SDIO_CLK_DISABLE();
#define SD_EXA_GPIOAF    GPIO_AF12_SDIO

#define BOARD_UART_DEFAULT            USART3
#define BOARD_UART_DEFAULT_GPIO       GPIOD
#define BOARD_UART_DEFAULT_GPIO_PINS  ( GPIO_PIN_8 | GPIO_PIN_9 )
#define BOARD_UART_DEFAULT_GPIO_AF    GPIO_AF7_USART3
#define BOARD_UART_DEFAULT_ENABLE     __USART3_CLK_ENABLE(); __GPIOD_CLK_ENABLE();
#define BOARD_UART_DEFAULT_DISABLE    __USART3_CLK_DISABLE();
#define BOARD_UART_DEFAULT_IRQ        USART3_IRQn
#define BOARD_UART_DEFAULT_IRQHANDLER USART3_IRQHandler

// TODO: add some other clocks
#define BOARD_I2C_DEFAULT               I2C1
#define BOARD_I2C_DEFAULT_NAME          "I2C1"
#define BOARD_I2C_DEFAULT_SPEED         100000
// 100 kHz over 200 MHz
#define BOARD_I2C_DEFAULT_TIMING_100    0x30C0EDFF
// 400 kHz over 200 MHz
#define BOARD_I2C_DEFAULT_TIMING_400    0x109035B7
// 1   MHz over 200 MHz
#define BOARD_I2C_DEFAULT_TIMING_1M     0x00902787
#define BOARD_I2C_DEFAULT_GPIO_SCL      GPIOB
#define BOARD_I2C_DEFAULT_GPIO_SDA      GPIOB
#define BOARD_I2C_DEFAULT_GPIO_PIN_SCL  GPIO_PIN_8
#define BOARD_I2C_DEFAULT_GPIO_PIN_SDA  GPIO_PIN_9
#define BOARD_I2C_DEFAULT_GPIO_AF       GPIO_AF4_I2C1
#define BOARD_I2C_DEFAULT_ENABLE        __I2C1_CLK_ENABLE(); __GPIOB_CLK_ENABLE();
#define BOARD_I2C_DEFAULT_DISABLE       __I2C1_CLK_DISABLE();
#define BOARD_I2C_DEFAULT_IRQ           I2C1_EV_IRQn
#define BOARD_I2C_DEFAULT_IRQHANDLER    I2C1_EV_IRQHandler


#define BOARD_SPI_DEFAULT               SPI5
#define BOARD_SPI_DEFAULT_NAME          "SPI5"
#define BOARD_SPI_DEFAULT_GPIO_SCK      GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_SCK  GPIO_PIN_7
#define BOARD_SPI_DEFAULT_GPIO_MISO     GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_MISO GPIO_PIN_14
#define BOARD_SPI_DEFAULT_GPIO_MOSI     GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_MOSI GPIO_PIN_15
#define BOARD_SPI_DEFAULT_GPIO_SNSS     GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_SNSS GPIO_PIN_12
#define BOARD_SPI_DEFAULT_GPIO_EXT1     GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_EXT1 GPIO_PIN_11
#define BOARD_SPI_DEFAULT_GPIO_EXT2     GPIOF
#define BOARD_SPI_DEFAULT_GPIO_PIN_EXT2 GPIO_PIN_12
#define BOARD_SPI_DEFAULT_GPIO_AF       GPIO_AF5_SPI2
#define BOARD_SPI_DEFAULT_ENABLE        __SPI5_CLK_ENABLE(); __GPIOF_CLK_ENABLE();
#define BOARD_SPI_DEFAULT_DISABLE       __SPI5_CLK_DISABLE();
#define BOARD_SPI_DEFAULT_IRQ           SPI5_IRQn
#define BOARD_SPI_DEFAULT_IRQHANDLER    SPI5_IRQHandler

//
// #define BOARD_MOTOR_DEFAULT_GPIO        GPIOG
// #define BOARD_MOTOR_DEFAULT_PIN0        9
//
// #define BOARD_1W_DEFAULT_GPIO           GPIOG
// #define BOARD_1W_DEFAULT_PIN            GPIO_PIN_13

// TODO: change, for now A0-A2,3, (0-2,3)
#define BOARD_ADC_DEFAULT_DEV           ADC1
#define BOARD_ADC_DEFAULT_EN            __HAL_RCC_ADC1_CLK_ENABLE();
#define BOARD_ADC_DEFAULT_DIS           __HAL_RCC_ADC1_CLK_DISABLE();
// #define BOARD_ADC_DEFAULT_GPIO0         GPIOA
// #define BOARD_ADC_DEFAULT_PIN0          GPIO_PIN_0
// #define BOARD_ADC_DEFAULT_CH0           ADC_CHANNEL_0
// #define BOARD_ADC_DEFAULT_GPIO1         GPIOA
// #define BOARD_ADC_DEFAULT_PIN1          GPIO_PIN_1
// #define BOARD_ADC_DEFAULT_CH1           ADC_CHANNEL_1
// #define BOARD_ADC_DEFAULT_GPIO2         GPIOA
// #define BOARD_ADC_DEFAULT_PIN2          GPIO_PIN_2
// #define BOARD_ADC_DEFAULT_CH2           ADC_CHANNEL_2
// #define BOARD_ADC_DEFAULT_GPIO3         GPIOA
// #define BOARD_ADC_DEFAULT_PIN3          GPIO_PIN_3
// #define BOARD_ADC_DEFAULT_CH3           ADC_CHANNEL_3
// #define BOARD_ADC_MEM_MAX               (1024*256)
// #define BOARD_ADC_MEM_MAX_FMC           (1024*1024*8)
// #define BOARD_ADC_COEFF                 3250


#define BOARD_CONSOLE_DEFINES         UART_CONSOLE_DEFINES( USART3 );
#define BOARD_CONSOLE_DEFINES_UART    UART_CONSOLE_DEFINES( USART3 );
#define BOARD_PROLOG                  STD_PROLOG_UART;
#define BOARD_CREATE_STD_TASKS        CREATE_STD_TASKS;
#define BOARD_POST_INIT_BLINK         delay_ms( PROLOG_LED_TIME ); leds.write( 0x01 ); delay_ms( PROLOG_LED_TIME );

#endif
