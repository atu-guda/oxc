#ifndef _OXC_COMMON1
#define _OXC_COMMON1

#include <oxc_devio.h>


// SmallRL storage and config
int smallrl_print( const char *s, int l ) __weak;
int smallrl_exec( const char *s, int l ) __weak;
void smallrl_sigint(void) __weak; // unused?
void sigint(int v) __weak;

void default_main_loop();

extern "C" {

void task_leds( void *prm UNUSED_ARG ) __weak;
void task_gchar( void *prm UNUSED_ARG ) __weak;
void task_main( void *prm UNUSED_ARG );

}


#endif

