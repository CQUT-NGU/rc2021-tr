/**
 * *****************************************************************************
 * @file         mpu6500.c/h
 * @brief        mpu6500
 * @author       ngu
 * @date         20210427
 * @version      1
 * @copyright    Copyright (c) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MPU6500_H__
#define __MPU6500_H__

/* Includes ------------------------------------------------------------------*/
#include "mpu6500reg.h"

/* Private includes ----------------------------------------------------------*/
#include <stdint.h>

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

typedef struct
{
    int16_t ax;
    int16_t ay;
    int16_t az;

    int16_t mx;
    int16_t my;
    int16_t mz;

    int16_t temp; /* temperature */

    int16_t gx;
    int16_t gy;
    int16_t gz;

    int16_t ax_offset;
    int16_t ay_offset;
    int16_t az_offset;

    int16_t gx_offset;
    int16_t gy_offset;
    int16_t gz_offset;
} mpu_t;

typedef struct
{
    int16_t ax; /* accelerometer x axis */
    int16_t ay; /* accelerometer y axis */
    int16_t az; /* accelerometer z axis */

    int16_t mx; /* geomagnetic x axis */
    int16_t my; /* geomagnetic y axis */
    int16_t mz; /* geomagnetic z axis */

    float temp; /* temperature */

    /*!< omiga, +- 2000dps => +-32768  so gx/16.384/57.3 = rad/s */
    float wx; /* gyroscope x axis */
    float wy; /* gyroscope y axis */
    float wz; /* gyroscope z axis */

    float vx;
    float vy;
    float vz;

    float rol; /* roll */
    float pit; /* pitch */
    float yaw; /* yaw */

} imu_t;

/* Exported functions prototypes ---------------------------------------------*/

__BEGIN_DECLS

extern void mpu_data_update(void);

extern void mpu_device_init(void);

__END_DECLS

/* Private defines -----------------------------------------------------------*/

/* __MPU6500_H__ -------------------------------------------------------------*/
#endif /* __MPU6500_H__ */

/************************ (C) COPYRIGHT ngu ********************END OF FILE****/
