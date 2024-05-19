#ifndef BETTERFLIGHT_SPI_SOFT_H
#define BETTERFLIGHT_SPI_SOFT_H

#include "stm32f7xx_hal.h"

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
} SPI_GPIO;

typedef struct {
    SPI_GPIO mosi;
    SPI_GPIO miso;
    SPI_GPIO sck;
    SPI_GPIO cs;
    uint32_t nop_cnt;  // amount of NOPs to delay for half of the SPI clock cycle

} SPI_handle_t;



void SPI_soft_init(SPI_handle_t *spi_h);
void SPI_soft_trx(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len);
void SPI_soft_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len);
void SPI_soft_burst_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len);
void SPI_soft_cs_high(SPI_handle_t *spi_h);
void SPI_soft_cs_low(SPI_handle_t *spi_h);




#endif //BETTERFLIGHT_SPI_SOFT_H