/**
 * *****************************************************************************
 * @file         bsp_dma.h
 * @brief        flash of boards
 * @author       tqfx
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DMA_H__
#define __BSP_DMA_H__

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
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

/* Clear all flag */
#define BSP_DMA_CLEAR_FLAG(_)                                     \
                                                                  \
    do                                                            \
    {                                                             \
        /* Clear Complete Transmit flag */                        \
        __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_));  \
        /* Clear Half Complete Transmit flag */                   \
        __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_));  \
        /* Clear Transfer error flag */                           \
        __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_));  \
        /* Clear Direct mode error flag */                        \
        __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_)); \
        /* Clear FIFO error flag */                               \
        __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_));  \
    } while (0)

/* Clear Complete Transmit flag */
#define BSP_DMA_CLEAR_TC(_) \
                            \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_))

/* Clear Half Complete Transmit flag */
#define BSP_DMA_CLEAR_HT(_) \
                            \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_))

/* Clear Transfer error flag */
#define BSP_DMA_CLEAR_TE(_) \
                            \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_))

/* Clear Direct mode error flag */
#define BSP_DMA_CLEAR_DME(_) \
                             \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_))

/* Clear FIFO error flag */
#define BSP_DMA_CLEAR_FE(_) \
                            \
    __HAL_DMA_CLEAR_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_))

/* Get Complete Transmit flag */
#define BSP_DMA_FLAG_TC(_) \
                           \
    __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_))

/* Get Half Complete Transmit flag */
#define BSP_DMA_FLAG_HT(_) \
                           \
    __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_HT_FLAG_INDEX(_))

/* Get Transfer error flag */
#define BSP_DMA_FLAG_TE(_) \
                           \
    __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TE_FLAG_INDEX(_))

/* Get Direct mode error flag */
#define BSP_DMA_FLAG_DME(_) \
                            \
    __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_DME_FLAG_INDEX(_))

/* Get FIFO error flag */
#define BSP_DMA_FLAG_FE(_) \
                           \
    __HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_FE_FLAG_INDEX(_))

/* Wait Complete Transmit flag to be set */
#define BSP_DMA_WAIT_TC(_) \
                           \
    while (!__HAL_DMA_GET_FLAG(_, __HAL_DMA_GET_TC_FLAG_INDEX(_)))

/* Exported types ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

__BEGIN_DECLS

__END_DECLS

/* Private defines -----------------------------------------------------------*/

/* __BSP_DMA_H__ -------------------------------------------------------------*/
#endif /* __BSP_DMA_H__ */

/************************ (C) COPYRIGHT tqfx *******************END OF FILE****/
