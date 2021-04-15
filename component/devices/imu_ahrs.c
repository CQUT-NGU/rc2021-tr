/**
 * *****************************************************************************
 * @file         imu_ahrs.c/h
 * @brief        Automatic Heading Reference System by imu
 * @author       tqfx
 * @date         20210101
 * @version      1
 * @copyright    Copyright (C) 2021
 * @code         utf-8                                                  @endcode
 * *****************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "imu_ahrs.h"

/* Private includes ----------------------------------------------------------*/
#include "ahrs.h"
#include "cc.h"
#include "main.h"
#include "mpu6500.h"

#include <math.h>

extern imu_t imu;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static float quat[4] = {1.0f, 0.0f, 0.0f, 0.0f};

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief        Gets half of the sampling cycle time
 * @return       float
*/
static float ht_get(void);

/**
 * @brief        Gets the sampling cycle time
 * @return       float
*/
static float t_get(void);

/* Private user code ---------------------------------------------------------*/

static float ht_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now  = HAL_GetTick(); /* ms */
    float ret   = (float)(update_now - update_last) / 2000.0f;
    update_last = update_now;

    return ret;
}

static float t_get(void)
{
    static volatile uint32_t update_last;
    static volatile uint32_t update_now;

    update_now  = HAL_GetTick(); /* ms */
    float ret   = (float)(update_now - update_last) / 1000.0f;
    update_last = update_now;

    return ret;
}

/**
 * @brief        Initialize quaternion
*/
void imu_quat_init(void)
{
    ahrs_quat_init(quat, imu.mx, imu.my);
}

/**
 * @brief        Update ahrs by imu
*/
void imu_update_ahrs(void)
{
    volatile float g[3] = {imu.wx, imu.wy, imu.wz};
    volatile float a[3] = {imu.ax, imu.ay, imu.az};
    volatile float m[3] = {imu.mx, imu.my, imu.mz};

    ahrs_madgwick(quat, g, a, m, t_get());
    //ahrs_mahony(quat, g, a, m, ht_get());
    //ahrs_mahony_imu(quat, g, a, ht_get());
}

/**
 * @brief        update imu attitude
*/
void imu_update_attitude(void)
{
    ahrs_euler_angle(quat, &imu.rol, &imu.pit, &imu.yaw);
}

/************************ (C) COPYRIGHT tqfx *******************END OF FILE****/
