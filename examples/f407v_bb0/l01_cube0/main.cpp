#include <cerrno>

#include <oxc_auto.h>
#include <oxc_hd44780_i2c.h>

#include <ff_gen_drv_st.h>
#include <usbh_diskio.h>
#include <ff.h>

#include <oxc_fs_cmd0.h>

#include <oxc_picoc.h>

using namespace std;
using namespace SMLRL;

USE_DIE4LED_ERROR_HANDLER;
BOARD_DEFINE_LEDS;

BOARD_CONSOLE_DEFINES_UART;

const char* common_help_string = "Appication control cube project" NL;

USBH_HandleTypeDef hUSB_Host;
char USBDISKPath[8]; // USB Host logical drive path
uint8_t sd_buf[512]; // one sector
FATFS fs;
int isUSBH_on = 0, isMSC_ready = 0;

void USBH_HandleEvent( USBH_HandleTypeDef *phost, uint8_t id );
int init_usbh_msc();

I2C_HandleTypeDef i2ch;
DevI2C i2cd( &i2ch, 0 );
HD44780_i2c lcdt( i2cd, 0x3F );
HD44780_i2c *p_lcdt = &lcdt;
void oxc_picoc_hd44780_i2c_init( Picoc *pc );


#define PICOC_STACK_SIZE (32*1024)
int picoc_cmdline_handler( char *s );
Picoc pc;
int init_picoc( Picoc *ppc );
// TMP here: move to system include
extern "C" {
void oxc_picoc_math_init( Picoc *pc );
}
double d_arr[4] = { 1.234, 9.87654321e-10, 5.432198765e12, 1.23456789e-100 };
double *d_ptr = d_arr;
char a_char[] = "ABCDE";
char *p_char = a_char;
void oxc_picoc_misc_init(  Picoc *pc );
void oxc_picoc_fatfs_init( Picoc *pc );

// --- local commands;
int cmd_test0( int argc, const char * const * argv );
CmdInfo CMDINFO_TEST0 { "test0", 'T', cmd_test0, " - test something 0"  };
int cmd_lcd_gotoxy( int argc, const char * const * argv );
CmdInfo CMDINFO_LCD_GOTOXY{ "lcd_gotoxy", 0, cmd_lcd_gotoxy, " x y - move pos to LCD (x, y)"  };
int cmd_lcd_xychar( int argc, const char * const * argv );
CmdInfo CMDINFO_LCD_XYCHAR{ "lcd_xychar", 0, cmd_lcd_xychar, " x y code - put char at x y ln LCD"  };
int cmd_lcd_puts( int argc, const char * const * argv );
CmdInfo CMDINFO_LCD_PUTS{ "lcd_puts", 0, cmd_lcd_puts, "string - put string at cur pos ln  LCD"  };

const CmdInfo* global_cmds[] = {
  DEBUG_CMDS,
  DEBUG_I2C_CMDS,

  &CMDINFO_TEST0,
  &CMDINFO_LCD_XYCHAR,
  &CMDINFO_LCD_GOTOXY,
  &CMDINFO_LCD_PUTS,
  FS_CMDS0,
  nullptr
};

void idle_main_task()
{
  if( isUSBH_on ) {
    leds.set( 1 );
    USBH_Process( &hUSB_Host );
    leds.reset( 1 );
  };
}



int main(void)
{
  STD_PROLOG_UART;

  UVAR('t') = 100;
  UVAR('n') =  20;

  UVAR('e') = i2c_default_init( i2ch /*, 400000 */ );
  i2c_dbg = &i2cd;
  i2c_client_def = &lcdt;
  lcdt.init_4b();
  lcdt.cls();

  fs.fs_type = 0; // none
  fspath[0] = '\0';

  cmdline_handlers[0] = picoc_cmdline_handler;
  cmdline_handlers[1] = nullptr;

  pc.InteractiveHead = nullptr;
  init_picoc( &pc );

  BOARD_POST_INIT_BLINK;
  leds.reset( 0xFF );

  pr( NL "##################### " PROJ_NAME NL );

  srl.re_ps();

  oxc_add_aux_tick_fun( led_task_nortos );

  UVAR('e') = init_usbh_msc();

  std_main_loop_nortos( &srl, idle_main_task );

  return 0;
}

// where to call it? requires main loop idle actions
int init_usbh_msc()
{
  int rc = FATFS_LinkDriver( &USBH_Driver, USBDISKPath );
  if( rc != 0 ) {
    std_out << "# Error LinkDriver: " << rc << NL;
    return 1;
  }

  USBH_Init( &hUSB_Host, USBH_HandleEvent, 0 );

  USBH_RegisterClass( &hUSB_Host, USBH_MSC_CLASS );

  isUSBH_on = 1;
  USBH_Start( &hUSB_Host );

  return 0;
}


int cmd_test0( int argc, const char * const * argv )
{
  // uint32_t n = arg2long_d( 1, argc, argv, UVAR('n'), 1, 100000000 ); // number of series

  std_out << "# Test: " << NL;
  // int fh = _open( "/STM32.txt", 0 #<{(| O_RDONLY|)}># );


  return 0;
}

int picoc_cmdline_handler( char *s )
{
  // static int nnn = 0;

  if( !s  ||  s[0] != ';' ) { // not my
    return -1;
  }

  const char *cmd = s + 1;
  std_out << NL "# C: cmd= \"" << cmd << '"' << NL;
  delay_ms( 10 );
  int ep_rc =  PicocPlatformSetExitPoint( &pc );
  if( ep_rc == 0 ) {
    PicocParse( &pc, "cmd", cmd, strlen(cmd), TRUE, TRUE, FALSE, TRUE );
  } else {
    std_out << "## Exit point: " << ep_rc << NL;
  }

  int rc = 0;

  return rc;

}

// on: 4,3,2 off: 5
void USBH_HandleEvent( USBH_HandleTypeDef *phost, uint8_t id )
{
  // leds.toggle( BIT1 );
  // std_out << "### UP " << (int)id << NL;
  FRESULT fr;

  switch( id ) {
    case HOST_USER_SELECT_CONFIGURATION: // 1
      break;

    case HOST_USER_CLASS_ACTIVE:         // 2
      fr = f_mount( &fs, fspath, 1 ); // todo: flar for automount?
      if( fr == 0 ) {
        isMSC_ready = 1;
        leds.set( BIT2 );
      } else {
        leds.set( BIT0 );
      }
      break;

    case HOST_USER_CLASS_SELECTED:       // 3
      break;

    case HOST_USER_CONNECTION:           // 4
      break;

    case HOST_USER_DISCONNECTION:        // 5
      leds.reset( BIT2 );
      f_mount( nullptr, (TCHAR const*)"", 0 );
      isMSC_ready = 0;
      break;

    case HOST_USER_UNRECOVERED_ERROR:    // 6
      // leds.set( BIT0 );
      f_mount( nullptr, (TCHAR const*)"", 0 );
      errno = 7555;
      break;

    default:
      break;
  }
}

int init_picoc( Picoc *ppc )
{
  if( ppc->InteractiveHead != nullptr ) {
    PicocCleanup( ppc );
  }
  PicocInitialise( ppc, PICOC_STACK_SIZE );
  oxc_picoc_math_init( ppc );
  oxc_picoc_misc_init( ppc );
  oxc_picoc_fatfs_init( ppc );
  oxc_picoc_hd44780_i2c_init( ppc );

  PicocIncludeAllSystemHeaders( ppc );

  VariableDefinePlatformVar( ppc, nullptr, "__a",         &(ppc->IntType), (union AnyValue *)&(UVAR('a')), TRUE );
  VariableDefinePlatformVar( ppc, nullptr, "d_arr",      ppc->FPArrayType, (union AnyValue *)d_arr,        TRUE );
  VariableDefinePlatformVar( ppc, nullptr, "d_ptr",        ppc->FPPtrType, (union AnyValue *)&d_ptr,       TRUE );
  VariableDefinePlatformVar( ppc, nullptr, "a_char",   ppc->CharArrayType, (union AnyValue *)a_char,       TRUE );
  VariableDefinePlatformVar( ppc, nullptr, "p_char",     ppc->CharPtrType, (union AnyValue *)&p_char,      TRUE );
  return 0;
}

// ---------------------------------------- LCD ------------------------------------------------------


int cmd_lcd_xychar( int argc, const char * const * argv )
{
  uint8_t x  = (uint8_t)arg2long_d( 1, argc, argv, 0x0, 0,   64 );
  uint8_t y  = (uint8_t)arg2long_d( 2, argc, argv, 0x0, 0,    3 );
  uint8_t ch = (uint8_t)arg2long_d( 3, argc, argv, 'Z', 0, 0xFF );

  lcdt.putxych( x, y, (uint8_t)ch );

  return 0;
}

int cmd_lcd_gotoxy( int argc, const char * const * argv )
{
  uint8_t x  = (uint8_t)arg2long_d( 1, argc, argv, 0x0, 0,   64 );
  uint8_t y  = (uint8_t)arg2long_d( 2, argc, argv, 0x0, 0,    3 );

  lcdt.gotoxy( x, y );
  return 0;
}

int cmd_lcd_puts( int argc, const char * const * argv )
{
  const char *s = "Z";
  if( argc > 1 ) {
    s = argv[1];
  }
  lcdt.puts( s );

  return 0;
}

// ----------------------------------------  ------------------------------------------------------

// vim: path=.,/usr/share/stm32cube/inc/,/usr/arm-none-eabi/include,/usr/share/stm32oxc/inc

