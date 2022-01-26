#include <cstring>

#include <oxc_auto.h>

#include <oxc_modbus_rtu_server.h>



const uint8_t MODBUS_RTU_server::CRC_hi[256] = {
  // 0     1     2     3      4     5     6     7      8     9     A     B      C     D     E     F
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // 00
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // 10
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // 20
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // 30
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // 40
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // 50
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // 60
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // 70
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // 80
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // 90
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // A0
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // B0
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  // C0
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // D0
  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40,  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  // E0
  0x00, 0xC1, 0x81, 0x40,  0x01, 0xC0, 0x80, 0x41,  0x01, 0xC0, 0x80, 0x41,  0x00, 0xC1, 0x81, 0x40   // F0
};

const uint8_t MODBUS_RTU_server::CRC_lo[256] = {
  // 0     1     2     3      4     5     6     7      8     9     A     B      C     D     E     F
  0x00, 0xC0, 0xC1, 0x01,  0xC3, 0x03, 0x02, 0xC2,  0xC6, 0x06, 0x07, 0xC7,  0x05, 0xC5, 0xC4, 0x04,  // 00
  0xCC, 0x0C, 0x0D, 0xCD,  0x0F, 0xCF, 0xCE, 0x0E,  0x0A, 0xCA, 0xCB, 0x0B,  0xC9, 0x09, 0x08, 0xC8,  // 10
  0xD8, 0x18, 0x19, 0xD9,  0x1B, 0xDB, 0xDA, 0x1A,  0x1E, 0xDE, 0xDF, 0x1F,  0xDD, 0x1D, 0x1C, 0xDC,  // 20
  0x14, 0xD4, 0xD5, 0x15,  0xD7, 0x17, 0x16, 0xD6,  0xD2, 0x12, 0x13, 0xD3,  0x11, 0xD1, 0xD0, 0x10,  // 30
  0xF0, 0x30, 0x31, 0xF1,  0x33, 0xF3, 0xF2, 0x32,  0x36, 0xF6, 0xF7, 0x37,  0xF5, 0x35, 0x34, 0xF4,  // 40
  0x3C, 0xFC, 0xFD, 0x3D,  0xFF, 0x3F, 0x3E, 0xFE,  0xFA, 0x3A, 0x3B, 0xFB,  0x39, 0xF9, 0xF8, 0x38,  // 50
  0x28, 0xE8, 0xE9, 0x29,  0xEB, 0x2B, 0x2A, 0xEA,  0xEE, 0x2E, 0x2F, 0xEF,  0x2D, 0xED, 0xEC, 0x2C,  // 60
  0xE4, 0x24, 0x25, 0xE5,  0x27, 0xE7, 0xE6, 0x26,  0x22, 0xE2, 0xE3, 0x23,  0xE1, 0x21, 0x20, 0xE0,  // 70
  0xA0, 0x60, 0x61, 0xA1,  0x63, 0xA3, 0xA2, 0x62,  0x66, 0xA6, 0xA7, 0x67,  0xA5, 0x65, 0x64, 0xA4,  // 80
  0x6C, 0xAC, 0xAD, 0x6D,  0xAF, 0x6F, 0x6E, 0xAE,  0xAA, 0x6A, 0x6B, 0xAB,  0x69, 0xA9, 0xA8, 0x68,  // 90
  0x78, 0xB8, 0xB9, 0x79,  0xBB, 0x7B, 0x7A, 0xBA,  0xBE, 0x7E, 0x7F, 0xBF,  0x7D, 0xBD, 0xBC, 0x7C,  // A0
  0xB4, 0x74, 0x75, 0xB5,  0x77, 0xB7, 0xB6, 0x76,  0x72, 0xB2, 0xB3, 0x73,  0xB1, 0x71, 0x70, 0xB0,  // B0
  0x50, 0x90, 0x91, 0x51,  0x93, 0x53, 0x52, 0x92,  0x96, 0x56, 0x57, 0x97,  0x55, 0x95, 0x94, 0x54,  // C0
  0x9C, 0x5C, 0x5D, 0x9D,  0x5F, 0x9F, 0x9E, 0x5E,  0x5A, 0x9A, 0x9B, 0x5B,  0x99, 0x59, 0x58, 0x98,  // D0
  0x88, 0x48, 0x49, 0x89,  0x4B, 0x8B, 0x8A, 0x4A,  0x4E, 0x8E, 0x8F, 0x4F,  0x8D, 0x4D, 0x4C, 0x8C,  // E0
  0x44, 0x84, 0x85, 0x45,  0x87, 0x47, 0x46, 0x86,  0x82, 0x42, 0x43, 0x83,  0x41, 0x81, 0x80, 0x40   // F0
};

MODBUS_RTU_server::MODBUS_RTU_server( USART_TypeDef *a_uart, volatile uint32_t *a_tim_cnt )
  : uart( a_uart ), tim_cnt( a_tim_cnt )
{
  reset();
}

void MODBUS_RTU_server::reset()
{
  i_pos = o_pos = 0;
  state = ST_IDLE; // ST_INIT;
  last_uart_status = 0;
  t_char = *tim_cnt;
  memset( ibuf, std::size(ibuf), '\x00' );
  memset( obuf, std::size(obuf), '\x00' );
}

uint16_t MODBUS_RTU_server::crc( const uint8_t *s, uint16_t l )
{
  if( !s || l < 1 || l > 254 ) {
    return 0; // TODO: error?
  }

  uint8_t  uh = 0xFF, ul = 0xFF;

  while( l-- ) {
    uint16_t idx = ul ^ *( s++ );
    ul = ( uint8_t )( uh ^ CRC_hi[idx] );
    uh = CRC_lo[idx];
  }
  return ( uint16_t )( uh << 8 | ul );
}

bool MODBUS_RTU_server::isCrcGood() const
{
  if( state != ST_MSG_IN || i_pos < 4 ) { // TODO: real min size @ F1 CRC2
    return false;
  }
  uint16_t v0 = (ibuf[i_pos-1] << 8) | ibuf[i_pos-2];
  uint16_t v1 = crc( ibuf, i_pos-2 );
  UVAR('v') = v0;
  UVAR('w') = v1;
  return v1 == v0;
}

void MODBUS_RTU_server::handle_UART_IRQ()
{
  int n_work = 0;

  leds.toggle( 4 );
  last_uart_status = uart->USART_SR_REG;
  UVAR('s') = last_uart_status;
  ++UVAR('i');

  if( last_uart_status & UART_FLAG_RXNE ) { // char received
    ++UVAR('j');
    leds.set( BIT1 );
    ++n_work;
    char cr = uart->USART_RX_REG & (uint16_t)0x0FF;
    // TODO: trylock

    if( last_uart_status & ( UART_FLAG_ORE | UART_FLAG_FE /*| UART_FLAG_LBD*/ ) ) { // TODO: on MCU
      UVAR('e') = last_uart_status;
      state = ST_ERR;
      return;
    }

    if( state == ST_MSG_IN ) {
      return;
    }

    if( state != ST_IDLE && state != ST_RECV ) {
      UVAR('x') = 1;
      UVAR('y') = state;
      state = ST_ERR;
      return;
    }

    if( i_pos >= bufsz-2) {
      state = ST_ERR;
      UVAR('x') = 2;
      return;
    }

    uint16_t t_c = *tim_cnt;

    if( state == ST_IDLE ) {
      t_char = t_c - 1;
    }
    uint16_t d_t = t_c - t_char;

    if( d_t < 20 ) { // TODO: config
      ibuf[i_pos++] = cr;
      state = ST_RECV;
      t_char = t_c;
    } else {
      UVAR('x') = 3;
      state = ST_ERR;
    }

    leds.reset( BIT1 );
  }

  // debug
  if( n_work == 0 ) { // unhandled
    leds.set( BIT0 );
  }
}

void MODBUS_RTU_server::handle_tick()
{
  if( state != ST_RECV ) {
    return;
  }
  uint16_t t_c = *tim_cnt;
  uint16_t d_t = t_c - t_char;
  if( d_t < 100 ) { // TODO: config
    return;
  }
  state = ST_MSG_IN;
  t_char = t_c;
}


