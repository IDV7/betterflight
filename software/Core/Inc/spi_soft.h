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



void SPI_Init(SPI_handle_t *spi_h, uint32_t spi_speed_hz);
void SPI_transmit_receive(SPI_handle_t *spi_h, uint8_t *tx_data, uint8_t *rx_data, uint32_t len);






#endif //BETTERFLIGHT_SPI_SOFT_H
