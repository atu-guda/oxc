#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>

#include <oxc_spi_max7219.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;

BOARD_DEFINE_LEDS;



const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

SmallRL srl( smallrl_exec );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

int cmd_sendr_spi( int argc, const char * const * argv );
CmdInfo CMDINFO_SENDR { "sendr", 'S', cmd_sendr_spi, "[0xXX ...] - send bytes, recv UVAR('r')"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_SENDR,
  nullptr
};


extern "C" {
void task_main( void *prm UNUSED_ARG );
}


int SPI2_Init_common( uint32_t baud_presc  = SPI_BAUDRATEPRESCALER_256 );
PinsOut nss_pin( GPIOB, 1, 1 ); //  to test GPIO
SPI_HandleTypeDef spi2_h;
DevSPI spi_d( &spi2_h, &nss_pin );
DevSPI_MAX7219 max7219( spi_d );



UART_HandleTypeDef uah;
UsartIO usartio( &uah, USART2 );
int init_uart( UART_HandleTypeDef *uahp, int baud = 115200 );

STD_USART2_SEND_TASK( usartio );
// STD_USART2_RECV_TASK( usartio );
STD_USART2_IRQ( usartio );

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

  HAL_Delay( 200 ); // delay_bad_ms( 200 );
  leds.write( 0x00 ); delay_ms( 200 );
  leds.write( BOARD_LEDS_ALL );  HAL_Delay( 200 );

  if( ! init_uart( &uah ) ) {
      die4led( 1 );
  }
  leds.write( 0x0A );  delay_bad_ms( 200 );

  if( SPI2_Init_common( SPI_BAUDRATEPRESCALER_256 ) != HAL_OK ) {
    die4led( 0x04 );
  }
  // nss_pin.initHW();
  //nss_pin.set(1);
  spi_d.initSPI();

  delay_bad_ms( 200 );  leds.write( 1 );

  UVAR('t') = 1000;
  UVAR('n') = 10;
  UVAR('r') = 0; // default bytes to read

  global_smallrl = &srl;

  //           code               name    stack_sz      param  prty TaskHandle_t*
  xTaskCreate( task_leds,        "leds", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_usart2_send, "send", 2*def_stksz, nullptr,   2, nullptr );  // 2
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
  SET_UART_AS_STDIO( usartio );

  default_main_loop();
  vTaskDelete(NULL);
}

#define DLY_T delay_mcs( 10 );

// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int v = arg2long_d( 1, argc, argv, 987 );
  uint8_t pos = arg2long_d( 2, argc, argv,  1, 0, 7 );
  uint8_t l = arg2long_d( 3, argc, argv,  3, 1, 8 );
  uint8_t v0[] = { 1, 2, 3, 4, 0x0E, 6, 7, 8 };
  pr( NL "Test0: v= " ); pr_d( v ); pr( " = " ); pr_h( v ); pr( "  pos= " ); pr_d( pos );
  pr( NL );

  // spi_d.resetDev();

  max7219.setIntens( 1 );
  max7219.setDecode( 0xFE ); // position 0 - bitfield
  max7219.setLimit( 7 );

  for( uint8_t i=1; i<8; ++i ) {
    max7219.setDigit( i, i+8 );
  };
  max7219.setDigit( 0, 0xC9 );

  max7219.test_on();
  delay_ms( 200 );
  max7219.test_off();
  delay_ms( 200 );
  max7219.off();
  delay_ms( 200 );
  max7219.on();
  delay_ms( 200 );
  max7219.setIntens( 3 );
  delay_ms( 200 );
  max7219.setIntens( 5 );
  delay_ms( 200 );
  max7219.setIntens( 7 );
  delay_ms( 200 );
  max7219.setIntens( 0 );

  delay_ms( 500 );
  max7219.setLimit( 4 );
  delay_ms( 500 );
  max7219.setLimit( 7 );
  delay_ms( 500 );

  max7219.setDecode( 0xFF ); // all = digits
  max7219.setDigits( v0, 3, 0, 5 );
  delay_ms( 1000 );
  max7219.setUVal( 12345678, 7, 0, 8 );
  delay_ms( 1000 );
  max7219.clsDig();
  delay_ms( 500 );
  max7219.setVal( v, 1, pos, l );

  delay_ms( 1000 );
  max7219.setDecode( 0x00 ); // all = bitmap
  max7219.setXVal( v, 1, 0, 8 );
  delay_ms( 1000 );
  max7219.setXDigit( 7, 0x10 );
  max7219.setXDigit( 6, 0x11 );
  max7219.setXDigit( 5, 0x12 );
  max7219.setXDigit( 4, 0x13 );
  max7219.setXDigit( 3, 0x14 );
  max7219.setXDigit( 2, 0x15 );

  delay_ms( 2000 );
  // max7219.setDecode( 0xFF ); // all = digits
  for( int i=0; i<8; ++i ) {
    max7219.setDigit( i, i );
  }

  delay_ms( 2000 );
  // max7219.setDecode( 0xFF ); // all = digits
  for( int i=0; i<8; ++i ) {
    max7219.setDigit( i, 1<<i | 1<<(i/2) | 0x81 );
  }

  return 0;
}

int cmd_sendr_spi( int argc, const char * const * argv )
{
  uint8_t sbuf[16]; // really used not more then 9 - max args
  uint16_t ns = argc - 1;

  for( uint16_t i = 0; i<ns; ++i ) {
    uint8_t t = arg2long_d( i+1, argc, argv, 0, 0, 0xFF );
    sbuf[i] = t;
  }

  int nd = imin( UVAR('r'), sizeof(gbuf_a) );
  pr( NL "Send/recv: ns= " ); pr_d( ns ); pr( " nd= " ); pr_d( nd );
  pr( NL );
  dump8( sbuf, ns );

  int rc = spi_d.send_recv( sbuf, ns, (uint8_t*)gbuf_a, nd );

  pr_sdx( rc );
  if( rc > 0 ) {
    dump8( gbuf_a, rc );
  }

  return 0;
}

//  ----------------------------- configs ----------------

FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
