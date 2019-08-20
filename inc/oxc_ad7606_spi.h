#ifndef _OXC_AD7606_SPI_H
#define _OXC_AD7606_SPI_H

// AD7606 - ADC 8ch simultanious, 16 bit, SPI/parallel, 200 kSps
// AD7606_7606-6_7606-4_ADC_16_x8_SPI_200ksa.pdf
// Used one-way(in) SPI + RST, ConStA, BUSY?
// Here: only out outout channel (DoutA) used

#include <oxc_spi.h>

class AD7606_SPI {
  public:
   // enum {
   // };
   AD7606_SPI( DevSPI &a_spi, PinsOut &a_rst, PinsOut &a_cnvst, PinsIn &a_busy )
     : spi_d( a_spi ),  rst_pin( a_rst ), cnvst_pin ( a_cnvst ), busy_pin( a_busy ) {};
   void init();
   void reset() { rst_pin.set( 1 );     delay_bad_n( 10 ); rst_pin.reset( 1 ); delay_bad_n( 5 ); }
   void start() { cnvst_pin.reset( 1 ); /* delay_bad_mcs(  1 ); */ cnvst_pin.set( 1 ); }
   uint32_t wait_nobusy();
   int read_only( int16_t *d, unsigned n );
   int read( int16_t *d, unsigned n );

  protected:
   DevSPI &spi_d;
   PinsOut &rst_pin;
   PinsOut &cnvst_pin;
   PinsIn  &busy_pin;
};

#endif

// vim: path=.,/usr/share/stm32cube/inc
