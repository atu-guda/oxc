#include <cstring>
#include <cstdlib>

#include <oxc_auto.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;

// PinsOut p1 { GPIOE, 8, 8 };
BOARD_DEFINE_LEDS;

extern "C" {
void TIM8_CC_IRQHandler(void);
};

TIM_HandleTypeDef tim8h;
void init_usonic();

const int def_stksz = 1 * configMINIMAL_STACK_SIZE;

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

  leds.write( 0x00 );

  UVAR('t') = 1000;
  UVAR('n') = 20;

  global_smallrl = &srl;

  //           code               name    stack_sz      param  prty TaskHandle_t*
  xTaskCreate( task_leds,        "leds", 1*def_stksz, nullptr,   1, nullptr );
  xTaskCreate( task_usart2_send, "send", 1*def_stksz, nullptr,   2, nullptr );  // 2
  xTaskCreate( task_main,        "main", 1*def_stksz, nullptr,   1, nullptr );
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

  init_usonic();

  delay_ms( 10 );

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

  TickType_t tc0 = xTaskGetTickCount();

  delay_ms( 10 );
  break_flag = 0; idle_flag = 1;


  for( int i=0; i<n && !break_flag; ++i ) {
    pr( "[" ); pr_d( i );
    pr( "]  l= " ); pr_d( UVAR('l') );
    pr( NL );
    vTaskDelayUntil( &tc0, t_step );
    // delay_ms( t_step );
  }

  pr( NL );

  delay_ms( 10 );
  break_flag = 0;
  idle_flag = 1;

  pr( NL "test0 end." NL );
  return 0;
}

//  ----------------------------- configs ----------------
//
void init_usonic()
{
  tim8h.Instance = TIM8;
  tim8h.Init.Prescaler         = 846; // 5.8 mks approx 1mm
  tim8h.Init.Period            = 8500; // F approx 20Hz: for future motor PWM
  tim8h.Init.ClockDivision     = 0;
  tim8h.Init.CounterMode       = TIM_COUNTERMODE_UP;
  tim8h.Init.RepetitionCounter = 0;
  if( HAL_TIM_PWM_Init( &tim8h ) != HAL_OK ) {
    UVAR('e') = 1; // like error
    return;
  }

  TIM_OC_InitTypeDef  tim_oc_cfg;
  tim_oc_cfg.OCMode = TIM_OCMODE_PWM1;
  tim_oc_cfg.OCPolarity = TIM_OCPOLARITY_HIGH;
  tim_oc_cfg.OCFastMode = TIM_OCFAST_DISABLE;
  tim_oc_cfg.Pulse = 3; // 3 = appex 16 mks
  if( HAL_TIM_PWM_ConfigChannel( &tim8h, &tim_oc_cfg, TIM_CHANNEL_1 ) != HAL_OK ) {
    UVAR('e') = 11;
    return;
  }
  if( HAL_TIM_PWM_Start( &tim8h, TIM_CHANNEL_1 ) != HAL_OK ) {
    UVAR('e') = 12;
    return;
  }

  TIM_IC_InitTypeDef  tim_ic_cfg;
  // tim_ic_cfg.ICPolarity = TIM_ICPOLARITY_RISING;
  tim_ic_cfg.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
  tim_ic_cfg.ICSelection = TIM_ICSELECTION_DIRECTTI;
  tim_ic_cfg.ICPrescaler = TIM_ICPSC_DIV1;
  tim_ic_cfg.ICFilter = 0; // 0 - 0x0F

  if( HAL_TIM_IC_ConfigChannel( &tim8h, &tim_ic_cfg, TIM_CHANNEL_2 ) != HAL_OK )
  {
    UVAR('e') = 21;
    return;
  }

  HAL_NVIC_SetPriority( TIM8_CC_IRQn, 14, 0 );
  HAL_NVIC_EnableIRQ( TIM8_CC_IRQn );

  if( HAL_TIM_IC_Start_IT( &tim8h, TIM_CHANNEL_2 ) != HAL_OK ) {
    UVAR('e') = 23;
  }
}

void TIM8_CC_IRQHandler(void)
{
  HAL_TIM_IRQHandler( &tim8h );
}

void HAL_TIM_IC_CaptureCallback( TIM_HandleTypeDef *htim )
{
  uint32_t cap2;
  static uint32_t c_old = 0xFFFFFFFF;
  if( htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 )  {
    cap2 = HAL_TIM_ReadCapturedValue( htim, TIM_CHANNEL_2 );
    if( cap2 > c_old ) {
      UVAR('l') = cap2 - c_old ;
    }
    c_old = cap2;

    UVAR('m') = cap2;
    UVAR('z') = htim->Instance->CNT;
  }
}



FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
