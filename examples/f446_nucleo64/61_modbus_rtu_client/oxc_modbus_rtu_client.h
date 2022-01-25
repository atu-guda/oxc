#ifndef _OXC_MODBUS_RTU_CLIENT_H
#define _OXC_MODBUS_RTU_CLIENT_H

class MODBUS_RTU_client {
  public:
    enum client_state {
      ST_INIT = 0,
      ST_IDLE = 1,
      ST_RECV = 2,
      ST_ERR  = 3
    };
    static const uint16_t bufsz = 256;
    MODBUS_RTU_client( USART_TypeDef *a_uart, volatile uint32_t *a_tim_cnt );
    static uint16_t crc( const uint8_t *s, uint16_t l );
    const uint8_t* get_ibuf() const { return ibuf; }
    const uint8_t* get_obuf() const { return obuf; }
    uint16_t get_ibuf_pos() const { return i_pos; }
    uint16_t get_obuf_pos() const { return o_pos; }
    uint32_t get_last_uart_status() const { return last_uart_status; }
    client_state get_client_state() const { return state; }
    void reset() { i_pos = o_pos = 0; state = ST_INIT; last_uart_status = 0; }
    void handle_UART_IRQ();
    void handle_tick();
  private:
    static const uint8_t CRC_hi[256];
    static const uint8_t CRC_lo[256];

    uint8_t  ibuf[bufsz];
    uint8_t obuf[bufsz];
    USART_TypeDef *uart;
    volatile uint32_t *tim_cnt;
    uint16_t i_pos = 0, o_pos = 0;
    uint16_t t_char = 0;
    client_state state = ST_INIT;
    uint32_t last_uart_status = 0;

};



#endif

