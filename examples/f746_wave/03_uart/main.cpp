#include <cstring>

#include <oxc_auto.h>

using namespace std;

USE_DIE4LED_ERROR_HANDLER;



BOARD_DEFINE_LEDS;

const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

extern "C" {

void task_leds( void *prm UNUSED_ARG );
void task_send( void *prm UNUSED_ARG );

} // extern "C"

UART_HandleTypeDef uah;
const int TX_BUF_SZ = 128;
char tx_buf[TX_BUF_SZ];
int init_uart( UART_HandleTypeDef *uahp, int baud = 115200 );

int main(void)
{
  HAL_Init();

  leds.initHW();
  leds.write( BOARD_LEDS_ALL );

  int rc = SystemClockCfg();
  if( rc ) {
    die4led( BOARD_LEDS_ALL );
    return 0;
  }

  delay_bad_ms( 200 );  leds.write( 0 );

  if( ! init_uart( &uah ) ) {
      die4led( 1 );
  }

  leds.write( BOARD_LEDS_ALL );
  delay_bad_ms( 200 );

  xTaskCreate( task_leds, "leds", 1*def_stksz, 0, 1, 0 );
  xTaskCreate( task_send, "send", 2*def_stksz, 0, 1, 0 );

  leds.write( 0x00 );
  ready_to_start_scheduler = 1;
  vTaskStartScheduler();

  die4led( 0xFF );
  return 0;
}


void task_send( void *prm UNUSED_ARG )
{
  strcpy( tx_buf, "ABC <.> ------\r\n" );
  //               0123456789ABCDEF1011
  int ssz = strlen( tx_buf );
  char c = '?', cn = '0';

  while( 1 ) {
    leds.toggle( BIT1 );
    tx_buf[5]  = ' ';
    tx_buf[9] = cn; tx_buf[11] = '.';
    ++cn;
    if( cn >= 0x7F ) { cn = ' '; }
    // if( HAL_UART_Receive( &uah, (uint8_t*)&c, 1, 0 ) == HAL_OK ) {
    //   leds.toggle( BIT2 );
    //   tx_buf[5]  = c; tx_buf[11] = 'R';
    // }
    if( uah.Instance->USART_SR_REG & UART_FLAG_RXNE ) {
      c = uah.Instance->USART_RX_REG;
      leds.toggle( BIT2 );
      tx_buf[5]  = c; tx_buf[11] = 'R';
    }
    if( uah.Instance->USART_SR_REG & UART_FLAG_ORE ) { // overrun
      uah.Instance->ICR |= UART_CLEAR_OREF;
      c = uah.Instance->USART_RX_REG;
      tx_buf[11] = 'O';
      leds.toggle( BIT0 );
    }
    if( HAL_UART_Transmit( &uah, (uint8_t*)tx_buf, ssz, 100 )!= HAL_OK ) {
      leds.toggle( BIT0 );
    }
    delay_ms( 1000 );
  }
}



// configs
FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc

