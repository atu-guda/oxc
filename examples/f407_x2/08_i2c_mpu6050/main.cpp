#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>
#include <oxc_mpu6050.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;
FreeRTOS_to_stm32cube_tick_hook;
BOARD_DEFINE_LEDS;

USBCDC_CONSOLE_DEFINES;


const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };

int cmd_data0( int argc, const char * const * argv );
CmdInfo CMDINFO_DATA0 { "data0", 'D', cmd_data0, " - data transmission 0"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,
  DEBUG_I2C_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_DATA0,
  nullptr
};


extern "C" {
void task_main( void *prm UNUSED_ARG );
}

I2C_HandleTypeDef i2ch;
MPU6050 accel( &i2ch );

void MX_I2C1_Init( I2C_HandleTypeDef &i2c, uint32_t speed = 100000 );


int main(void)
{
  STD_PROLOG_USBCDC;

  UVAR('t') = 1000;
  UVAR('n') = 10;

  MX_I2C1_Init( i2ch );
  i2c_dbg = &accel;

  delay_ms( PROLOG_LED_TIME ); leds.write( 0x01 ); delay_ms( PROLOG_LED_TIME );

  CREATE_STD_TASKS( task_usbcdc_send );

  SCHEDULER_START;
  return 0;
}

void task_main( void *prm UNUSED_ARG ) // TMAIN
{
  accel.init();
  accel.setAccScale( MPU6050::ACC_scale::accs_8g );
  accel.setDLP( MPU6050::DLP_BW::bw_44 );
  delay_ms( 50 );

  default_main_loop();
  vTaskDelete(NULL);
}


// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  int n = arg2long_d( 1, argc, argv, UVAR('n'), 0 );
  uint32_t t_step = UVAR('t');
  pr( NL "Test0: n= " ); pr_d( n ); pr( " t= " ); pr_d( t_step );
  pr( NL );

  int16_t adata[MPU6050::mpu6050_alldata_sz];
  // accel.sleep();
  accel.resetDev();
  accel.setDLP( MPU6050::DLP_BW::bw_10 );
  accel.init();

  TickType_t tc0 = xTaskGetTickCount();

  for( int i=0; i<n && !break_flag; ++i ) {
    for( auto &d : adata ) { d = 0; };
    accel.getAll( adata );
    for( int j=0; j<MPU6050::mpu6050_alldata_sz; ++j ) {
      pr_d( (int)(adata[j]) ); pr( " " );
    }
    pr( NL );
    vTaskDelayUntil( &tc0, t_step );
    // delay_ms( t_step );
  }

  return 0;
}


int cmd_data0( int argc UNUSED_ARG , const char * const * argv  UNUSED_ARG )
{
  int16_t adata[MPU6050::mpu6050_alldata_sz];
  int tick_c = xTaskGetTickCount();

  pr( "@0 " ); pr_d( tick_c ); pr( " " );

  for( auto &d : adata ) { d = 0; };
  accel.getAll( adata );
  for( int j=0; j<MPU6050::mpu6050_alldata_sz; ++j ) {
    pr_d( (int)(adata[j]) ); pr( " " );
  }
  pr( NL );
  return 0;
}

//  ----------------------------- configs ----------------


// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc

