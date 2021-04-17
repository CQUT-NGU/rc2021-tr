/**
 * *****************************************************************************
 * @file         bsp_spi.c/h
 * @brief        spi of boards
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * @details      
 *               SPI5_SCK  ------> PF7
 *               SPI5_NSS  ------> PF6
 *               SPI5_MOSI ------> PF9
 *               SPI5_MISO ------> PF8
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_SPI_H__
#define __BSP_SPI_H__

/* Includes ------------------------------------------------------------------*/
#include "bsp_dma.h"
#include "main.h"

/* Private includes ----------------------------------------------------------*/
#include <stdint.h>

extern SPI_HandleTypeDef hspi5;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#undef __BEGIN_DECLS
#undef __END_DECLS

#if defined(__cplusplus)
#define __BEGIN_DECLS \
    extern "C"        \
    {
#define __END_DECLS }
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif /* __cplusplus */

/* Exported types ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

__BEGIN_DECLS

/**
 * @brief        Initializes the SPI5 DMA function
*/
extern void spi_dma_init(SPI_HandleTypeDef *hspi);

/**
 * @brief        Start SPI DMA
 * @param[in]    buf_tx: the buffer of transfer
 * @param[out]   buf_rx: the buffer of reciever
 * @param[in]    len: the length of buffer
*/
extern void spi_dma_start(SPI_HandleTypeDef *hspi,
                          uint32_t           buf_tx,
                          uint32_t           buf_rx,
                          uint16_t           len);

__END_DECLS

/* Private defines -----------------------------------------------------------*/

#define spi5_dma_init() spi_dma_init(&hspi5)
#define spi5_dma_start(tx, rx, len) \
                                    \
    spi_dma_start(&hspi5,           \
                  (uint32_t)(tx),   \
                  (uint32_t)(rx),   \
                  (uint16_t)(len))

/* __BSP_SPI_H__ -------------------------------------------------------------*/
#endif /* __BSP_SPI_H__ */

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
