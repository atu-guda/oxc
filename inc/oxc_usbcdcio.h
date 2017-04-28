#ifndef _OXC_USBCDCIO_H
#define _OXC_USBCDCIO_H

#include <oxc_devio.h>

#include <usbd_cdc.h>

extern PCD_HandleTypeDef hpcd;
extern USBD_DescriptorsTypeDef VCP_Desc;
extern USBD_HandleTypeDef USBD_Dev;

class UsbcdcIO : public DevIO {
  public:
   UsbcdcIO()
    { pusb_dev = &usb_dev; static_usbcdcio = this; }
   // virtual ~UsbcdcIO() override {};
   // virtual void reset() override {}
   int init();

   virtual int sendBlockSync( const char *s, int l ) override;
   //{ usb->transmit( s, l ); return l; } // TODO: check return status

   virtual int recvBytePoll( char *b, int w_tick = 0 ) override
   {
     return recvByte( b, w_tick ); // no special
   };

   virtual int  setAddrLen( int addrLen ) override { return 0; };
   virtual int  getAddrLen() const override { return 0; };
   virtual int  setAddr( uint32_t addr ) override { return 0; };

   USBD_CDC_LineCodingTypeDef* getCdcLineCoding() { return &lineCoding; }

   static int8_t CDC_Itf_Init();
   static int8_t CDC_Itf_DeInit();
   static int8_t CDC_Itf_Control( uint8_t cmd, uint8_t* pbuf, uint16_t length );
   static int8_t CDC_Itf_Receive( uint8_t* pbuf, uint32_t *Len );


  protected:
   USBD_HandleTypeDef usb_dev;
   static USBD_HandleTypeDef *pusb_dev;
   static UsbcdcIO *static_usbcdcio;
   USBD_CDC_LineCodingTypeDef lineCoding =
   { 115200, /* baud rate*/ 0x00, /* stop bits-1*/  0x00,   /* parity - none*/  0x08 /* nb. of bits 8*/
   };
   char rx_buf[TX_BUF_SIZE];
   USBD_CDC_ItfTypeDef cdc_fops =
   {
     CDC_Itf_Init,
     CDC_Itf_DeInit,
     CDC_Itf_Control,
     CDC_Itf_Receive
   };
};

// common declarations
extern "C" {
  void task_usbcdc_send( void *prm UNUSED_ARG );
  void task_usbcdc_recv( void *prm UNUSED_ARG );
}

#define STD_USBCDC_RECV_TASK( obj ) STD_COMMON_RECV_TASK( task_usbcdc_recv, obj )
#define STD_USBCDC_SEND_TASK( obj ) STD_COMMON_SEND_TASK( task_usbcdc_send, obj )

#endif
// vim: path=.,/usr/share/stm32lib/inc/,/usr/arm-none-eabi/include
