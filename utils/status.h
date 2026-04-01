#ifndef BW_STATUS_H
#define BW_STATUS_H

typedef enum 
{
    // General
    STATUS_OK,
    STATUS_ERR,
    STATUS_I2C_REPEATED_START,
    STATUS_I2C_NACKF,
    STATUS_I2C_ERR,
    STATUS_SPI_OVR,
    STATUS_SPI_MODF,
    STATUS_DMA_TERR,
    STATUS_DMA_TC,
    STATUS_DMA_HTC
} bw_status_t;

#endif
