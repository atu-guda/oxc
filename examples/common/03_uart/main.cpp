#include <cstring>

#include <oxc_auto.h>

using namespace std;

USE_DIE4LED_ERROR_HANDLER;
FreeRTOS_to_stm32cube_tick_hook;
BOARD_DEFINE_LEDS;

extern "C" {
void task_leds( void *prm UNUSED_ARG );
void task_send( void *prm UNUSED_ARG );
} // extern "C"

UART_HandleTypeDef uah;

const int TX_BUF_SZ = 128;
char tx_buf[TX_BUF_SZ];

int main(void)
{
  STD_PROLOG_UART_NOCON;

  xTaskCreate( task_leds, "leds", 1*def_stksz, 0, 1, 0 );
  xTaskCreate( task_send, "send", 2*def_stksz, 0, 1, 0 );

  SCHEDULER_START;
  return 0;
}


void task_send( void *prm UNUSED_ARG )
{
  char c = '?', cn = '0';

  while( 1 ) {
    bool was_action = false;
    strcpy( tx_buf, "ABCDE <.> -----\r\n" );
    //               0123456789ABCDEF1011
    int ssz = strlen( tx_buf );
    // leds.toggle( BIT2 );
    tx_buf[10] = cn++;
    if( cn >= 0x7F ) { cn = ' '; }

    if( __HAL_USART_GET_FLAG( &uah, UART_FLAG_ORE ) ) { // overrun
      __HAL_USART_CLEAR_OREFLAG( &uah );
      tx_buf[13] = 'O';
      leds.toggle( BIT1 );
    }

    if( __HAL_USART_GET_FLAG( &uah, UART_FLAG_RXNE ) ) {
      c = uah.Instance->USART_RX_REG;
      was_action = true;
      // leds.toggle( BIT2 );
      if( int8_t(c) >= ' ' ) {
        tx_buf[7]  = c;
      }
      tx_buf[12] = 'R';
    }

    if( HAL_UART_Transmit( &uah, (uint8_t*)tx_buf, ssz, 100 ) != HAL_OK ) {
      // leds.toggle( BIT0 );
    }
    delay_ms( was_action ? 50 : 1000 );
  }
}



// vim: path=.,/usr/share/stm32cube/inc/,/usr/arm-none-eabi/include,/usr/share/stm32oxc/inc
