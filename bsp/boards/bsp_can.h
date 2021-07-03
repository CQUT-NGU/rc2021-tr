/**
 * *****************************************************************************
 * @file         bsp_can.h
 * @brief        can of boards
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * @details
 *               CAN1_RX ------> PD0
 *               CAN1_TX ------> PD1
 *               CAN2_RX ------> PB12
 *               CAN2_TX ------> PB13
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include <stdint.h>

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

#ifndef CAN2_IS_ENABLE
#define CAN2_IS_ENABLE 0
#endif /* CAN2_IS_ENABLE */

__BEGIN_DECLS

/**
 * @brief        Intialize the CAN filter
*/
void can_filter_init(void);

__END_DECLS

/* Terminate definition to prevent recursive inclusion -----------------------*/
#endif /* __BSP_CAN_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
