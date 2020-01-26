#include <errno.h>
#include <oxc_base.h>
#include <oxc_gpio.h>

#include <usbd_core.h>


PCD_HandleTypeDef hpcd;
void default_USBFS_MspInit(void);
USBD_StatusTypeDef USBD_Get_USB_Status( HAL_StatusTypeDef hal_status );









void HAL_PCD_MspInit(PCD_HandleTypeDef* pcdHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if(pcdHandle->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspInit 0 */

  /* USER CODE END USB_OTG_FS_MspInit 0 */
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USB_OTG_FS GPIO Configuration    
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();

    /* Peripheral interrupt init */
    HAL_NVIC_SetPriority(OTG_FS_EP1_OUT_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_EP1_OUT_IRQn);
    HAL_NVIC_SetPriority(OTG_FS_EP1_IN_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_EP1_IN_IRQn);
    HAL_NVIC_SetPriority(OTG_FS_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
  /* USER CODE BEGIN USB_OTG_FS_MspInit 1 */

  /* USER CODE END USB_OTG_FS_MspInit 1 */
  }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef* pcdHandle)
{
  if(pcdHandle->Instance==USB_OTG_FS)
  {
  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 0 */

  /* USER CODE END USB_OTG_FS_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
  
    /**USB_OTG_FS GPIO Configuration    
    PA11     ------> USB_OTG_FS_DM
    PA12     ------> USB_OTG_FS_DP 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(OTG_FS_EP1_OUT_IRQn);

    HAL_NVIC_DisableIRQ(OTG_FS_EP1_IN_IRQn);

    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

  /* USER CODE BEGIN USB_OTG_FS_MspDeInit 1 */

  /* USER CODE END USB_OTG_FS_MspDeInit 1 */
  }
}


/*******************************************************************************
                       LL Driver Callbacks (PCD -> USB Device Library)
*******************************************************************************/


/**
  * @brief  Setup stage callback
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_SetupStageCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_SetupStageCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_SetupStage( (USBD_HandleTypeDef*)hpcd->pData, (uint8_t *)hpcd->Setup );
}

/**
  * @brief  Data Out stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_DataOutStageCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#else
void HAL_PCD_DataOutStageCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DataOutStage( (USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff );
}

/**
  * @brief  Data In stage callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_DataInStageCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#else
void HAL_PCD_DataInStageCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DataInStage( (USBD_HandleTypeDef*)hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff );
}

/**
  * @brief  SOF callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_SOFCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_SOFCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_SOF( (USBD_HandleTypeDef*)hpcd->pData );
}

/**
  * @brief  Reset callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_ResetCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_ResetCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_SpeedTypeDef speed = USBD_SPEED_FULL; // atu: only full
  USBD_LL_SetSpeed( (USBD_HandleTypeDef*)hpcd->pData, speed );
  USBD_LL_Reset( (USBD_HandleTypeDef*)hpcd->pData );
}

/**
  * @brief  Suspend callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_SuspendCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_SuspendCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  /* Inform USB library that core enters in suspend Mode. */
  USBD_LL_Suspend( (USBD_HandleTypeDef*)hpcd->pData );
  __HAL_PCD_GATE_PHYCLOCK( hpcd );
  if( hpcd->Init.low_power_enable )
  {
    SCB->SCR |= (uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk);
  }
}

/**
  * @brief  Resume callback.
  * When Low power mode is enabled the debug cannot be used (IAR, Keil doesn't support it)
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_ResumeCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_ResumeCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_Resume( (USBD_HandleTypeDef*)hpcd->pData );
}

/**
  * @brief  ISOOUTIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_ISOOUTIncompleteCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#else
void HAL_PCD_ISOOUTIncompleteCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_IsoOUTIncomplete( (USBD_HandleTypeDef*)hpcd->pData, epnum );
}

/**
  * @brief  ISOINIncomplete callback.
  * @param  hpcd: PCD handle
  * @param  epnum: Endpoint number
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_ISOINIncompleteCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#else
void HAL_PCD_ISOINIncompleteCallback( PCD_HandleTypeDef *hpcd, uint8_t epnum )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_IsoINIncomplete( (USBD_HandleTypeDef*)hpcd->pData, epnum );
}

/**
  * @brief  Connect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_ConnectCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_ConnectCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DevConnected( (USBD_HandleTypeDef*)hpcd->pData );
}

/**
  * @brief  Disconnect callback.
  * @param  hpcd: PCD handle
  * @retval None
  */
#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
static void PCD_DisconnectCallback( PCD_HandleTypeDef *hpcd )
#else
void HAL_PCD_DisconnectCallback( PCD_HandleTypeDef *hpcd )
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
{
  USBD_LL_DevDisconnected( (USBD_HandleTypeDef*)hpcd->pData );
}

/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/
/**
  * @brief  Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Init( USBD_HandleTypeDef *pdev )
{
  /* Init USB Ip. */
  if (pdev->id == DEVICE_FS) {
  /* Link the driver to the stack. */
  hpcd.pData = pdev;
  pdev->pData = &hpcd;

  hpcd.Instance = USB_OTG_FS;
  hpcd.Init.dev_endpoints = 9;
  hpcd.Init.speed = PCD_SPEED_FULL;
  hpcd.Init.dma_enable = DISABLE;
  hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd.Init.Sof_enable = DISABLE;
  hpcd.Init.low_power_enable = DISABLE;
  hpcd.Init.lpm_enable = DISABLE;
  hpcd.Init.battery_charging_enable = DISABLE;
  hpcd.Init.vbus_sensing_enable = DISABLE;
  hpcd.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd) != HAL_OK)
  {
    // Error_Handler( );
  }

#if ( USE_HAL_PCD_REGISTER_CALLBACKS != 0 )
#warning "USE_HAL_PCD_REGISTER_CALLBACKS != 0"
  /* Register USB PCD CallBacks */
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_SOF_CB_ID, PCD_SOFCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_SETUPSTAGE_CB_ID, PCD_SetupStageCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_RESET_CB_ID, PCD_ResetCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_SUSPEND_CB_ID, PCD_SuspendCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_RESUME_CB_ID, PCD_ResumeCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_CONNECT_CB_ID, PCD_ConnectCallback);
  HAL_PCD_RegisterCallback(&hpcd, HAL_PCD_DISCONNECT_CB_ID, PCD_DisconnectCallback);

  HAL_PCD_RegisterDataOutStageCallback(&hpcd, PCD_DataOutStageCallback);
  HAL_PCD_RegisterDataInStageCallback(&hpcd, PCD_DataInStageCallback);
  HAL_PCD_RegisterIsoOutIncpltCallback(&hpcd, PCD_ISOOUTIncompleteCallback);
  HAL_PCD_RegisterIsoInIncpltCallback(&hpcd, PCD_ISOINIncompleteCallback);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */

  HAL_PCDEx_SetRxFiFo( &hpcd, 0x80 );
  HAL_PCDEx_SetTxFiFo( &hpcd, 0, 0x40 );
  HAL_PCDEx_SetTxFiFo( &hpcd, 1, 0x80 );
  }
  return USBD_OK;
}

/**
  * @brief  De-Initializes the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_DeInit( USBD_HandleTypeDef *pdev )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_DeInit( (PCD_HandleTypeDef*)pdev->pData ) );
  return rc;
}

/**
  * @brief  Starts the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Start( USBD_HandleTypeDef *pdev )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_Start( (PCD_HandleTypeDef*)pdev->pData ) );
  return rc;
}

/**
  * @brief  Stops the low level portion of the device driver.
  * @param  pdev: Device handle
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Stop( USBD_HandleTypeDef *pdev )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_Stop( (PCD_HandleTypeDef*)pdev->pData ) );
  return rc;
}

/**
  * @brief  Opens an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  ep_type: Endpoint type
  * @param  ep_mps: Endpoint max packet size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_OpenEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type, uint16_t ep_mps )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_Open( (PCD_HandleTypeDef*)pdev->pData, ep_addr, ep_mps, ep_type ) );
  return rc;
}

/**
  * @brief  Closes an endpoint of the low level driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_CloseEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_Close( (PCD_HandleTypeDef*)pdev->pData, ep_addr ) );
  return rc;
}

/**
  * @brief  Flushes an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_FlushEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_Flush( (PCD_HandleTypeDef*)pdev->pData, ep_addr ) );
  return rc;
}

/**
  * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_StallEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_SetStall( (PCD_HandleTypeDef*)pdev->pData, ep_addr ) );
  return rc;
}

/**
  * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_ClearStallEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_ClrStall( (PCD_HandleTypeDef*)pdev->pData, ep_addr ) );
  return rc;
}

/**
  * @brief  Returns Stall condition.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Stall (1: Yes, 0: No)
  */
uint8_t USBD_LL_IsStallEP( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*)pdev->pData;

  if( (ep_addr & 0x80 ) == 0x80 ) {
    return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
  } else {
    return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
  }
}

/**
  * @brief  Assigns a USB address to the device.
  * @param  pdev: Device handle
  * @param  dev_addr: Device address
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_SetUSBAddress( USBD_HandleTypeDef *pdev, uint8_t dev_addr )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_SetAddress( (PCD_HandleTypeDef*)pdev->pData, dev_addr ) );
  return rc;
}

/**
  * @brief  Transmits data over an endpoint.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be sent
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_Transmit( USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_Transmit( (PCD_HandleTypeDef*)pdev->pData, ep_addr, pbuf, size ) );
  return rc;
}

/**
  * @brief  Prepares an endpoint for reception.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @param  pbuf: Pointer to data to be received
  * @param  size: Data size
  * @retval USBD status
  */
USBD_StatusTypeDef USBD_LL_PrepareReceive( USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size )
{
  USBD_StatusTypeDef rc = USBD_Get_USB_Status( HAL_PCD_EP_Receive( (PCD_HandleTypeDef*)pdev->pData, ep_addr, pbuf, size ) );
  return rc;
}

/**
  * @brief  Returns the last transfered packet size.
  * @param  pdev: Device handle
  * @param  ep_addr: Endpoint number
  * @retval Recived Data Size
  */
uint32_t USBD_LL_GetRxDataSize( USBD_HandleTypeDef *pdev, uint8_t ep_addr )
{
  return HAL_PCD_EP_GetRxCount( (PCD_HandleTypeDef*)pdev->pData, ep_addr );
}

/**
  * @brief  Delays routine for the USB Device Library.
  * @param  Delay: Delay in ms
  * @retval None
  */
void USBD_LL_Delay( uint32_t delay )
{
  // HAL_Delay( delay );
  // leds.set( BIT1 );
  delay_ms( delay );
  // leds.reset( BIT1 );
  // delay_bad_ms( delay );
}

/**
  * @brief  Retuns the USB status depending on the HAL status:
  * @param  hal_status: HAL status
  * @retval USB status
  */
USBD_StatusTypeDef USBD_Get_USB_Status( HAL_StatusTypeDef hal_status )
{
  USBD_StatusTypeDef usb_status = USBD_OK;

  switch (hal_status)
  {
    case HAL_OK :
      usb_status = USBD_OK;
      break;
    case HAL_ERROR:
      usb_status = USBD_FAIL;
      break;
    case HAL_BUSY:
      usb_status = USBD_BUSY;
      break;
    case HAL_TIMEOUT :
      usb_status = USBD_FAIL;
      break;
    default:
      usb_status = USBD_FAIL;
      break;
  }
  return usb_status;
}


