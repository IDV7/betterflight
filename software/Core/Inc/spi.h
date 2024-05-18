
#include "stm32f7xx.h"

#ifndef BETTERFLIGHT_SPI_H
#define BETTERFLIGHT_SPI_H

typedef struct {
    SPI_TypeDef *SPIx; // CMSIS SPI peripheral "registers"
    SPI_GPIO mosi;
    SPI_GPIO miso;
    SPI_GPIO sck;
    SPI_GPIO cs;
}SPI_handle_t;

void SPI_init(SPI_handle_t *hspi);

#endif //BETTERFLIGHT_SPI_H
