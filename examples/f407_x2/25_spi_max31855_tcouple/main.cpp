#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;

// PinsOut p1 { GPIOC, 0, 4 };
BOARD_DEFINE_LEDS;

UsbcdcIO usbcdc;


const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

SmallRL srl( smallrl_exec );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };


int cmd_reset_spi( int argc, const char * const * argv );
CmdInfo CMDINFO_RESETSPI { "reset_spi", 'Z', cmd_reset_spi, " - reset spi"  };

  const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_RESETSPI,
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

const uint32_t MAX31855_FAIL = 0x00010000;
const uint32_t MAX31855_BRK  = 0x00000001;
const uint32_t MAX31855_GND  = 0x00000002;
const uint32_t MAX31855_VCC  = 0x00000004;

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  leds.initHW();

  leds.write( 0x0F );  delay_bad_ms( 200 );
  if( MX_SPI1_Init() != HAL_OK ) {
    die4led( 0x04 );
  }
  // nss_pin.initHW();
  //nss_pin.set(1);
  spi_d.setMaxWait( 500 );
  spi_d.initSPI();

  UVAR('t') = 100;
  UVAR('n') = 10;

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

// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int n      = arg2long_d( 1, argc, argv,    UVAR('n'), 1, 0xFFFFFF );
  int t_step = UVAR('t');
  pr( NL "Test0: n= " ); pr_d( n );
  pr( NL " t_step= " ); pr_d( t_step );
  pr( NL );

  uint8_t v[4];
  int rc;
  TickType_t tc0 = xTaskGetTickCount(), tc00 = tc0;

  for( int i=0; i<n; ++i ) {
    rc = spi_d.recv( (uint8_t*)(v), sizeof(v) );
    TickType_t tcc = xTaskGetTickCount();
    pr_d( tcc - tc00 ); pr( " " );
    pr_d( rc ); pr( " " );
    // pr( NL );
    // dump8( v, sizeof(v) );

    if( v[0] & MAX31855_FAIL ) {
      pr( "FAIL, " );
      if( v[0] & MAX31855_BRK ) {
        pr( "BREAK" );
      }
      if( v[0] & MAX31855_GND ) {
        pr( "GND" );
      }
      if( v[0] & MAX31855_VCC ) {
        pr( "VCC" );
      }
    } else {
      int32_t tif =  ( v[3] >> 4 ) | ( v[2] << 4 );
      if( tif & 0x0800 ) {
        tif |= 0xFFFFF000;
      }
      // pr_d( tif ); pr( " = " ); pr_h( tif );
      int32_t tid4 = tif * 625;
      // pr( " = " ) ;
      pr_d( tid4 / 10000 ); pr( "." ); pr_d( tid4 % 10000 );
      pr( " " );

      int32_t tof =  ( v[1] >> 2 ) | ( v[0] << 6 );
      if( tof & 0x2000 ) {
        tof |= 0xFFFFC000;
      }
      int tod4 = tof * 25;
      pr( " " );
      pr_d( tod4 / 100 ); pr( "." ); pr_d( tod4 % 100 );
    }

    pr( NL );
    vTaskDelayUntil( &tc0, t_step );
  }


  spi_d.pr_info();


  delay_ms( 10 );
  break_flag = 0;  idle_flag = 1;

  pr( NL "test0 end." NL );
  return 0;
}



int cmd_reset_spi( int argc UNUSED_ARG, const char * const * argv UNUSED_ARG )
{
  spi_d.resetDev();

  spi_d.pr_info();

  pr( NL "reset SPI end." NL );
  return 0;
}



//  ----------------------------- configs ----------------

FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
