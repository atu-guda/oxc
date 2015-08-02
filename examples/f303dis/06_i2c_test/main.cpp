#include <cstring>
#include <cstdlib>

#include <oxc_gpio.h>
#include <oxc_usartio.h>
#include <oxc_console.h>
#include <oxc_debug1.h>
#include <oxc_debug_i2c.h>
#include <oxc_common1.h>
#include <oxc_smallrl.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

using namespace std;
using namespace SMLRL;

// PinsOut p1 { GPIOE, 8, 8 };
BOARD_DEFINE_LEDS;



const int def_stksz = 1 * configMINIMAL_STACK_SIZE;

SmallRL srl( smallrl_print, smallrl_exec );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,
  DEBUG_I2C_CMDS,

  &CMDINFO_TEST0,
  nullptr
};


extern "C" {

void task_main( void *prm UNUSED_ARG );


}

I2C_HandleTypeDef i2ch;
void MX_I2C1_Init( I2C_HandleTypeDef &i2c );

UART_HandleTypeDef uah;
UsartIO usartio( &uah, USART2 );
int init_uart( UART_HandleTypeDef *uahp, int baud = 115200 );

STD_USART2_SEND_TASK( usartio );
// STD_USART2_RECV_TASK( usartio );
STD_USART2_IRQ( usartio );

int main(void)
{
  HAL_Init();

  SystemClock_Config();
  leds.initHW();

  leds.write( 0x0F );  delay_bad_ms( 200 );
  if( !init_uart( &uah ) ) {
    die4led( 0x08 );
  }
  leds.write( 0x0A );  delay_bad_ms( 200 );

  // HAL_UART_Transmit( &uah, (uint8_t*)"START\r\n", 7, 100 );

  usartio.sendStrSync( "0123456789---main()---ABCDEF" NL );

  MX_I2C1_Init( i2ch );
  i2ch_dbg = &i2ch;


  leds.write( 0x00 );

  user_vars['t'-'a'] = 1000;
  user_vars['n'-'a'] = 10;

  global_smallrl = &srl;

  //           code               name    stack_sz      param  prty TaskHandle_t*
  xTaskCreate( task_leds,        "leds", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_usart2_send, "send", 1*def_stksz, nullptr,   2, nullptr );  // 2
  xTaskCreate( task_main,        "main", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_gchar,      "gchar", 2*def_stksz, nullptr,   1, nullptr );

  vTaskStartScheduler();
  die4led( 0xFF );



  return 0;
}

void task_main( void *prm UNUSED_ARG ) // TMAIN
{
  uint32_t nl = 0;

  delay_ms( 50 );
  user_vars['t'-'a'] = 1000;

  usartio.itEnable( UART_IT_RXNE );
  usartio.setOnSigInt( sigint );
  devio_fds[0] = &usartio; // stdin
  devio_fds[1] = &usartio; // stdout
  devio_fds[2] = &usartio; // stderr

  delay_ms( 50 );
  pr( "*=*** Main loop: ****** " NL );
  delay_ms( 20 );

  srl.setSigFun( smallrl_sigint );
  srl.set_ps1( "\033[32m#\033[0m ", 2 );
  srl.re_ps();
  srl.set_print_cmd( true );


  idle_flag = 1;
  while(1) {
    ++nl;
    if( idle_flag == 0 ) {
      pr_sd( ".. main idle  ", nl );
      srl.redraw();
    }
    idle_flag = 0;
    delay_ms( 60000 );
    // delay_ms( 1 );

  }
  vTaskDelete(NULL);
}


// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int st_a = 2;
  if( argc > 1 ) {
    st_a = strtol( argv[1], 0, 0 );
    pr( NL "Test0: argv[1]= \"" ); pr( argv[1] ); pr( "\"" NL );
  }
  int en_a = 127;
  if( argc > 2 ) {
    pr( NL "Test0: argv[2]= \"" ); pr( argv[2] ); pr( "\"" NL );
    en_a = strtol( argv[2], 0, 0 );
  }
  pr( NL "Test0: st_a= " ); pr_h( st_a ); pr( " en_a= " ); pr_h( en_a );
  pr( NL );
  __HAL_I2C_DISABLE( &i2ch );
  delay_ms( 50 );
  __HAL_I2C_ENABLE( &i2ch );
  delay_ms( 50 );

  // uint8_t val;
  int i_err;
  for( uint8_t ad = (uint16_t)st_a; ad <= (uint16_t)en_a && ! break_flag; ++ad ) {
    uint16_t ad2 = ad << 1;
    // HAL_StatusTypeDef rc = HAL_I2C_Master_Transmit( &i2ch, ad, &val, 1, 200 );
    HAL_StatusTypeDef rc = HAL_I2C_IsDeviceReady( &i2ch, ad2, 3, 200 );
    i_err = i2ch.ErrorCode;
    // pr( "ad = " ); pr_h( ad ); pr( "  rc= " ); pr_d( rc ) ; pr( "  err= " ); pr_d( i_err ); pr(NL);
    delay_ms( 10 );
    if( rc != HAL_OK ) {
      continue;
    }
    pr( "ad = " ); pr_h( ad ); pr( "  rc= " ); pr_d( rc ) ; pr( "  err= " ); pr_d( i_err ); pr(NL);
    // pr( NL "************************************ " );
    // pr_shx( ad );
  }

  pr( NL );

  delay_ms( 10 );
  break_flag = 0;
  idle_flag = 1;

  pr( NL "test0 end." NL );
  return 0;
}

//  ----------------------------- configs ----------------

FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc

