/**
 * *****************************************************************************
 * @file         bsp_imu_pwm.h
 * @brief        The IMU temperature is controlled by PWM
 * @author       NGU
 * @date         20210427
 * @version      1
 * @copyright    Copyright (C) 2021 NGU
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_IMU_PWM_H__
#define __BSP_IMU_PWM_H__

#include "bsp.h"

#define IMU_PWM_PSC 180
#define IMU_PWM_MAX 0x3FFF

__BEGIN_DECLS

/**
 * @brief        Start imu pwm
*/
extern void imu_pwm_start(void);

/**
 * @brief        Stop imu pwm
*/
extern void imu_pwm_stop(void);

/**
 * @brief        Set pwm of the imu
 * @param[in]    pwm: 0x0000 ~ IMU_PWM_MAX
*/
extern void imu_pwm_set(uint16_t pwm);

__END_DECLS

/* Enddef to prevent recursive inclusion ------------------------------------ */
#endif /* __BSP_IMU_PWM_H__ */

/************************ (C) COPYRIGHT NGU ********************END OF FILE****/
