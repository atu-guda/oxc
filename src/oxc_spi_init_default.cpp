#include <oxc_auto.h>

// --------------------------- SPI --------------------------------------

extern SPI_HandleTypeDef spi_h; // in main.c

int SPI_init_default( uint32_t baud_presc )
{
  spi_h.Instance               = BOARD_SPI_DEFAULT;
  spi_h.Init.Mode              = SPI_MODE_MASTER;
  spi_h.Init.Direction         = SPI_DIRECTION_2LINES;
  spi_h.Init.DataSize          = SPI_DATASIZE_8BIT;
  spi_h.Init.CLKPolarity       = SPI_POLARITY_LOW;
  spi_h.Init.CLKPhase          = SPI_PHASE_1EDGE;
  spi_h.Init.NSS               = SPI_NSS_SOFT;
  spi_h.Init.BaudRatePrescaler = baud_presc; // SPI_BAUDRATEPRESCALER_2; ... _256
  spi_h.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  spi_h.Init.TIMode            = SPI_TIMODE_DISABLED;
  spi_h.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLED;
  return HAL_SPI_Init( &spi_h );
}

void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi )
{
  GPIO_InitTypeDef gio;
  if( hspi->Instance == BOARD_SPI_DEFAULT ) {
    BOARD_SPI_DEFAULT_ENABLE;
    // SPI2 GPIO Configuration
    // B13 --> SPI2_SCK
    // B14 --> SPI2_MISO
    // B15 --> SPI2_MOSI
    // B1  --> manual NSS (separate init)

    gio.Mode      = GPIO_MODE_AF_PP;
    gio.Pull      = GPIO_NOPULL;
    gio.Speed     = GPIO_SPEED_MAX;
    gio.Alternate = BOARD_SPI_DEFAULT_GPIO_AF;
    gio.Pin       = BOARD_SPI_DEFAULT_GPIO_PIN_SCK;
    HAL_GPIO_Init( BOARD_SPI_DEFAULT_GPIO_SCK, &gio );
    gio.Pin       = BOARD_SPI_DEFAULT_GPIO_PIN_MISO;
    HAL_GPIO_Init( BOARD_SPI_DEFAULT_GPIO_MISO, &gio );
    gio.Pin       = BOARD_SPI_DEFAULT_GPIO_PIN_MOSI;
    HAL_GPIO_Init( BOARD_SPI_DEFAULT_GPIO_MOSI, &gio );
  }
}

void HAL_SPI_MspDeInit( SPI_HandleTypeDef* hspi )
{
  if( hspi->Instance == BOARD_SPI_DEFAULT ) {
    BOARD_SPI_DEFAULT_DISABLE;
    HAL_GPIO_DeInit( BOARD_SPI_DEFAULT_GPIO_SCK,  BOARD_SPI_DEFAULT_GPIO_PIN_SCK  );
    HAL_GPIO_DeInit( BOARD_SPI_DEFAULT_GPIO_MISO, BOARD_SPI_DEFAULT_GPIO_PIN_MISO );
    HAL_GPIO_DeInit( BOARD_SPI_DEFAULT_GPIO_MOSI, BOARD_SPI_DEFAULT_GPIO_PIN_MOSI );
  }
}
