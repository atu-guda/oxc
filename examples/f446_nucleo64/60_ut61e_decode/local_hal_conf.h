#ifndef _LOCAL_HAL_CONF_H
#define _LOCAL_HAL_CONF_H

#define UART_UT61E USART1
#define UART_UT61E_CLK_ENABLE __HAL_RCC_USART1_CLK_ENABLE
#define UART_UT61E_DMA_CLK_ENABLE __HAL_RCC_DMA2_CLK_ENABLE

#define DMA_UT61E_IRQ DMA2_Stream2_IRQn
#define UT61E_DMA_IRQHandler DMA2_Stream2_IRQHandler
#define DMA_UT61E_STREAM DMA2_Stream2
#define DMA_UT61E_CHANNEL DMA_CHANNEL_4

#define UART_UT61E_GPIO GpioA
#define UART_UT61E_GPIO_PINS ( BIT9 | BIT10 )
#define UART_UT61E_GPIO_AF GPIO_AF7_USART1

#define HAL_UART_USERINIT_FUN HAL_UART_UserInit

int MX_UT61E_UART_Init(void);
void MX_UT61E_DMA_Init(void);
// void HAL_UART_UserInit( UART_HandleTypeDef* uartHandle );


#endif
