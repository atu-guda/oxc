#include <errno.h>
#include <oxc_gpio.h>

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern uint32_t adc_clk;
void ADC_DMA_REINIT();
uint32_t calc_ADC_clk( uint32_t presc, int *div_val );
uint32_t hint_ADC_presc();

int adc_init_exa_4ch_dma( uint32_t presc, uint32_t sampl_cycl, uint8_t n_ch )
{
  BOARD_ADC_DEFAULT_EN;
  #if defined(STM32F7)
  __HAL_RCC_DAC_CLK_ENABLE(); // !!!!!!!!!!!!! see errata - need for timer interaction
  #endif
  if( n_ch > 4 ) { n_ch = 4; }
  if( n_ch < 1 ) { n_ch = 1; }

  hadc1.Instance                   = BOARD_ADC_DEFAULT_DEV;
  hadc1.Init.ClockPrescaler        = presc;
  adc_clk                          = calc_ADC_clk( presc, nullptr );
  hadc1.Init.Resolution            = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode          = ENABLE;  // if disabled, only first channel works
  hadc1.Init.ContinuousConvMode    = DISABLE; // to start at trigger
  hadc1.Init.DiscontinuousConvMode = DISABLE; // if enabled, seems to not work at all
  hadc1.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T2_TRGO;

  hadc1.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion       = n_ch;
  hadc1.Init.DMAContinuousRequests = DISABLE; // ???
  /// hadc1.Init.DMAContinuousRequests = ENABLE; // ???
  hadc1.Init.EOCSelection          = ADC_EOC_SINGLE_CONV; // test
  if( HAL_ADC_Init( &hadc1 ) != HAL_OK )  {
    errno = 3000;
    return 0;
  }

  decltype(ADC_CHANNEL_0) static const constexpr chs[] {
    BOARD_ADC_DEFAULT_CH0,
    BOARD_ADC_DEFAULT_CH1,
    BOARD_ADC_DEFAULT_CH2,
    BOARD_ADC_DEFAULT_CH3
  };
  ADC_ChannelConfTypeDef sConfig;
  sConfig.SamplingTime = sampl_cycl;

  int rank = 1;
  for( auto ch : chs  ) {
    sConfig.Channel = ch;
    sConfig.Rank = rank++;
    if( HAL_ADC_ConfigChannel( &hadc1 , &sConfig ) != HAL_OK )  {
      errno = 3001;
      return 0;
    }
  }
  return 1;
}

void HAL_ADC_MspInit( ADC_HandleTypeDef* adcHandle )
{
  GPIO_InitTypeDef gio;
  if( adcHandle->Instance == BOARD_ADC_DEFAULT_DEV ) {
    BOARD_ADC_DEFAULT_EN;

    gio.Mode = GPIO_MODE_ANALOG;
    gio.Pull = GPIO_NOPULL;

    GPIO_enableClk( BOARD_ADC_DEFAULT_GPIO0 );
    GPIO_enableClk( BOARD_ADC_DEFAULT_GPIO1 );
    GPIO_enableClk( BOARD_ADC_DEFAULT_GPIO2 );
    GPIO_enableClk( BOARD_ADC_DEFAULT_GPIO3 );
    gio.Pin  = BOARD_ADC_DEFAULT_PIN0;
    HAL_GPIO_Init( BOARD_ADC_DEFAULT_GPIO0, &gio );
    gio.Pin  = BOARD_ADC_DEFAULT_PIN1;
    HAL_GPIO_Init( BOARD_ADC_DEFAULT_GPIO1, &gio );
    gio.Pin  = BOARD_ADC_DEFAULT_PIN2;
    HAL_GPIO_Init( BOARD_ADC_DEFAULT_GPIO2, &gio );
    gio.Pin  = BOARD_ADC_DEFAULT_PIN3;
    HAL_GPIO_Init( BOARD_ADC_DEFAULT_GPIO3, &gio );

    __HAL_RCC_DMA2_CLK_ENABLE();

    ADC_DMA_REINIT();

    // HAL_NVIC_SetPriority( DMA2_Stream0_IRQn, configKERNEL_INTERRUPT_PRIORITY, 0 );
    HAL_NVIC_SetPriority( DMA2_Stream0_IRQn, 1, 0 );
    HAL_NVIC_EnableIRQ( DMA2_Stream0_IRQn );

    HAL_NVIC_SetPriority( ADC_IRQn, 2, 0 );
    HAL_NVIC_EnableIRQ( ADC_IRQn );
  }
}

void ADC_DMA_REINIT()
{
  hdma_adc1.Instance                 = DMA2_Stream0;
  hdma_adc1.Init.Channel             = DMA_CHANNEL_0;
  hdma_adc1.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  hdma_adc1.Init.PeriphInc           = DMA_PINC_DISABLE;
  hdma_adc1.Init.MemInc              = DMA_MINC_ENABLE;
  hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
  hdma_adc1.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;
  hdma_adc1.Init.Mode                = DMA_NORMAL;
  hdma_adc1.Init.Priority            = DMA_PRIORITY_HIGH;
  hdma_adc1.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
  if( HAL_DMA_Init( &hdma_adc1 ) != HAL_OK )   {
    Error_Handler( 6 );
  }

  __HAL_LINKDMA( &hadc1, DMA_Handle, hdma_adc1 );
}

void HAL_ADC_MspDeInit( ADC_HandleTypeDef* adcHandle )
{
  if( adcHandle->Instance == BOARD_ADC_DEFAULT_DEV ) {
    BOARD_ADC_DEFAULT_DIS;
    HAL_GPIO_DeInit( BOARD_ADC_DEFAULT_GPIO0, BOARD_ADC_DEFAULT_PIN0 );
    HAL_GPIO_DeInit( BOARD_ADC_DEFAULT_GPIO1, BOARD_ADC_DEFAULT_PIN1 );
    HAL_GPIO_DeInit( BOARD_ADC_DEFAULT_GPIO2, BOARD_ADC_DEFAULT_PIN2 );
    HAL_GPIO_DeInit( BOARD_ADC_DEFAULT_GPIO3, BOARD_ADC_DEFAULT_PIN3 );
    HAL_DMA_DeInit( adcHandle->DMA_Handle );
  }
}

void ADC_IRQHandler(void)
{
  HAL_ADC_IRQHandler( &hadc1 );
  // leds.toggle( BIT0 );
}

void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler( &hdma_adc1 );
}


uint32_t calc_ADC_clk( uint32_t presc, int *div_val )
{
  int dv_fake = 0;
  if( div_val == nullptr ) {
    div_val = &dv_fake;
  }
  *div_val = 1;

  uint32_t clk =  HAL_RCC_GetPCLK2Freq();
  switch( presc ) {
    case ADC_CLOCK_SYNC_PCLK_DIV2: *div_val = 2; break;
    case ADC_CLOCK_SYNC_PCLK_DIV4: *div_val = 4; break;
    case ADC_CLOCK_SYNC_PCLK_DIV6: *div_val = 6; break;
    case ADC_CLOCK_SYNC_PCLK_DIV8: *div_val = 8; break;
    default: break; // newer
  }
  clk /= *div_val = 2;
  return clk;
}

uint32_t hint_ADC_presc()
{
  uint32_t clk =  HAL_RCC_GetPCLK2Freq();
  const uint32_t max_ADC_Clk = ADC_FREQ_MAX;
  if( ( clk / 2 ) >= max_ADC_Clk ) {
    return ADC_CLOCK_SYNC_PCLK_DIV2;
  }
  if( ( clk / 4 ) >= max_ADC_Clk ) {
    return ADC_CLOCK_SYNC_PCLK_DIV4;
  }
  if( ( clk / 6 ) >= max_ADC_Clk ) {
    return ADC_CLOCK_SYNC_PCLK_DIV6;
  }
  return ADC_CLOCK_SYNC_PCLK_DIV8;
}

