#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>

#include <ff.h>
#include <fatfs.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;
FreeRTOS_to_stm32cube_tick_hook;
BOARD_DEFINE_LEDS;

USBCDC_CONSOLE_DEFINES;

extern SD_HandleTypeDef hsd;
void MX_SDIO_SD_Init();
uint8_t sd_buf[512]; // one sector
HAL_SD_CardInfoTypeDef cardInfo;
FATFS fs;
const int fspath_sz = 32;
char fspath[fspath_sz];


extern "C" {
 void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef *hadc );
 void HAL_ADC_ErrorCallback( ADC_HandleTypeDef *hadc );
 void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim );
}
void MX_ADC1_Init( uint8_t n_ch, uint32_t sampl_time );
void ADC_DMA_REINIT();
void pr_ADC_state();
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
uint32_t tim_freq_in; // timer input freq
uint32_t adc_clk = 36000000;     // depend in MCU, set in MX_ADC1_Init
uint32_t t_step = 100000; // in us, recalculated before measurement
int v_adc_ref = 3250; // in mV, measured before test, adjust as UVAR('v')
const uint32_t n_ADC_ch_max = 4; // current - in UVAR('c')
const uint32_t n_ADC_mem  = 1024*32; // MCU dependent, in 16-bit samples
const uint32_t n_ADC_mem_guard  = n_ADC_mem + 2 * n_ADC_ch_max; // 2 lines for guard
uint16_t adc_v0[ n_ADC_mem_guard ];
volatile int adc_end_dma = 0;
volatile int adc_dma_error = 0;
volatile uint32_t n_series = 0;
uint32_t n_series_todo = 0;
const uint32_t n_sampl_times = 7; // current number - in UVAR('s')
const uint32_t sampl_times_codes[n_sampl_times] = { // all for 36 MHz ADC clock
  ADC_SAMPLETIME_3CYCLES   , //  15  tick: 2.40 MSa,  0.42 us
  ADC_SAMPLETIME_15CYCLES  , //  27  tick: 1.33 MSa,  0.75 us
  ADC_SAMPLETIME_28CYCLES  , //  40  tick:  900 kSa,  1.11 us
  ADC_SAMPLETIME_56CYCLES  , //  68  tick:  529 kSa,  1.89 us
  ADC_SAMPLETIME_84CYCLES  , //  96  tick:  375 kSa,  2.67 us
  ADC_SAMPLETIME_144CYCLES , // 156  tick:  231 kSa,  4.33 us
  ADC_SAMPLETIME_480CYCLES   // 492  tick:   73 kSa, 13.67 us
};
const uint32_t sampl_times_cycles[n_sampl_times] = { // sample+conv(12)
    15,  // ADC_SAMPLETIME_3CYCLES
    27,  // ADC_SAMPLETIME_15CYCLES
    40,  // ADC_SAMPLETIME_28CYCLES
    68,  // ADC_SAMPLETIME_56CYCLES
    96,  // ADC_SAMPLETIME_84CYCLES
   156,  // ADC_SAMPLETIME_144CYCLES
   492,  // ADC_SAMPLETIME_480CYCLES
};



TIM_HandleTypeDef tim2h;
void tim2_init( uint16_t presc = 36, uint32_t arr = 100 ); // 1MHz, 10 kHz
void tim2_deinit();

const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test ADC"  };
int cmd_out( int argc, const char * const * argv );
CmdInfo CMDINFO_OUT { "out", 'O', cmd_out, " [N [start]]- output data "  };
int cmd_outsd( int argc, const char * const * argv );
CmdInfo CMDINFO_OUTSD { "outsd", 'X', cmd_outsd, "filename [N [start]]- output data to SD"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_OUT,
  &CMDINFO_OUTSD,
  nullptr
};


extern "C" {
void task_main( void *prm UNUSED_ARG );
}

int main(void)
{
  STD_PROLOG_USBCDC;

  MX_SDIO_SD_Init();
  UVAR('e') = HAL_SD_Init( &hsd );
  delay_ms( 10 );
  MX_FATFS_Init();
  UVAR('x') = HAL_SD_GetState( &hsd );
  UVAR('y') = HAL_SD_GetCardInfo( &hsd, &cardInfo );
  fs.fs_type = 0; // none
  fspath[0] = '\0';
  UVAR('z') = f_mount( &fs, "", 1 );

  tim_freq_in = HAL_RCC_GetPCLK1Freq(); // to TIM2
  uint32_t hclk_freq = HAL_RCC_GetHCLKFreq();
  if( tim_freq_in < hclk_freq ) {
    tim_freq_in *= 2;
  }

  UVAR('t') = 1000; // 1 s extra wait
  UVAR('v') = v_adc_ref;
  UVAR('p') = (tim_freq_in/1000000)-1; // timer PSC, for 1MHz
  UVAR('a') = 99999; // timer ARR, for 10Hz
  UVAR('c') = n_ADC_ch_max;
  UVAR('n') = 8; // number of series
  UVAR('s') = 1; // sampling time index

  delay_ms( PROLOG_LED_TIME ); leds.write( 0x01 ); delay_ms( PROLOG_LED_TIME );


  delay_ms( PROLOG_LED_TIME ); leds.write( 0x01 ); delay_ms( PROLOG_LED_TIME );

  CREATE_STD_TASKS( task_usbcdc_send );

  SCHEDULER_START;
  return 0;
}

void task_main( void *prm UNUSED_ARG ) // TMAIN
{
  default_main_loop();
  vTaskDelete(NULL);
}

void pr_ADC_state()
{
  if( UVAR('d') > 0 ) {
    pr_shx( ADC1->SR );
    pr_shx( ADC1->CR1 );
    pr_shx( ADC1->CR2 );
    pr_shx( ADC1->SMPR2 );
    pr_shx( ADC1->SQR1 );
    pr_shx( ADC1->SQR3 );
  }
}

void pr_TIM_state( TIM_TypeDef *htim )
{
  if( UVAR('d') > 1 ) {
    pr_sdx( htim->CNT  );
    pr_sdx( htim->ARR  );
    pr_sdx( htim->PSC  );
    pr_shx( htim->CR1  );
    pr_shx( htim->CR2  );
    pr_shx( htim->SMCR );
    pr_shx( htim->DIER );
    pr_shx( htim->SR   );
  }
}

// TEST0
int cmd_test0( int argc, const char * const * argv )
{
  char buf[32];
  uint8_t n_ch = UVAR('c');
  if( n_ch > n_ADC_ch_max ) { n_ch = n_ADC_ch_max; };
  if( n_ch < 1 ) { n_ch = 1; };

  const uint32_t n_ADC_series_max  = n_ADC_mem / ( 2 * n_ch ); // 2 is 16bit/sample
  uint32_t n = arg2long_d( 1, argc, argv, UVAR('n'), 0, n_ADC_series_max ); // number of series

  uint32_t sampl_t_idx = UVAR('s');
  if( sampl_t_idx >= n_sampl_times ) { sampl_t_idx = n_sampl_times-1; };
  uint32_t f_sampl_ser = adc_clk / ( sampl_times_cycles[sampl_t_idx] * n_ch );

  uint32_t t_step_tick =  (UVAR('a')+1) * (UVAR('p')+1); // in timer input ticks
  uint32_t tim_f = tim_freq_in / t_step_tick; // timer update freq
  t_step = t_step_tick / ( tim_freq_in / 1000000 ); // in us
  uint32_t t_wait0 = n  * t_step / 1000; // in ms
  if( t_wait0 < 1 ) { t_wait0 = 1; }

  if( n > n_ADC_series_max ) { n = n_ADC_series_max; };

  pr( NL "Test0: n= " ); pr_d( n ); pr( " n_ch= " ); pr_d( n_ch );
  pr( "  tim_f= " ); pr_d( tim_f );
  pr( " Hz;  t_step= " ); pr_d( t_step );
  pr( " us;  f_sampl_ser= " ); pr_d( f_sampl_ser );
  pr( " t_wait0= " ); pr_d( t_wait0 );  pr( NL );
  ifcvt( t_step, 1000000, buf, 6 );
  pr( " t_step= " ); pr( buf );
  // double zz = 0.1;
  // pr( " sizeof(double)=" ); pr_d( sizeof(zz));
  // float t_step_f = (float)t_step_tick / tim_freq_in;
  // snprintf( buf, sizeof(buf)-1, "%g", t_step_f );
  // pr( " = " ); pr( buf );
  // double t_step_f = (float)t_step_tick / tim_freq_in;
  // snprintf( buf, sizeof(buf)-1, "%g", t_step_f );
  // pr( " = " ); pr( buf );
  pr( NL );
  tim2_deinit();

  pr_ADC_state();
  hadc1.Instance->SR = 0;
  HAL_ADC_MspDeInit( &hadc1 );
  delay_ms( 10 );
  HAL_ADC_MspInit( &hadc1 );
  MX_ADC1_Init( n_ch, sampl_t_idx );
  delay_ms( 10 );

  // log_add( "Test0 " );

  uint32_t n_ADC_bytes = n * n_ch;
  uint32_t n_ADC_bytes_guard = n_ADC_bytes + n_ch * 2;
  for( uint32_t i=0; i<n_ADC_bytes_guard;  ++i ) { // TODO: memset
    adc_v0[i] = 0;
  }
  adc_end_dma = 0; adc_dma_error = 0; n_series = 0; n_series_todo = n;
  TickType_t tc0 = xTaskGetTickCount(), tc00 = tc0;
  if( HAL_ADC_Start_DMA( &hadc1, (uint32_t*)adc_v0, n_ADC_bytes ) != HAL_OK )   {
    pr( "ADC_Start_DMA error" NL );
  }
  tim2_init( UVAR('p'), UVAR('a') );

  delay_ms( t_wait0 );
  for( uint32_t ti=0; adc_end_dma == 0 && ti<(uint32_t)UVAR('t'); ++ti ) {
    delay_ms(1);
  }
  TickType_t tcc = xTaskGetTickCount();
  delay_ms( 10 ); // to settle all

  // HAL_ADC_Stop( &hadc1 );
  if( adc_end_dma == 0 ) {
    pr( "Fail to wait DMA end " NL );
  }
  if( adc_dma_error != 0 ) {
    pr( "Found DMA error "  ); pr_d( adc_dma_error ); pr( NL );
  }
  pr( "  tick: "); pr_d( tcc - tc00 );
  pr( NL );

  bool was_hole = false;
  for( uint32_t i=0; i< (n_series_todo+2); ++i ) { // +2 = show guard
    if( i > 2 && i < n_series_todo - 2 ) {
      if( ! was_hole ) {
        was_hole = true;
        pr( "....." NL );
      }
      continue;
    }
    for( int j=0; j< n_ch; ++j ) {
      // pr_d( adc_v0[i*n_ch+j] ) ; pr( "\t" );
      int vv = adc_v0[i*n_ch+j] * 10 * UVAR('v') / 4096;
      ifcvt( vv, 10000, buf, 4 );
      pr( buf ); pr( "\t" );
    }
    pr( NL );
  }
  pr( NL );

  pr_ADC_state();
  pr( NL );

  delay_ms( 10 );
  pr_TIM_state( TIM2 );

  return 0;
}

int cmd_out( int argc, const char * const * argv )
{
  char buf[32];
  uint8_t n_ch = UVAR('c');
  if( n_ch > n_ADC_ch_max ) { n_ch = n_ADC_ch_max; };
  if( n_ch < 1 ) { n_ch = 1; };
  uint32_t n = arg2long_d( 1, argc, argv, n_series_todo, 0, n_series_todo+1 ); // number output series
  uint32_t st= arg2long_d( 2, argc, argv,             0, 0, n_series_todo-2 );

  if( n+st >= n_series_todo+1 ) {
    n = 1 + n_series_todo - st;
  }

  uint32_t t = st * t_step;
  for( uint32_t i=0; i< n; ++i ) {
    uint32_t ii = i + st;
    ifcvt( t, 1000000, buf, 6 );
    pr( buf ); pr( "   " );
    for( int j=0; j< n_ch; ++j ) {
      int vv = adc_v0[ii*n_ch+j] * 10 * UVAR('v') / 4096;
      ifcvt( vv, 10000, buf, 4 );
      pr( buf ); pr( "  " );
    }
    t += t_step;
    pr( NL );
    delay_ms( 5 );
  }

  return 0;
}

int cmd_outsd( int argc, const char * const * argv )
{
  if( argc < 2 ) {
    pr( "Error: need filename [n [start]]" NL );
    return 1;
  }

  char buf[32];
  uint8_t n_ch = UVAR('c');
  if( n_ch > n_ADC_ch_max ) { n_ch = n_ADC_ch_max; };
  if( n_ch < 1 ) { n_ch = 1; };
  uint32_t n = arg2long_d( 2, argc, argv, n_series_todo, 0, n_series_todo+1 ); // number output series
  uint32_t st= arg2long_d( 3, argc, argv,             0, 0, n_series_todo-2 );

  if( n+st >= n_series_todo+1 ) {
    n = 1 + n_series_todo - st;
  }

  uint32_t t = st * t_step;

  const char *fn = argv[1];
  FIL f;
  FRESULT r = f_open( &f, fn, FA_WRITE | FA_OPEN_ALWAYS );
  if( r == FR_OK ) {
    for( uint32_t i=0; i< n; ++i ) {
      uint32_t ii = i + st;
      ifcvt( t, 1000000, buf, 6 );
      f_puts( buf, &f ); f_puts( "   ", &f );
      for( int j=0; j< n_ch; ++j ) {
        int vv = adc_v0[ii*n_ch+j] * 10 * UVAR('v') / 4096;
        ifcvt( vv, 10000, buf, 4 );
        f_puts( buf, &f ); f_puts( "  ", &f );
      }
      t += t_step;
      f_puts( NL, &f );
    }
    f_close( &f );
  } else {
    pr( "f_open error: " ); pr_d( r ); pr( NL );
  }

  return r;
}


void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef *hadc )
{
  // tim2_deinit();
  UVAR('x') = hadc1.Instance->SR;
  hadc1.Instance->SR = 0;
  adc_end_dma |= 1;
  leds.toggle( BIT2 );
  ++UVAR('g'); // 'g' means good
}

void HAL_ADC_ErrorCallback( ADC_HandleTypeDef *hadc )
{
  // tim2_deinit();
  UVAR('z') = HAL_ADC_GetError( hadc );
  adc_dma_error = hadc->DMA_Handle->ErrorCode;
  UVAR('y') = hadc1.Instance->SR;
  HAL_ADC_Stop_DMA( hadc );
  hadc1.Instance->SR = 0;
  hadc->DMA_Handle->ErrorCode = 0;
  adc_end_dma |= 2;
  // leds.toggle( BIT0 );
  ++UVAR('e');
}

void _exit( int rc )
{
  die4led( rc );
}

void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler( &tim2h );
}

// not used for now: only TRGO
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim )
{
  ++UVAR('i');
  UVAR('j') = htim->Instance->CNT;
  ++n_series;
  if( n_series < n_series_todo ) {
    // ADC1->CR2 |= 0x40000000; // SWSTART???
  } else {
    htim->Instance->CR1 &= ~1u;
    // STOP?
  }
  leds.toggle( BIT1 );
}

void HAL_ADCEx_InjectedConvCpltCallback( ADC_HandleTypeDef * /*hadc*/ )
{
}

// configs

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc

