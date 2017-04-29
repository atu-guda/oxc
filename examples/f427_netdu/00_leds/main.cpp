#include <oxc_auto.h>

void MX_GPIO_Init(void);
int delay_bad(void);

USE_DIE_ERROR_HANDLER;


int main(void)
{
  HAL_Init();

  int rc = SystemClockCfg();
  if( rc ) {
    die( 1 );
    return 0;
  }

  MX_GPIO_Init();


  int i=0x04,  j = 0;
  volatile int t = 0;
  BOARD_LEDS_GPIO->ODR = BOARD_LEDS_MASK;
  while(1) {
    // j = ( (~i) & 0x000F ) << shift;
    t = HAL_GetTick();
    j = (t + 1);
    t = j - 1;
    GPIO_WriteBits( BOARD_LEDS_GPIO,                 i, BOARD_LEDS_MASK );
    //
    // GPIO_WriteBits( TPORT, j, mask );
    // HAL_GPIO_TogglePin( GPIOC, i );
    ++i;
    i &= BOARD_LEDS_MASK;
    HAL_Delay( 200 );
    // delay_bad();
  }
  return 0;
}

int delay_bad()
{
  volatile int k = 0, j;
  for( j=0; j<800000; ++j ) {
    k += j * j;
  }
  return k;
}

// configs


void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  //__GPIOA_CLK_ENABLE();
  //__GPIOB_CLK_ENABLE();
  __GPIOC_CLK_ENABLE();
  // __GPIOD_CLK_ENABLE();
  // __GPIOE_CLK_ENABLE();
  // __GPIOG_CLK_ENABLE();
  // __GPIOH_CLK_ENABLE();

  /*Configure GPIO pins : PC0 PC1 PC2 PC3 */
  GPIO_InitStruct.Pin = BOARD_LEDS_MASK; // GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init( BOARD_LEDS_GPIO, &GPIO_InitStruct );

  /*Configure GPIO pins : PA0 PA1 */
  // GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  // GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  // GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  // HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include,ox/inc
