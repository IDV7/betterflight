


#include "spi_soft.h"
#include "misc.h"
#include "log.h"

__attribute__((optimize("O0")))  void SPI_Delay(uint32_t cycles);
static uint32_t calc_nops_spi_half_cycle(uint32_t mcu_clk_speed_hz, uint32_t spi_clock_speed_hz);



#define MOSI_HIGH spi_h->mosi.port->BSRR = (spi_h->mosi.pin)
#define MOSI_LOW spi_h->mosi.port->BSRR = (spi_h->mosi.pin << 16)

#define SCK_HIGH spi_h->sck.port->BSRR = (spi_h->sck.pin)
#define SCK_LOW spi_h->sck.port->BSRR = (spi_h->sck.pin << 16)

#define CS_HIGH spi_h->cs.port->BSRR = (spi_h->cs.pin)
#define CS_LOW spi_h->cs.port->BSRR = (spi_h->cs.pin << 16)


// Initialize the SPI pins (requires the user to have configured the pins a bit already... (like clocks and stuff))
void SPI_soft_init(SPI_handle_t *spi_h) {

    // calc number of NOPs to delay for half of the SPI clock cycle
//    spi_h->nop_cnt = calc_nops_spi_half_cycle(get_mcu_clock_speed(), spi_speed_hz);

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //init mosi
    GPIO_InitStruct.Pin = spi_h->mosi.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->mosi.port, &GPIO_InitStruct);
    MOSI_LOW;

    //init miso
    GPIO_InitStruct.Pin = spi_h->miso.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(spi_h->miso.port, &GPIO_InitStruct);

    //init sck
    GPIO_InitStruct.Pin = spi_h->sck.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->sck.port, &GPIO_InitStruct);
    SCK_HIGH;

    //init cs
    GPIO_InitStruct.Pin = spi_h->cs.pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(spi_h->cs.port, &GPIO_InitStruct);
    CS_HIGH;

}


#define NOP_CNT 1 //5
void SPI_soft_trx(SPI_handle_t *spi_h, const uint8_t *tx_data, uint8_t *rx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    CS_LOW;
    SPI_Delay(NOP_CNT);

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];
        uint8_t rx_byte = 0;

        for (uint8_t j = 0; j < 8; j++) { //for each bit

            // set mosi
            if (tx_byte & 0x80) {
                MOSI_HIGH;
            } else {
                MOSI_LOW;
            }

            // shift tx_byte
            tx_byte <<= 1;

            SCK_LOW;
            //SPI_Delay(NOP_CNT);

            // read miso
            rx_byte <<= 1;
            if (spi_h->miso.port->IDR & spi_h->miso.pin) { // if miso is high
                rx_byte |= 0x01; // set the last bit
            }

            SCK_HIGH;
            //SPI_Delay(NOP_CNT);
        }
        MOSI_LOW;

        rx_data[i] = rx_byte;
    }

    // disable cs
    CS_HIGH;

    // enable interrupts again
    __enable_irq();
}

void SPI_soft_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    CS_LOW;
    SPI_Delay(NOP_CNT);

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];

        for (uint8_t j = 0; j < 8; j++) { //for each bit

            // set mosi
            if (tx_byte & 0x80) {
                MOSI_HIGH;
            } else {
                MOSI_LOW;
            }

            // shift tx_byte
            tx_byte <<= 1;

            SCK_LOW;
            SPI_Delay(NOP_CNT);

            SCK_HIGH;
            SPI_Delay(NOP_CNT);
        }
        MOSI_LOW;
    }

    // disable cs
    CS_HIGH;

    // enable interrupts again
    __enable_irq();
}

void SPI_soft_burst_tx(SPI_handle_t *spi_h, uint8_t *tx_data, uint32_t len) {

    // turn off interrupts
    __disable_irq();

    // enable cs
    CS_LOW;
    SPI_Delay(NOP_CNT);

    for (uint32_t i = 0; i < len; i++) { //for each byte
        uint8_t tx_byte = tx_data[i];

        for (uint8_t j = 0; j < 8; j++) { //for each bit

            // set mosi
            if (tx_byte & 0x80) {
                MOSI_HIGH;
            } else {
                MOSI_LOW;
            }

            // shift tx_byte
            tx_byte <<= 1;

            SCK_LOW;
            SPI_Delay(NOP_CNT);

            SCK_HIGH;
            SPI_Delay(NOP_CNT);
        }
        MOSI_LOW;
    }

    // disable cs
    CS_HIGH;

    // enable interrupts again
    __enable_irq();
}

void SPI_soft_cs_high(SPI_handle_t *spi_h) {
    CS_HIGH;
}

void SPI_soft_cs_low(SPI_handle_t *spi_h) {
    CS_LOW;
}




__attribute__((optimize("O0")))  void SPI_Delay(uint32_t cycles) {
    while (cycles--) {__NOP();}
}

// calc delay in nop cycles for given spi speed and mcu speed
uint32_t calc_nops_spi_half_cycle(uint32_t mcu_clk_speed_hz, uint32_t spi_clock_speed_hz) {
    //calc period of spi clock in ns
    uint32_t spiPeriodNs = 1000000000 / spi_clock_speed_hz;

    //calc period of mcu clock in ns
    uint32_t mcuCycleDurationNs = 1000000000 / mcu_clk_speed_hz;

    //calc number of mcu cycles in one spi clock period
    uint32_t nopCount = spiPeriodNs / mcuCycleDurationNs / 2;


//    LOGD("nopCount: %d", nopCount);
    return nopCount;
}