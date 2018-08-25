#ifndef _MEAS0_H
#define _MEAS0_H

struct D_in_sources {
  decltype( GPIOA ) gpio;
  uint16_t           bit;
};

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim8;

inline const int n_adc_ch = 4;
inline const int n_din_ch = 4;
inline const int n_pwm_ch = 4;
extern float vref_in;
extern float vref_out;

extern D_in_sources d_ins[n_din_ch];

int MX_TIM2_Init();
int MX_TIM3_Init();
int MX_TIM8_Init();

extern DAC_HandleTypeDef hdac;
int  MX_DAC_Init(void);
void dac_output( float v0, float v1 );

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
int  MX_ADC1_Init();
int  MX_DMA_Init(); // used for ADC
int  dma_subinit();

#endif

