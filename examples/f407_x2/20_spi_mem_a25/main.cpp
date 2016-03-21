#include <cstring>
#include <cstdlib>

#include <oxc_gpio.h>
#include <oxc_usbcdcio.h>
#include <oxc_console.h>
#include <oxc_spi.h>
#include <oxc_spimem_at.h>
#include <oxc_debug1.h>
#include <oxc_common1.h>
#include <oxc_smallrl.h>

#include <FreeRTOS.h>
#include <task.h>

using namespace std;
using namespace SMLRL;

// PinsOut p1 { GPIOC, 0, 4 };
BOARD_DEFINE_LEDS;

UsbcdcIO usbcdc;


const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

SmallRL srl( smallrl_exec );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

int cmd_spimem_erase( int argc, const char * const * argv );
CmdInfo CMDINFO_ERASR { "erase",  0, cmd_spimem_erase, "- ERASE CHIP!"  };

int cmd_spimem_sector0_erase( int argc, const char * const * argv );
CmdInfo CMDINFO_ERAS0 { "era0",  0, cmd_spimem_sector0_erase, "- erase sector 0"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_ERASR,
  &CMDINFO_ERAS0,
  nullptr
};


extern "C" {
void task_main( void *prm UNUSED_ARG );
}

STD_USBCDC_SEND_TASK( usbcdc );

int MX_SPI1_Init();
PinsOut nss_pin( GPIOA, 4, 1 ); //  to test GPIO
SPI_HandleTypeDef spi1_h;
DevSPI spi_d( &spi1_h, &nss_pin );
DevSPIMem_AT memspi( spi_d );

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  leds.initHW();

  leds.write( 0x0F );  delay_bad_ms( 200 );
  if( MX_SPI1_Init() != HAL_OK ) {
    die4led( 0x04 );
  }
  // nss_pin.initHW(); //?? remove?
  spi_d.initSPI();

  UVAR('t') = 1000;
  UVAR('n') = 10;
  UVAR('r') = 0x20; // default bytes to read/write

  global_smallrl = &srl;

  //           code               name    stack_sz      param  prty TaskHandle_t*
  xTaskCreate( task_leds,        "leds", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_usbcdc_send, "send", 2*def_stksz, nullptr,   2, nullptr );  // 2
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
  SET_USBCDC_AS_STDIO(usbcdc);

  default_main_loop();
  vTaskDelete(NULL);
}

// #define DLY_T delay_mcs( 10 );

// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int nd     = imin( UVAR('r'), sizeof(gbuf_b) );
  pr( NL "Test0: nd= " ); pr_d( nd );
  pr( NL );

  uint16_t chip_id = memspi.read_id();
  pr_shx( chip_id );

  int status = memspi.status();
  pr_shx( status );

  memset( gbuf_b, '\x00', sizeof( gbuf_b ) );

  int rc = memspi.read( (uint8_t*)gbuf_b, 0x00, nd );
  pr( " Read Before: rc = " ); pr_d( rc ); pr( NL );
  dump8( gbuf_b, nd );

  for( int i=0; i<nd; ++i ) {
    gbuf_a[i] = (char)( '0' + i );
  }
  rc = memspi.write( (uint8_t*)gbuf_a, 0x00, nd );
  pr( NL "Write: rc= " ); pr_d( rc ); pr( NL );

  rc = memspi.read( (uint8_t*)gbuf_b, 0x00, nd );
  pr( " Read After: rc = " ); pr_d( rc ); pr( NL );
  dump8( gbuf_b, nd );

  rc = memspi.read( (uint8_t*)gbuf_b, 0x03, nd );
  pr( " Read After with offset 3: rc = " ); pr_d( rc ); pr( NL );
  dump8( gbuf_b, nd );

  delay_ms( 10 );
  break_flag = 0;  idle_flag = 1;

  pr( NL "test0 end." NL );
  return 0;
}

int cmd_spimem_erase( int argc, const char * const * argv )
{
  pr( NL "Erase chips " NL );

  TickType_t tc0 = xTaskGetTickCount();

  int rc = memspi.erase_chip();

  TickType_t tc1 = xTaskGetTickCount();

  pr( NL "rc= " ); pr_d( rc ); pr( " ticks: " ); pr_d( tc1 - tc0 );
  break_flag = 0;  idle_flag = 1;

  pr( NL "Erase end." NL );
  return 0;
}

int cmd_spimem_sector0_erase( int argc, const char * const * argv )
{
  pr( NL "Erase sector 0 " NL );

  TickType_t tc0 = xTaskGetTickCount();

  int rc = memspi.erase_sector( 0x000000 );

  TickType_t tc1 = xTaskGetTickCount();

  pr( NL "rc= " ); pr_d( rc ); pr( " ticks: " ); pr_d( tc1 - tc0 );
  break_flag = 0;  idle_flag = 1;

  pr( NL "Erase end." NL );
  return 0;
}



//  ----------------------------- configs ----------------

FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
