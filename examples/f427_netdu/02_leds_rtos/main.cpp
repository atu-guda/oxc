#include <oxc_auto.h>

using namespace std;

USE_DIE4LED_ERROR_HANDLER;

void MX_GPIO_Init(void);


BOARD_DEFINE_LEDS;

const int def_stksz = 2 * configMINIMAL_STACK_SIZE;

extern "C" {

void task_leds( void *prm UNUSED_ARG );

}

volatile int led_delay = 1000;

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

  MX_GPIO_Init();

  leds.write( 0x0A );
  delay_bad_ms( 500 );
  // HAL_Delay( 500 );
  // delay_ms( 500 );
  leds.write( 0x0F );
  delay_bad_ms( 500 );

  xTaskCreate( task_leds, "leds", 2*def_stksz, 0, 1, 0 );

  ready_to_start_scheduler = 1;
  vTaskStartScheduler();

  die4led( 0xFF );
  return 0;
}

void task_leds( void *prm UNUSED_ARG )
{
  int i=8;
  while (1)
  {
    leds.write( i );
    ++i;
    i &= 0x0F;
    delay_ms( led_delay );
    // HAL_Delay( 1000 );
  }
}

// // configs
void MX_GPIO_Init(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef gpi;

  gpi.Pin = GPIO_PIN_5;
  // gpi.Mode = GPIO_MODE_EVT_RISING;
  gpi.Mode = GPIO_MODE_IT_RISING;
  gpi.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init( GPIOB, &gpi );

  HAL_NVIC_SetPriority( EXTI9_5_IRQn, configKERNEL_INTERRUPT_PRIORITY, 0 );
  // HAL_NVIC_SetPriority( EXTI1_IRQn, configKERNEL_INTERRUPT_PRIORITY, 0 );
  HAL_NVIC_EnableIRQ( EXTI9_5_IRQn );
  // HAL_NVIC_EnableIRQ( EXTI1_IRQn );
}

void EXTI9_5_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler( BIT5 );
}

// void EXTI1_IRQHandler(void)
// {
//   HAL_GPIO_EXTI_IRQHandler( BIT1 );
// }

void HAL_GPIO_EXTI_Callback( uint16_t pin )
{
  // if( pin == BIT0 )  {
  //   leds.set( 0x0F );
  //   led_delay = 1000;
  // }
  if( pin == BIT5 )  {
    leds.reset( 0x0F );
    led_delay >>= 1;
    ++led_delay;
  }
  leds.toggle( 0x01 );
}

FreeRTOS_to_stm32cube_tick_hook;

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,../../../inc
