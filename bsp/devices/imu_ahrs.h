/**
 * *****************************************************************************
 * @file         imu_ahrs.c/h
 * @brief        Automatic Heading Reference System by imu
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __IMU_AHRS_H__
#define __IMU_AHRS_H__

/* Includes ------------------------------------------------------------------*/
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

/* Exported types ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

__BEGIN_DECLS

/**
 * @brief        Initialize quaternion
*/
extern void imu_quat_init(void);

/**
 * @brief        Update ahrs by imu
*/
extern void imu_update_ahrs(void);

/**
 * @brief        update imu attitude
*/
extern void imu_update_attitude(void);

__END_DECLS

/* Private defines -----------------------------------------------------------*/

/* __IMU_AHRS_H__ ------------------------------------------------------------*/
#endif /* __IMU_AHRS_H__ */

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
