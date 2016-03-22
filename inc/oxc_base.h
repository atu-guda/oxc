#ifndef _OXC_BASE_H
#define _OXC_BASE_H

#if defined (STM32F0)
 #include <stm32f0xx_hal.h>
 #define SET_BIT_REG   BSRR
 #define RESET_BIT_REG BRR
 #define RESET_BIT_SHIFT 0
 #define USART_TX_REG TDR
 #define USART_RX_REG RDR
 #define USART_SR_REG ISR
#elif defined (STM32F1)
 #include <stm32f1xx_hal.h>
 #define SET_BIT_REG   BSRR
 #define RESET_BIT_REG BRR
 #define RESET_BIT_SHIFT 0
 #define USART_TX_REG DR
 #define USART_RX_REG DR
 #define USART_SR_REG SR
#elif defined (STM32F2)
 #include <stm32f2xx_hal.h>
 #define SET_BIT_REG   BSRR
 #define RESET_BIT_REG BSRR
 #define RESET_BIT_SHIFT 16
 #define USART_TX_REG DR
 #define USART_RX_REG DR
 #define USART_SR_REG SR
#elif defined (STM32F3)
 #include <stm32f3xx_hal.h>
 #define SET_BIT_REG   BSRR
 #define RESET_BIT_REG BRR
 #define RESET_BIT_SHIFT 0
 #define USART_TX_REG TDR
 #define USART_RX_REG RDR
 #define USART_SR_REG ISR
#elif defined (STM32F4) || defined(STM32F7)
 #include <stm32f4xx_hal.h>
 #define SET_BIT_REG   BSRR
 #define RESET_BIT_REG BSRR
 #define RESET_BIT_SHIFT 16
 #define USART_TX_REG DR
 #define USART_RX_REG DR
 #define USART_SR_REG SR
#else
  #error "Unsupported MCU"
#endif

#if REQ_MCBASE != MCBASE
  #error "Required and give MCBASE is not equal"
#endif

#define UNUSED_ARG __attribute__((unused))

#if defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif
#endif


#define PORT_BITS 16
#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

extern int ready_to_start_scheduler;
extern int exit_rc;
extern volatile int dbg_val0, dbg_val1, dbg_val2, dbg_val3;
extern volatile int idle_flag;
extern volatile int break_flag;
extern char* __heap_top;

typedef __IO uint32_t reg32;
typedef const char *const ccstr;
#define BAD_ADDR ((void*)(0xFFFFFFFF))

#define ARR_SZ(x) (sizeof(x) / sizeof(x[0]))
#define ARR_AND_SZ(x) x, (sizeof(x) / sizeof(x[0]))


// timings for bad loop delays TODO: for other too
// for 72MHz
#if REQ_SYSCLK_FREQ == 24
  #define T_MKS_MUL    2
  #define T_MS_MUL  2660
  #define T_S_MUL   3420459
#elif REQ_SYSCLK_FREQ == 36
  #define T_MKS_MUL    4
  #define T_MS_MUL  3990
  #define T_S_MUL   5130689
#elif REQ_SYSCLK_FREQ == 48
  #define T_MKS_MUL    5
  #define T_MS_MUL  5319
  #define T_S_MUL   6840918
#elif REQ_SYSCLK_FREQ == 56
  #define T_MKS_MUL    8
  #define T_MS_MUL  7979
  #define T_S_MUL   10261378
#elif REQ_SYSCLK_FREQ == 72
  #define T_MKS_MUL 9
  #define T_MS_MUL  7979
  #define T_S_MUL   10259736
#elif REQ_SYSCLK_FREQ == 168
  #define T_MKS_MUL 32
  #define T_MS_MUL  33845
  #define T_S_MUL   33845000
#endif


#ifdef __cplusplus
// template<typename T> class _ShowType; // to output decucted type
//                                       // _ShowType< decltype(XXXX) > xType;
 extern "C" {
#endif

// void die4led( uint16_t n );
void taskYieldFun(void);
void vApplicationIdleHook(void);
void vApplicationTickHook(void);
// misc functions
void _exit( int rc );
void die( uint16_t n );

void delay_ms( uint32_t ms ); // base on vTaskDelay - switch to shceduler (if avail), or to HAL
void delay_mcs( uint32_t mcs );
// dumb delay fuctions - loop based - for use w/o timer and for small times
void delay_bad_n( uint32_t n );
void delay_bad_s( uint32_t s );
void delay_bad_ms( uint32_t ms );
void delay_bad_mcs( uint32_t mcs );

// signal handlers descriotions
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void OTG_FS_IRQHandler(void);
void USB_LP_CAN_RX0_IRQHandler(void);
void USBWakeUp_IRQHandler(void);
void WWDG_IRQHandler(void);

void SystemClock_Config(void);

#define FreeRTOS_to_stm32cube_tick_hook \
  void vApplicationTickHook() { HAL_IncTick();  }


#ifdef __cplusplus
}
#endif



#endif

// vim: path=.,/usr/share/stm32cube/inc
