#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;

// PinsOut p1 { GPIOC, 0, 4 };
BOARD_DEFINE_LEDS;



const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

SmallRL srl( smallrl_exec );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  nullptr
};


extern "C" {
void task_main( void *prm UNUSED_ARG );
}


UART_HandleTypeDef huart1;
UsartIO usartio( &huart1, USART1 );
void MX_USART1_UART_Init(void);

void MX_FMC_Init(void);
void BSP_SDRAM_Initialization_sequence( uint32_t RefreshCount );

STD_USART1_SEND_TASK( usartio );
STD_USART1_IRQ( usartio );

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  leds.initHW();

  MX_USART1_UART_Init();
  leds.write( 0x0F );  delay_bad_ms( 200 );
  MX_FMC_Init();
  leds.write( 0x01 );  delay_bad_ms( 200 );
  BSP_SDRAM_Initialization_sequence( 0 ); // 0 if fake
  leds.write( 0x0A );  delay_bad_ms( 500 );


  // usartio.sendStrSync( "0123456789---main()---ABCDEF" NL );

  UVAR('t') = 1000;
  UVAR('n') = 10;

  global_smallrl = &srl;

  //           code               name    stack_sz      param  prty TaskHandle_t*
  xTaskCreate( task_leds,        "leds", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_usart1_send, "send", 2*def_stksz, nullptr,   2, nullptr );  // 2
  xTaskCreate( task_main,        "main", 2*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_gchar,      "gchar", 2*def_stksz, nullptr,   1, nullptr );

  leds.write( 0x00 );
  ready_to_start_scheduler = 1;
  vTaskStartScheduler();

  die4led( 0xFF );
  return 0;
}

void task_main( void *prm UNUSED_ARG ) // TMAIN
{
  SET_UART_AS_STDIO(usartio);

  usartio.sendStrSync( "0123456789ABCDEF" NL );
  delay_ms( 10 );

  default_main_loop();
  vTaskDelete(NULL);
}



void _exit( int rc )
{
  die4led( rc );
}
// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  pr( NL "Test0: " );
  pr( NL );

  uint8_t *mem = (uint8_t *)(0xD0000000);
  dump8( mem, 0x40, true );



  pr( NL );

  delay_ms( 10 );
  break_flag = 0;  idle_flag = 1;

  pr( NL "test0 end." NL );
  return 0;
}


// // configs
FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
