#ifndef _OXC_SSD1306_H
#define _OXC_SSD1306_H

#include <oxc_i2c.h>
#include <oxc_pixbuf1v.h>

class SSD1306 {
  public:
   enum {
     BASE_ADDR = 0x3C,
     X_SZ = 128,
     Y_SZ = 64,
     CMD_1 = 0x80,
     CMD_N = 0x00,
     DATA_1 = 0xC0,
     DATA_N = 0x40,
     CMD_MODE = 0x20,
     CMD_CONTRAST = 0x81,
     CMD_RAM = 0xA4, // default, output follows RAM
     CMD_FULLON = 0xA5, // output in ON, independent of RAM
     CMD_NOINVERSE = 0xA6,
     CMD_INVERSE = 0xA7,
     CMD_OFF = 0xAE,
     CMD_ON = 0xAF,
     MEM_SZ = ( X_SZ * Y_SZ / 8 )
   };
   SSD1306( DevI2C &a_dev, uint8_t a_addr = BASE_ADDR )
     : dev( a_dev ), addr( a_addr ) {};
   void setAddr( uint8_t d_addr ) { addr = d_addr; };
   uint8_t getAddr() const { return addr; }
   void resetDev() { dev.resetDev(); }
   int init();
   int cmd1( uint8_t cmd );
   int cmd2( uint8_t cmd, uint8_t val );
   int data1( uint8_t d );

   int switch_on() { return cmd1( CMD_ON ); };
   int switch_off() { return cmd1( CMD_OFF ); };
   int contrast( uint8_t v ) { return cmd2( CMD_CONTRAST, v ); };
   int full_on() { return cmd1( CMD_FULLON ); };
   int on_ram() { return cmd1( CMD_RAM ); };
   int no_inverse() { return cmd1( CMD_NOINVERSE ); };
   int inverse() { return cmd1( CMD_INVERSE ); };
   int mode_horisontal() { return cmd2( CMD_MODE, 0x00 ); };
   int mode_vertical()   { return cmd2( CMD_MODE, 0x01 ); };
   int mode_paged()      { return cmd2( CMD_MODE, 0x02 ); };
   int out( PixBuf1V &pb );

  protected:
   DevI2C &dev;
   uint8_t addr;
};

#endif

// vim: path=.,/usr/share/stm32cube/inc
